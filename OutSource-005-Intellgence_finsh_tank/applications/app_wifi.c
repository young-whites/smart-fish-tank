#include "app_data.h"
#include "app_wifi.h"
#include "stm32f1xx.h"
#include <stdint.h>
#include <string.h>

volatile uint8_t g_wifi_connected = 0;

/* 协议帧: 0xAA | CMD | LEN | DATA | SUM | 0x55 */
static uint8_t frame_buf[256];

/* ========== USART2 寄存器操作 ========== */

static void uart_send_byte(uint8_t b)
{
    USART2->DR = b;
    while(!(USART2->SR & USART_SR_TXE));
}

static void uart_send_string(const char *s)
{
    while(*s) {
        uart_send_byte((uint8_t)*s);
        s++;
    }
}

static void uart_send_buf(const uint8_t *buf, uint16_t len)
{
    while(len--) {
        uart_send_byte(*buf++);
    }
}

static void uart_init(void)
{
    /* 使能 USART2, GPIOA 时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PA2 (TX): 复用推挽输出 50MHz -> CRL[11:8] */
    GPIOA->CRL &= ~(0xFU << 8);
    GPIOA->CRL |=  (0xBU << 8);  /* MODE=11, CNF=10 */

    /* PA3 (RX): 上拉输入 -> CRL[15:12] */
    GPIOA->CRL &= ~(0xFU << 12);
    GPIOA->CRL |=  (0x8U << 12); /* MODE=00, CNF=10 */
    GPIOA->BSRR = (1U << 3);     /* 上拉 */

    /* USART2: 115200, 8N1 */
    USART2->BRR = 0x271;  /* 72MHz / 16 / 115200 ≈ 39.0625 -> 0x271 */
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    rt_thread_mdelay(100);
}

/* ========== 协议帧构建 (无 sprintf) ========== */

static void build_frame(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    uint8_t sum = cmd + len;
    uint8_t i;
    frame_buf[0] = 0xAA;
    frame_buf[1] = cmd;
    frame_buf[2] = len;
    for(i = 0; i < len; i++) {
        frame_buf[3 + i] = data[i];
        sum += data[i];
    }
    frame_buf[3 + len] = sum;
    frame_buf[4 + len] = 0x55;
}

static void send_sensor_data(void)
{
    uint8_t payload[14];
    int32_t temp_int, ph_int;

    /* 将 float 拆为整数传输 (温度*10, PH*10) */
    temp_int = (int32_t)(g_sensor.water_temp * 10.0f);
    ph_int = (int32_t)(g_sensor.ph_value * 10.0f);

    payload[0] = (temp_int >> 8) & 0xFF;
    payload[1] = temp_int & 0xFF;
    payload[2] = (ph_int >> 8) & 0xFF;
    payload[3] = ph_int & 0xFF;
    payload[4] = g_sensor.water_level;
    payload[5] = (g_sensor.air_quality >> 8) & 0xFF;
    payload[6] = g_sensor.air_quality & 0xFF;
    payload[7] = g_sensor.run_mode;
    payload[8] = (g_sensor.feed_countdown >> 8) & 0xFF;
    payload[9] = g_sensor.feed_countdown & 0xFF;

    build_frame(0x01, payload, 10);
    uart_send_buf(frame_buf, 15);  /* 5 header + 10 data */
}

static void send_device_status(void)
{
    uint8_t payload[5] = {0};
    if(g_status.relay_heat)   payload[0] |= 0x01;
    if(g_status.relay_fill)   payload[0] |= 0x02;
    if(g_status.relay_drain)  payload[0] |= 0x04;
    if(g_status.relay_oxygen) payload[0] |= 0x08;
    payload[1] = g_status.alarm_enable;
    payload[2] = g_status.feeding;
    build_frame(0x02, payload, 5);
    uart_send_buf(frame_buf, 10);  /* 5 header + 5 data */
}

/* ========== WiFi 连接 (无 sprintf, AT 指令硬编码) ========== */

static uint8_t wifi_try_connect(void)
{
    uart_send_string("AT\r\n");
    rt_thread_mdelay(500);
    uart_send_string("AT+CWMODE=1\r\n");
    rt_thread_mdelay(500);
    /* 注意: SSID 和密码需要根据实际修改 */
    uart_send_string("AT+CWJAP=\"YOUR_SSID\",\"YOUR_PASSWORD\"\r\n");
    rt_thread_mdelay(8000);
    uart_send_string("AT+CIPMUX=1\r\n");
    rt_thread_mdelay(500);
    uart_send_string("AT+CIPSERVER=1,8080\r\n");
    rt_thread_mdelay(500);
    return 1;
}

static struct rt_thread wifi_thread;
static rt_uint8_t wifi_stack[1024];

static void wifi_thread_entry(void *param)
{
    uart_init();

    rt_thread_mdelay(1000);

    if(wifi_try_connect()) {
        g_wifi_connected = 1;
    }

    while(1) {
        if(g_wifi_connected) {
            send_sensor_data();
            rt_thread_mdelay(500);
            send_device_status();
        } else {
            rt_thread_mdelay(5000);
            if(wifi_try_connect()) g_wifi_connected = 1;
        }
        rt_thread_mdelay(500);
    }
}

void app_wifi_init(void)
{
    rt_thread_init(&wifi_thread,
        "t_wifi", wifi_thread_entry, RT_NULL,
        &wifi_stack[0], sizeof(wifi_stack),
        17, 10);
    rt_thread_startup(&wifi_thread);
}
