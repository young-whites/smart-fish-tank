/**
 * @file    bsp_esp01s.c
 * @brief   ESP-01S WiFi module driver (AP mode + TCP server + custom protocol)
 *
 * Hardware: ESP-01S on USART2 (PA2=TX, PA3=RX), 115200 baud
 * Note:     RST/EN pins are NOT physically connected on this board.
 * Protocol: 0xAA | CMD | LEN | PAYLOAD | CHECKSUM | 0x55
 * TCP port: 8080
 */

#include "bsp_esp01s.h"
#include "bsp_led.h"
#include "systick_delay.h"
#include "MyTypedef.h"
#include <stdio.h>
#include <string.h>

/* ======================== Configuration ======================== */
#define ESP01S_USART            USART2
#define ESP01S_BAUDRATE         115200
#define ESP01S_RINGBUF_SIZE     512
#define ESP01S_MAX_LINKS        5       /* CIPMUX=1 supports up to 5 links */
#define ESP01S_TX_BUF_SIZE      128     /* AT+CIPSEND staging buffer */
#define ESP01S_AT_TIMEOUT_MS    3000

/* Frame constants */
#define FRAME_HEADER            0xAA
#define FRAME_END               0x55
#define FRAME_OVERHEAD          5       /* HEAD+CMD+LEN+SUM+END */

/* TX commands (MCU -> APP) */
#define CMD_SENSOR_DATA         0x01    /* Sensor data upload (14 bytes) */
#define CMD_DEVICE_STATUS       0x02    /* Device status upload (5 bytes) */
#define CMD_ALARM_EVENT         0x03    /* Alarm event (2 bytes) */

/* RX commands (APP -> MCU) */
#define CMD_ECHO_REQUEST        0x10    /* APP sends echo; if LEN=0 send DeviceStatus */
#define CMD_SWITCH_MODE         0x11    /* APP -> MCU: switch auto/manual mode */
#define CMD_CONTROL_RELAY       0x12    /* APP -> MCU: control relay */
#define CMD_SYNC_THRESHOLDS     0x13    /* APP -> MCU: sync thresholds (20 bytes) */
#define CMD_TRIGGER_FEED        0x14    /* APP -> MCU: trigger feeding */
#define CMD_SET_ALARM           0x15    /* APP -> MCU: set alarm enable/disable */
#define CMD_SET_FEED_INTERVAL   0x16    /* APP -> MCU: set feed interval (2 bytes BE) */
#define CMD_LED_CONTROL         0x20    /* APP controls LED: payload=[led_id, state] */

/* ======================== Ring Buffer ======================== */
static volatile uint8_t  s_ringBuf[ESP01S_RINGBUF_SIZE];
static volatile uint16_t s_ringHead = 0;   /* Write index (ISR) */
static volatile uint16_t s_ringTail = 0;   /* Read index (main) */

/* ======================== State ======================== */
uint8_t ESP01S_WiFiConnected = 0;
static uint8_t s_clientLinkId = 0;         /* Connected client link ID */
static uint8_t s_clientConnected = 0;      /* 0=no client, 1=client connected */
static volatile uint32_t s_connectCount = 0;   /* Debug: total connections received */
static volatile uint32_t s_disconnectCount = 0; /* Debug: total disconnections */
static volatile uint32_t s_ipdCount = 0;        /* Debug: total +IPD received */
static volatile uint8_t  s_statusDirty = 1;     /* Flag: DeviceStatus needs re-send */

/* ======================== USART2 Init ======================== */
static void ESP01S_USART_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* Enable clocks: GPIOA on APB2, USART2 on APB1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* PA2: TX - Alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA3: RX - Floating input */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART2 configuration */
    USART_InitStructure.USART_BaudRate            = ESP01S_BAUDRATE;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(ESP01S_USART, &USART_InitStructure);

    /* NVIC: USART2 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(ESP01S_USART, USART_IT_RXNE, ENABLE);
    USART_Cmd(ESP01S_USART, ENABLE);
}

/* ======================== USART2 IRQ Handler ======================== */
/**
 * @brief  USART2 interrupt handler - push received bytes into ring buffer.
 */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(ESP01S_USART, USART_IT_RXNE) != RESET) {
        uint8_t ch = (uint8_t)USART_ReceiveData(ESP01S_USART);
        uint16_t next = (s_ringHead + 1) % ESP01S_RINGBUF_SIZE;
        if (next != s_ringTail) {  /* Buffer not full */
            s_ringBuf[s_ringHead] = ch;
            s_ringHead = next;
        }
        USART_ClearITPendingBit(ESP01S_USART, USART_IT_RXNE);
    }
}

/* ======================== Low-level helpers ======================== */

/** Wait for USART TX register empty, then send one byte */
static void USART_SendByte(uint8_t ch)
{
    while (USART_GetFlagStatus(ESP01S_USART, USART_FLAG_TXE) == RESET);
    USART_SendData(ESP01S_USART, ch);
}

/** Send a C string via USART2 (blocking) */
static void USART_SendString(const char* str)
{
    while (*str) {
        USART_SendByte((uint8_t)*str++);
    }
    while (USART_GetFlagStatus(ESP01S_USART, USART_FLAG_TC) == RESET);
}

/** Send a raw byte array via USART2 (blocking) */
static void USART_SendBytes(const uint8_t* data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++) {
        USART_SendByte(data[i]);
    }
    while (USART_GetFlagStatus(ESP01S_USART, USART_FLAG_TC) == RESET);
}

/** Check if ring buffer contains a specific string */
static int RingBuf_Contains(const char* keyword)
{
    uint16_t len = strlen(keyword);
    uint16_t count, i;

    if (s_ringHead >= s_ringTail)
        count = s_ringHead - s_ringTail;
    else
        count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

    if (count < len) return 0;

    for (i = 0; i <= count - len; i++) {
        uint16_t idx = (s_ringTail + i) % ESP01S_RINGBUF_SIZE;
        uint16_t j;
        int match = 1;
        for (j = 0; j < len; j++) {
            if (s_ringBuf[(idx + j) % ESP01S_RINGBUF_SIZE] != (uint8_t)keyword[j]) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

/** Discard bytes from ring buffer up to and including 'keyword', return 1 if found */
static int RingBuf_SkipUntil(const char* keyword)
{
    uint16_t len = strlen(keyword);
    uint16_t count, i;

    if (s_ringHead >= s_ringTail)
        count = s_ringHead - s_ringTail;
    else
        count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

    if (count < len) return 0;

    for (i = 0; i <= count - len; i++) {
        uint16_t idx = (s_ringTail + i) % ESP01S_RINGBUF_SIZE;
        uint16_t j;
        int match = 1;
        for (j = 0; j < len; j++) {
            if (s_ringBuf[(idx + j) % ESP01S_RINGBUF_SIZE] != (uint8_t)keyword[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            s_ringTail = (idx + len) % ESP01S_RINGBUF_SIZE;
            return 1;
        }
    }
    return 0;
}

/** Flush ring buffer */
static void RingBuf_Flush(void)
{
    s_ringTail = s_ringHead;
}

/* ======================== AT Command Engine ======================== */

/**
 * @brief  Send AT command and wait for expected response.
 * @param  cmd: AT command string (without \r\n)
 * @param  ack: Expected acknowledgement string (e.g. "OK", "SEND OK")
 * @param  timeout_ms: Timeout in milliseconds
 * @retval 1=success, 0=timeout
 */
static uint8_t ESP01S_SendCmd(const char* cmd, const char* ack, uint16_t timeout_ms)
{
    uint16_t elapsed = 0;

    RingBuf_Flush();
    USART_SendString(cmd);
    USART_SendString("\r\n");

    while (elapsed < timeout_ms) {
        delay_ms(50);
        elapsed += 50;

        if (RingBuf_Contains(ack)) {
            return 1;
        }
        if (RingBuf_Contains("ERROR") || RingBuf_Contains("FAIL")) {
            return 0;
        }
    }

    return 0;  /* Timeout */
}

/**
 * @brief  Send AT command, wait for ack, ignore "ALREADY CONNECTED" noise.
 * @retval 1=success, 0=fail
 */
static uint8_t AT_WaitOK(const char* cmd, uint16_t timeout)
{
    if (ESP01S_SendCmd(cmd, "OK", timeout)) return 1;
    if (ESP01S_SendCmd(cmd, "no change", 500)) return 1;
    return 0;
}

/* ======================== Initialization Sequence ======================== */

/**
 * @brief  AT configuration sequence.
 *         RST/EN pins are NOT connected, so hardware reset is replaced
 *         with a power-on delay to let ESP-01S self-boot complete.
 */
static void ESP01S_ConfigSequence(void)
{
    uint8_t ok;

    printf("[CFG] Step1: Power-on delay (RST/EN not connected)...\r\n");
    delay_ms(1000);  /* Wait for ESP-01S power-on self-boot */
    printf("[CFG] Step1: Done.\r\n");

    /* Step 2: Check module alive - auto baud rate detection */
    printf("[CFG] Step2: AT check (auto baud)...\r\n");
    ok = AT_WaitOK("AT", 2000);
    if (!ok) {
        /* Try 9600 */
        USART_InitTypeDef USART_InitStructure;
        printf("[CFG] Step2: 115200 failed, trying 9600...\r\n");
        USART_Cmd(ESP01S_USART, DISABLE);
        USART_InitStructure.USART_BaudRate = 9600;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(ESP01S_USART, &USART_InitStructure);
        USART_Cmd(ESP01S_USART, ENABLE);
        delay_ms(200);
        RingBuf_Flush();
        ok = AT_WaitOK("AT", 2000);
        if (ok) {
            printf("[CFG] Step2: Works at 9600, setting to 115200...\r\n");
            AT_WaitOK("AT+UART_DEF=115200,8,1,0,0", 2000);
            USART_Cmd(ESP01S_USART, DISABLE);
            USART_InitStructure.USART_BaudRate = 115200;
            USART_Init(ESP01S_USART, &USART_InitStructure);
            USART_Cmd(ESP01S_USART, ENABLE);
            delay_ms(200);
            RingBuf_Flush();
            ok = AT_WaitOK("AT", 2000);
        }
    }
    if (!ok) {
        /* Try 74880 */
        USART_InitTypeDef USART_InitStructure;
        printf("[CFG] Step2: trying 74880...\r\n");
        USART_Cmd(ESP01S_USART, DISABLE);
        USART_InitStructure.USART_BaudRate = 74880;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(ESP01S_USART, &USART_InitStructure);
        USART_Cmd(ESP01S_USART, ENABLE);
        delay_ms(200);
        RingBuf_Flush();
        ok = AT_WaitOK("AT", 2000);
        if (ok) {
            printf("[CFG] Step2: Works at 74880, setting to 115200...\r\n");
            AT_WaitOK("AT+UART_DEF=115200,8,1,0,0", 2000);
            USART_Cmd(ESP01S_USART, DISABLE);
            USART_InitStructure.USART_BaudRate = 115200;
            USART_Init(ESP01S_USART, &USART_InitStructure);
            USART_Cmd(ESP01S_USART, ENABLE);
            delay_ms(200);
            RingBuf_Flush();
            ok = AT_WaitOK("AT", 2000);
        }
    }
    printf("[CFG] Step2: %s\r\n", ok ? "OK" : "FAIL");

    printf("[CFG] Step3: Set AP mode...\r\n");
    ok = AT_WaitOK("AT+CWMODE=2", 2000);
    printf("[CFG] Step3: %s\r\n", ok ? "OK" : "FAIL");

    printf("[CFG] Step4: Create AP...\r\n");
    ok = AT_WaitOK("AT+CWSAP=\"ESP8266\",\"12345678\",1,3", 3000);
    delay_ms(1000);
    printf("[CFG] Step4: %s\r\n", ok ? "OK" : "FAIL");

    printf("[CFG] Step5: Enable MUX...\r\n");
    ok = AT_WaitOK("AT+CIPMUX=1", 2000);
    printf("[CFG] Step5: %s\r\n", ok ? "OK" : "FAIL");

    printf("[CFG] Step6: Start TCP server...\r\n");
    ok = AT_WaitOK("AT+CIPSERVER=1,8080", 2000);
    if (!ok) {
        printf("[CFG] Step6: Retry...\r\n");
        delay_ms(1000);
        ok = AT_WaitOK("AT+CIPSERVER=1,8080", 2000);
    }
    printf("[CFG] Step6: %s\r\n", ok ? "OK" : "FAIL");

    printf("[CFG] Step7: Set timeout...\r\n");
    ok = AT_WaitOK("AT+CIPSTO=120", 2000);
    printf("[CFG] Step7: %s\r\n", ok ? "OK" : "FAIL");

    printf("[CFG] Step8: Check IP...\r\n");
    ESP01S_SendCmd("AT+CIFSR", "192.168.4.1", 2000);
    printf("[CFG] Init complete.\r\n");
}

/* ======================== Frame TX ======================== */

/**
 * @brief  Build a protocol frame and send it to a connected client.
 * @param  link_id: TCP connection link ID
 * @param  cmd: Command byte
 * @param  data: Payload data
 * @param  len: Payload length
 */
static void ESP01S_SendFrame(uint8_t link_id, uint8_t cmd, const uint8_t* data, uint8_t len)
{
    uint8_t frame[ESP01S_TX_BUF_SIZE];
    uint8_t i;
    uint8_t checksum = 0;
    uint16_t frameLen;
    char cipSendCmd[32];

    if (!s_clientConnected) {
        printf("[SEND] No client connected, skip.\r\n");
        return;
    }

    /* Build frame: HEADER | CMD | LEN | PAYLOAD | CHECKSUM | END */
    frameLen = (uint16_t)len + FRAME_OVERHEAD;
    if (frameLen > ESP01S_TX_BUF_SIZE) return;

    frame[0] = FRAME_HEADER;
    frame[1] = cmd;
    frame[2] = len;
    checksum = cmd + len;
    for (i = 0; i < len; i++) {
        frame[3 + i] = data[i];
        checksum += data[i];
    }
    frame[3 + len] = checksum;
    frame[4 + len] = FRAME_END;

    /* AT+CIPSEND=<link_id>,<length> */
    sprintf(cipSendCmd, "AT+CIPSEND=%d,%d", link_id, (int)frameLen);
    printf("[SEND] CMD: %s\r\n", cipSendCmd);
    if (!ESP01S_SendCmd(cipSendCmd, ">", 1000)) {
        printf("[SEND] FAILED - no > prompt\r\n");
        return;
    }
    printf("[SEND] Got > prompt, sending frame...\r\n");

    USART_SendBytes(frame, frameLen);

    {
        uint16_t elapsed = 0;
        while (elapsed < 200) {
            delay_ms(10);
            elapsed += 10;
            if (RingBuf_Contains("SEND OK")) {
                printf("[SEND] SEND OK confirmed\r\n");
                RingBuf_SkipUntil("SEND OK");
                return;
            }
        }
        printf("[SEND] Timeout waiting for SEND OK\r\n");
        RingBuf_SkipUntil("SEND OK");
    }
}

/* ======================== Public TX Functions ======================== */

extern volatile uint32_t TimeCnt_ms;  /* Defined in stm32f10x_it.c */

void ESP01S_SendSensorData(void)
{
    static uint32_t lastSendTick = 0;
    uint32_t now;
    uint8_t payload[14];

    if (!s_clientConnected) return;

    /* Rate limit: skip if less than 2s since last send */
    now = TimeCnt_ms;
    if ((now - lastSendTick) < 2000) return;
    lastSendTick = now;

    /* Pack WaterTemp (float, 4 bytes) */
    memcpy(&payload[0], &Record.waterTemp, 4);

    /* Pack PH_Value (float, 4 bytes) */
    memcpy(&payload[4], &Record.phValue, 4);

    /* Pack WaterLevel (uint8, 1 byte) */
    payload[8] = Record.waterLevel;

    /* Pack AirQuality (uint16, big-endian, 2 bytes) */
    payload[9]  = (uint8_t)(Record.airQuality >> 8);
    payload[10] = (uint8_t)(Record.airQuality & 0xFF);

    /* Pack RunMode (uint8, 1 byte) */
    payload[11] = Record.runMode;

    /* Pack FeedCountdown (int16 -> uint16 big-endian, clamp negative to 0) */
    {
        uint16_t fc = (Record.feedCountdown > 0) ? (uint16_t)Record.feedCountdown : 0;
        payload[12] = (uint8_t)(fc >> 8);
        payload[13] = (uint8_t)(fc & 0xFF);
    }

    ESP01S_SendFrame(s_clientLinkId, CMD_SENSOR_DATA, payload, 14);
    printf("[TX] Sensor data: T=%.1f PH=%.1f WL=%d%%\r\n",
           Record.waterTemp, Record.phValue, Record.waterLevel);

    /* Always send device status after sensor data (rate-limited 500ms) */
    ESP01S_SendDeviceStatus();
}

/**
 * @brief  Send device status frame (CMD 0x02, 5 bytes).
 *         Rate-limited to min 500ms between sends.
 */
void ESP01S_SendDeviceStatus(void)
{
    static uint32_t lastSendTick = 0;
    uint32_t now;
    uint8_t payload[5];

    if (!s_clientConnected) return;

    now = TimeCnt_ms;
    if ((now - lastSendTick) < 500) return;
    lastSendTick = now;
    s_statusDirty = 0; /* Clear dirty flag on successful send */

    /* relayState bitfield */
    payload[0] = 0;
    if (Flag.relayHeat)   payload[0] |= 0x01;
    if (Flag.relayFill)   payload[0] |= 0x02;
    if (Flag.relayDrain)  payload[0] |= 0x04;
    if (Flag.relayOxygen) payload[0] |= 0x08;

    payload[1] = Flag.alarmEnable;
    payload[2] = Flag.feeding;
    payload[3] = 0; /* reserved */
    payload[4] = 0; /* reserved */

    ESP01S_SendFrame(s_clientLinkId, CMD_DEVICE_STATUS, payload, 5);
    printf("[TX] Device status: relay=0x%02X alrm=%d feed=%d\r\n",
           payload[0], payload[1], payload[2]);
}

uint8_t ESP01S_IsClientConnected(void)
{
    return s_clientConnected;
}

void ESP01S_DumpRingBuf(void)
{
    uint16_t count, i;

    if (s_ringHead >= s_ringTail)
        count = s_ringHead - s_ringTail;
    else
        count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

    if (count == 0) {
        printf("[DUMP] RingBuf empty\r\n");
        return;
    }

    printf("[DUMP] RingBuf %d bytes: ", count);
    for (i = 0; i < count && i < 64; i++) {
        uint8_t b = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
        printf("%02X ", b);
    }
    if (count > 64) printf("...");
    printf("\r\n");

    printf("[DUMP] ASCII: ");
    for (i = 0; i < count && i < 64; i++) {
        uint8_t b = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
        printf("%c", (b >= 0x20 && b < 0x7F) ? b : '.');
    }
    if (count > 64) printf("...");
    printf("\r\n");
}

void ESP01S_FlushRingBuf(void)
{
    s_ringTail = s_ringHead;
}

void ESP01S_QueryStatus(void)
{
    uint16_t elapsed = 0;
    uint16_t count, i;
    uint16_t scanCount;
    uint8_t scanBuf[256];

    RingBuf_Flush();
    USART_SendString("AT+CIPSTATUS\r\n");

    while (elapsed < 2000) {
        delay_ms(50);
        elapsed += 50;
        if (RingBuf_Contains("OK")) break;
        if (RingBuf_Contains("ERROR")) break;
    }

    if (s_ringHead >= s_ringTail)
        count = s_ringHead - s_ringTail;
    else
        count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

    scanCount = count < 256 ? count : 256;
    for (i = 0; i < scanCount; i++) {
        scanBuf[i] = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
    }

    printf("[CIPSTATUS] Response (%d bytes): ", count);
    for (i = 0; i < scanCount && i < 128; i++) {
        printf("%c", (scanBuf[i] >= 0x20 && scanBuf[i] < 0x7F) ? scanBuf[i] : '.');
    }
    printf("\r\n");

    for (i = 0; i + 9 <= scanCount; i++) {
        if (memcmp(&scanBuf[i], "STATUS:3", 8) == 0) {
            printf("[CIPSTATUS] STATUS:3 = has connections\r\n");
            {
                uint16_t j;
                for (j = 0; j + 12 <= scanCount; j++) {
                    if (memcmp(&scanBuf[j], "+CIPSTATUS:", 11) == 0) {
                        uint8_t lid = scanBuf[j + 11] - '0';
                        if (lid <= 4) {
                            if (!s_clientConnected) {
                                s_clientLinkId = lid;
                                s_clientConnected = 1;
                                printf("[CIPSTATUS] Found client on link_id=%d\r\n", lid);
                            }
                        }
                        break;
                    }
                }
            }
            RingBuf_Flush();
            return;
        }
    }

    for (i = 0; i + 9 <= scanCount; i++) {
        if (memcmp(&scanBuf[i], "STATUS:", 7) == 0) {
            printf("[CIPSTATUS] STATUS=%c\r\n", scanBuf[i + 7]);
            break;
        }
    }

    RingBuf_Flush();
}

void ESP01S_EnsureServer(void)
{
    uint16_t elapsed = 0;
    uint16_t count, i;
    uint16_t scanCount;
    uint8_t scanBuf[128];
    uint8_t hasServer = 0;

    RingBuf_Flush();
    USART_SendString("AT+CIPSTATUS\r\n");

    while (elapsed < 2000) {
        delay_ms(50);
        elapsed += 50;
        if (RingBuf_Contains("OK")) break;
    }

    if (s_ringHead >= s_ringTail)
        count = s_ringHead - s_ringTail;
    else
        count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

    scanCount = count < 128 ? count : 128;
    for (i = 0; i < scanCount; i++) {
        scanBuf[i] = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
    }

    for (i = 0; i + 11 <= scanCount; i++) {
        if (memcmp(&scanBuf[i], "+CIPSTATUS:", 11) == 0) {
            hasServer = 1;
            break;
        }
    }

    for (i = 0; i + 8 <= scanCount; i++) {
        if (memcmp(&scanBuf[i], "STATUS:2", 8) == 0 ||
            memcmp(&scanBuf[i], "STATUS:3", 8) == 0) {
            hasServer = 1;
            break;
        }
    }

    RingBuf_Flush();

    if (!hasServer) {
        printf("[SERVER] TCP server not detected, restarting...\r\n");
        AT_WaitOK("AT+CIPMUX=1", 2000);
        AT_WaitOK("AT+CIPSERVER=1,8080", 2000);
        AT_WaitOK("AT+CIPSTO=120", 2000);
        printf("[SERVER] TCP server restarted.\r\n");
    } else {
        printf("[SERVER] TCP server OK.\r\n");
    }
}

/* ======================== Frame RX Parsing ======================== */

/**
 * @brief  Handle a received protocol frame.
 * @param  frame: Pointer to frame bytes (includes HEADER..END)
 *
 * Note: Board has only 1 LED (LED_Name_1 on PB9).
 *       CMD_LED_CONTROL: led_id is ignored, state=1 means ON, 0 means OFF.
 */
static void ESP01S_HandleFrame(const uint8_t* frame)
{
    uint8_t cmd  = frame[1];
    uint8_t len  = frame[2];
    const uint8_t* payload = &frame[3];

    switch (cmd) {
    case CMD_ECHO_REQUEST:
        if (len == 0) {
            /* APP queries status: respond with DeviceStatus */
            s_statusDirty = 1;
        } else {
            ESP01S_SendFrame(s_clientLinkId, CMD_ECHO_REQUEST, payload, len);
        }
        break;
    case CMD_LED_CONTROL:
        /* LEN>=1: state(0=OFF,1=ON) */
        if (len >= 1) {
            uint8_t state = payload[0];
            if (state) {
                LED_On(LED_Name_1);
            } else {
                LED_Off(LED_Name_1);
            }
            printf("[LED] LED1 %s\r\n", state ? "ON" : "OFF");
        }
        break;
    case CMD_SWITCH_MODE:
        /* LEN>=1: mode(0=auto, 1=manual) */
        if (len >= 1) {
            Record.runMode = payload[0] ? 1 : 0;
            printf("[RX] Switch mode: %s\r\n", Record.runMode ? "MANUAL" : "AUTO");
            s_statusDirty = 1;
        }
        break;
    case CMD_CONTROL_RELAY:
        /* LEN>=2: relay_id(0-3), state(0=OFF,1=ON) */
        if (len >= 2) {
            uint8_t relayId = payload[0];
            uint8_t state = payload[1] ? 1 : 0;
            switch (relayId) {
                case 0: Flag.relayHeat   = state; printf("[RX] Relay Heat: %s\r\n", state ? "ON" : "OFF"); break;
                case 1: Flag.relayFill   = state; printf("[RX] Relay Fill: %s\r\n", state ? "ON" : "OFF"); break;
                case 2: Flag.relayDrain  = state; printf("[RX] Relay Drain: %s\r\n", state ? "ON" : "OFF"); break;
                case 3: Flag.relayOxygen = state; printf("[RX] Relay Oxygen: %s\r\n", state ? "ON" : "OFF"); break;
                default: printf("[RX] Unknown relay ID: %d\r\n", relayId); break;
            }
            s_statusDirty = 1;
        }
        break;
    case CMD_TRIGGER_FEED:
        /* LEN>=0: no payload */
        Flag.feeding = 1;
        Record.feedCountdown = 3; /* Keep servo open for 3 seconds */
        printf("[RX] Trigger feed\r\n");
        s_statusDirty = 1;
        break;
    case CMD_SET_ALARM:
        /* LEN>=1: enabled(0=off,1=on) */
        if (len >= 1) {
            Flag.alarmEnable = payload[0] ? 1 : 0;
            printf("[RX] Alarm enable: %s\r\n", Flag.alarmEnable ? "ON" : "OFF");
            s_statusDirty = 1;
        }
        break;
    case CMD_SYNC_THRESHOLDS:
        /* LEN=20: tempLower(4F) tempUpper(4F) airQMax(2BE) phLower(4F) phUpper(4F) wlMin(1U) wlMax(1U) */
        if (len >= 20) {
            memcpy(&Record.tempLower, &payload[0], 4);
            memcpy(&Record.tempUpper, &payload[4], 4);
            Record.airQualityMax = ((uint16_t)payload[8] << 8) | payload[9];
            memcpy(&Record.phLower, &payload[10], 4);
            memcpy(&Record.phUpper, &payload[14], 4);
            Record.waterLevelMin = payload[18];
            Record.waterLevelMax = payload[19];
            printf("[RX] Thresholds synced\r\n");
            s_statusDirty = 1;
        }
        break;
    case CMD_SET_FEED_INTERVAL:
        /* LEN=2: seconds (uint16 BE) */
        if (len >= 2) {
            Record.feedInterval = ((uint16_t)payload[0] << 8) | payload[1];
            printf("[RX] Feed interval: %d sec\r\n", Record.feedInterval);
            s_statusDirty = 1;
        }
        break;
    default:
        printf("[RX] Unknown CMD=0x%02X LEN=%d\r\n", cmd, len);
        break;
    }
}

/**
 * @brief  Extract and process protocol frames from a data buffer.
 */
static void ESP01S_ParseFrames(const uint8_t* data, uint16_t dataLen)
{
    uint16_t i = 0;

    while (i < dataLen) {
        if (data[i] != FRAME_HEADER) {
            i++;
            continue;
        }

        if (i + 3 > dataLen) break;

        {
            uint8_t payloadLen = data[i + 2];
            uint16_t frameSize = (uint16_t)payloadLen + FRAME_OVERHEAD;

            if (i + frameSize > dataLen) {
                i++;
                continue;
            }

            if (data[i + frameSize - 1] != FRAME_END) {
                i++;
                continue;
            }

            {
                uint8_t calcSum = 0;
                uint8_t j;
                for (j = 1; j < 3 + payloadLen; j++) {
                    calcSum += data[i + j];
                }
                if (calcSum != data[i + frameSize - 2]) {
                    i++;
                    continue;
                }
            }

            ESP01S_HandleFrame(&data[i]);
            i += frameSize;
        }
    }
}

/* ======================== +IPD Parser ======================== */

/** Simple decimal parser: read digits from ring buffer at given index */
static int16_t RingBuf_ParseNumber(uint16_t startIdx, uint16_t* outEndIdx, uint16_t maxLen)
{
    int16_t value = 0;
    uint16_t idx = startIdx;
    uint16_t count = 0;
    uint8_t ch;

    while (count < maxLen) {
        ch = s_ringBuf[idx % ESP01S_RINGBUF_SIZE];
        if (ch >= '0' && ch <= '9') {
            value = value * 10 + (ch - '0');
            idx = (idx + 1) % ESP01S_RINGBUF_SIZE;
            count++;
        } else {
            break;
        }
    }

    if (count == 0) return -1;
    *outEndIdx = idx;
    return value;
}

/**
 * @brief  Process ring buffer: look for +IPD, CONNECT, DISCONNECT patterns.
 */
void ESP01S_Process(void)
{
    uint16_t count, i;

    if (s_ringHead >= s_ringTail)
        count = s_ringHead - s_ringTail;
    else
        count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

    if (count == 0) return;

    /* ---- Check for WiFi CONNECT/DISCONNECT notifications ---- */
    /* ESP-01S AP mode sends: +STA_CONNECTED / +STA_DISCONNECTED */
    /* ESP-01S STA mode sends: WIFI CONNECTED / WIFI DISCONNECTED */
    {
        uint16_t scanCount = count < 64 ? count : 64;
        uint8_t scanBuf[64];
        for (i = 0; i < scanCount; i++) {
            scanBuf[i] = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
        }

        for (i = 0; i + 14 <= scanCount; i++) {
            if (memcmp(&scanBuf[i], "WIFI CONNECTED", 14) == 0 ||
                memcmp(&scanBuf[i], "+STA_CONNECTED", 14) == 0) {
                ESP01S_WiFiConnected = 1;
                RingBuf_SkipUntil((memcmp(&scanBuf[i], "WIFI", 4) == 0) ? "WIFI CONNECTED" : "+STA_CONNECTED");
                printf("[NET] WiFi connected\r\n");
                if (s_ringHead >= s_ringTail)
                    count = s_ringHead - s_ringTail;
                else
                    count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;
                break;
            }
        }

        for (i = 0; i + 17 <= scanCount; i++) {
            if (memcmp(&scanBuf[i], "WIFI DISCONNECTED", 17) == 0) {
                ESP01S_WiFiConnected = 0;
                s_clientConnected = 0;
                RingBuf_SkipUntil("WIFI DISCONNECTED");
                printf("[NET] WiFi disconnected\r\n");
                if (s_ringHead >= s_ringTail)
                    count = s_ringHead - s_ringTail;
                else
                    count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;
                break;
            }
            if (i + 18 <= scanCount && memcmp(&scanBuf[i], "+STA_DISCONNECTED", 17) == 0) {
                ESP01S_WiFiConnected = 0;
                s_clientConnected = 0;
                RingBuf_SkipUntil("+STA_DISCONNECTED");
                printf("[NET] WiFi disconnected (AP mode)\r\n");
                if (s_ringHead >= s_ringTail)
                    count = s_ringHead - s_ringTail;
                else
                    count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;
                break;
            }
        }

        /* Also check for +DIST_STA_IP (AP mode: station got IP) */
        for (i = 0; i + 12 <= scanCount; i++) {
            if (memcmp(&scanBuf[i], "+DIST_STA_IP", 12) == 0) {
                ESP01S_WiFiConnected = 1;
                RingBuf_SkipUntil("+DIST_STA_IP");
                printf("[NET] WiFi station got IP (AP mode)\r\n");
                if (s_ringHead >= s_ringTail)
                    count = s_ringHead - s_ringTail;
                else
                    count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;
                break;
            }
        }
    }

    /* ---- Check for CONNECT / DISCONNECT notifications ---- */
    {
        uint16_t scanCount = count < 32 ? count : 32;
        uint8_t scanBuf[32];
        for (i = 0; i < scanCount; i++) {
            scanBuf[i] = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
        }

        for (i = 0; i + 8 <= scanCount; i++) {
            if (memcmp(&scanBuf[i], ",CONNECT", 8) == 0) {
                if (i > 0 && scanBuf[i - 1] >= '0' && scanBuf[i - 1] <= '4') {
                    s_clientLinkId = scanBuf[i - 1] - '0';
                    s_clientConnected = 1;
                    s_connectCount++;
                    printf("[NET] Client CONNECTED link_id=%d\r\n", s_clientLinkId);
                }
                RingBuf_SkipUntil(",CONNECT");
                if (s_ringHead >= s_ringTail)
                    count = s_ringHead - s_ringTail;
                else
                    count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;
                break;
            }
        }

        scanCount = count < 32 ? count : 32;
        for (i = 0; i < scanCount; i++) {
            scanBuf[i] = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
        }
        for (i = 0; i + 7 <= scanCount; i++) {
            if (memcmp(&scanBuf[i], ",CLOSED", 7) == 0) {
                s_clientConnected = 0;
                s_disconnectCount++;
                printf("[NET] Client DISCONNECTED\r\n");
                RingBuf_SkipUntil(",CLOSED");
                if (s_ringHead >= s_ringTail)
                    count = s_ringHead - s_ringTail;
                else
                    count = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;
                break;
            }
        }
    }

    /* ---- Check for +IPD data ---- */
    {
        uint16_t scanCount = count < 128 ? count : 128;
        uint8_t scanBuf[128];
        uint16_t ipdPos = 0xFFFF;
        int foundIPD = 0;

        for (i = 0; i < scanCount; i++) {
            scanBuf[i] = s_ringBuf[(s_ringTail + i) % ESP01S_RINGBUF_SIZE];
        }

        for (i = 0; i + 5 <= scanCount; i++) {
            if (memcmp(&scanBuf[i], "+IPD", 4) == 0) {
                ipdPos = i;
                foundIPD = 1;
                break;
            }
        }

        if (!foundIPD && scanCount > 0) {
            static uint32_t lastNoIpdDbg = 0;
            if ((TimeCnt_ms - lastNoIpdDbg) >= 3000 && scanCount > 4) {
                lastNoIpdDbg = TimeCnt_ms;
                printf("[IPD] No +IPD found in %d bytes: ", scanCount);
                { uint16_t _di; for (_di = 0; _di < scanCount && _di < 20; _di++) printf("%02X ", scanBuf[_di]); }
                printf("\r\n");
            }
        }

        if (foundIPD) {
            uint16_t pos = ipdPos + 4;
            int16_t linkId, dataLen;
            uint16_t endPos;
            uint16_t dataAvail;
            uint16_t dataStartRingIdx;

            if (pos >= scanCount || scanBuf[pos] != ',') {
                RingBuf_SkipUntil("+IPD");
                return;
            }
            pos++;

            linkId = RingBuf_ParseNumber((s_ringTail + pos) % ESP01S_RINGBUF_SIZE, &endPos, 2);
            if (linkId < 0) {
                RingBuf_SkipUntil("+IPD");
                return;
            }

            if (endPos >= s_ringTail)
                pos = endPos - s_ringTail;
            else
                pos = ESP01S_RINGBUF_SIZE - s_ringTail + endPos;

            if (pos < scanCount && scanBuf[pos] == ',') {
                pos++;
            }

            dataLen = RingBuf_ParseNumber((s_ringTail + pos) % ESP01S_RINGBUF_SIZE, &endPos, 5);
            if (dataLen <= 0) {
                RingBuf_SkipUntil("+IPD");
                return;
            }

            if (endPos >= s_ringTail)
                pos = endPos - s_ringTail;
            else
                pos = ESP01S_RINGBUF_SIZE - s_ringTail + endPos;

            if (pos < scanCount && scanBuf[pos] == ':') {
                pos++;
            }

            {
                uint16_t totalAvail;
                if (s_ringHead >= s_ringTail)
                    totalAvail = s_ringHead - s_ringTail;
                else
                    totalAvail = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

                dataAvail = totalAvail > pos ? totalAvail - pos : 0;

                if (dataAvail >= (uint16_t)dataLen) {
                    uint8_t extractBuf[128];
                    uint16_t extractLen = (uint16_t)dataLen;
                    if (extractLen > sizeof(extractBuf)) extractLen = sizeof(extractBuf);

                    dataStartRingIdx = (s_ringTail + pos) % ESP01S_RINGBUF_SIZE;
                    for (i = 0; i < extractLen; i++) {
                        extractBuf[i] = s_ringBuf[(dataStartRingIdx + i) % ESP01S_RINGBUF_SIZE];
                    }

                    s_ringTail = (dataStartRingIdx + extractLen) % ESP01S_RINGBUF_SIZE;

                    printf("[RX] +IPD len=%d, data: ", extractLen);
                    { uint16_t _di; for (_di = 0; _di < extractLen && _di < 16; _di++) printf("%02X ", extractBuf[_di]); }
                    printf("\r\n");

                    ESP01S_ParseFrames(extractBuf, extractLen);
                    printf("[RX] Parse done.\r\n");
                }
            }
        }
    }

    /* ---- Garbage collection ---- */
    {
        uint16_t used;
        if (s_ringHead >= s_ringTail)
            used = s_ringHead - s_ringTail;
        else
            used = ESP01S_RINGBUF_SIZE - s_ringTail + s_ringHead;

        if (used > (ESP01S_RINGBUF_SIZE * 80 / 100)) {
            uint16_t discard = used / 4;
            s_ringTail = (s_ringTail + discard) % ESP01S_RINGBUF_SIZE;
        }
    }
}

/* ======================== Public Init ======================== */

void ESP01S_Init(void)
{
    ESP01S_USART_Init();

    delay_ms(500);
    RingBuf_Flush();

    ESP01S_ConfigSequence();
}

/* ======================== End of File ======================== */
