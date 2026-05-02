/**
 * @file    bsp_esp01s.c
 * @brief   ESP-01S WiFi 模块 AT 命令驱动 + 通信协议实现
 * @version 1.0
 */
#include "bsp_esp01s.h"
#include <string.h>
#include "usart_use1.h"

/* ===================== 外部变量 ===================== */
extern FishTank_DataStruct  FishTank_Data;
extern FishTank_FlagStruct  FishTank_Flag;
extern FishTank_SetStruct   FishTank_Set;
extern FishTank_FeedStruct  FishTank_Feed;
extern uint8_t Feed_Trigger;

/* ===================== WiFi 状态 ===================== */
volatile uint8_t ESP01S_WiFiConnected = 0;  /* 0=离线 1=已连接 */

/* ===================== 环形缓冲区 ===================== */
static volatile uint8_t  _rx_buf[ESP01S_RX_BUF_SIZE];
static volatile uint16_t _rx_head = 0;  /* 写入位置 (ISR) */
static volatile uint16_t _rx_tail = 0;  /* 读取位置 (main) */

/* AT 应答缓冲区 */
static uint8_t _at_buf[256];
static volatile uint16_t _at_len = 0;

/* ===================== 环形缓冲区操作 ===================== */

/**
 * @brief  从环形缓冲区读取一个字节
 * @retval 1=有效  0=空
 */
static uint8_t _ring_pop(uint8_t *out)
{
    if (_rx_head == _rx_tail) return 0;
    *out = _rx_buf[_rx_tail];
    _rx_tail = (_rx_tail + 1) % ESP01S_RX_BUF_SIZE;
    return 1;
}

/**
 * @brief  检查环形缓冲区中可读字节数
 */
static uint16_t _ring_available(void)
{
    if (_rx_head >= _rx_tail)
        return _rx_head - _rx_tail;
    else
        return ESP01S_RX_BUF_SIZE - _rx_tail + _rx_head;
}

/* ===================== USART 中断调用 ===================== */

/**
 * @brief  USART 接收中断中调用，将字节存入环形缓冲区
 * @note   由 usart_use1.c 的中断处理函数调用
 */
void ESP01S_RxByte(uint8_t byte)
{
    uint16_t next = (_rx_head + 1) % ESP01S_RX_BUF_SIZE;
    if (next != _rx_tail)  /* 缓冲区未满 */
    {
        _rx_buf[_rx_head] = byte;
        _rx_head = next;
    }
    /* 缓冲区满时丢弃新数据 */
}

/* ===================== USART 发送辅助 ===================== */

/**
 * @brief  通过 USART2 发送字符串
 */
static void _send_str(const char *str)
{
    Usart_SendString(DEBUG_USART_1, (char *)str);
}

/**
 * @brief  通过 USART2 发送原始字节
 */
static void _send_bytes(const uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        Usart_SendByte(DEBUG_USART_1, data[i]);
    }
}

/* ===================== AT 指令 ===================== */

/**
 * @brief  简易 ms 延时 (使用全局 delay_ms)
 */
extern void delay_ms(uint16_t ms);

/**
 * @brief  发送 AT 指令并等待应答
 */
uint8_t ESP01S_SendATCmd(const char *cmd, const char *ack, uint16_t timeout_ms)
{
    uint16_t elapsed = 0;
    uint8_t  ch;
    char     cmp_buf[64];
    uint16_t cmp_len = 0;

    /* 清空之前残留数据 */
    while (_ring_pop(&ch)) {}

    /* 发送指令 */
    _send_str(cmd);
    _send_str("\r\n");

    /* 等待应答 */
    while (elapsed < timeout_ms)
    {
        while (_ring_pop(&ch))
        {
            /* 简单匹配: 将收到的字符累积，在其中搜索 ack */
            if (cmp_len < sizeof(cmp_buf) - 1)
            {
                cmp_buf[cmp_len++] = (char)ch;
                cmp_buf[cmp_len] = '\0';
            }

            /* 检查是否包含期望应答 */
            if (strstr(cmp_buf, ack) != NULL)
            {
                return 1;
            }

            /* 检查是否收到 ERROR */
            if (strstr(cmp_buf, "ERROR") != NULL)
            {
                return 0;
            }
        }
        delay_ms(10);
        elapsed += 10;
    }

    return 0;  /* 超时 */
}

/**
 * @brief  通过 TCP 连接发送数据帧
 */
uint8_t ESP01S_SendData(uint8_t link_id, const uint8_t *data, uint16_t len)
{
    char cmd[32];

    /* AT+CIPSEND=<id>,<len> */
    sprintf(cmd, "AT+CIPSEND=%d,%d", link_id, len);
    if (!ESP01S_SendATCmd(cmd, ">", 2000))
        return 0;

    /* 发送数据 */
    _send_bytes(data, len);

    /* 等待 SEND OK */
    {
        uint16_t elapsed = 0;
        uint8_t  ch;
        char     buf[32];
        uint16_t blen = 0;

        while (elapsed < 2000)
        {
            while (_ring_pop(&ch))
            {
                if (blen < sizeof(buf) - 1)
                {
                    buf[blen++] = (char)ch;
                    buf[blen] = '\0';
                }
                if (strstr(buf, "SEND OK") != NULL)
                    return 1;
                if (strstr(buf, "SEND FAIL") != NULL || strstr(buf, "ERROR") != NULL)
                    return 0;
            }
            delay_ms(10);
            elapsed += 10;
        }
    }

    return 0;
}

/* ===================== ESP-01S 初始化 ===================== */

/**
 * @brief  内部函数: 尝试连接 WiFi 并开启 TCP 服务器
 * @retval 1=成功  0=失败
 */
static uint8_t _connectWiFi(void)
{
    char cmd[128];
    uint8_t retry;

    /* 1. AT 测试 */
    ESP01S_SendATCmd("AT", "OK", 1000);
    delay_ms(200);

    /* 2. 设置 Station 模式 */
    ESP01S_SendATCmd("AT+CWMODE=1", "OK", 1000);
    delay_ms(200);

    /* 3. 连接 WiFi (带重试) */
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ESP01S_WIFI_SSID, ESP01S_WIFI_PASSWORD);
    for (retry = 0; retry < ESP01S_WIFI_RETRY_MAX; retry++)
    {
        if (ESP01S_SendATCmd(cmd, "OK", 15000))
        {
            /* WiFi 连接成功 */
            delay_ms(500);

            /* 4. 多连接模式 */
            ESP01S_SendATCmd("AT+CIPMUX=1", "OK", 2000);
            delay_ms(200);

            /* 5. 开启 TCP 服务器 */
            sprintf(cmd, "AT+CIPSERVER=1,%d", ESP01S_TCP_PORT);
            ESP01S_SendATCmd(cmd, "OK", 3000);
            delay_ms(200);

            return 1;  /* 全部成功 */
        }
        delay_ms(ESP01S_WIFI_RETRY_MS);  /* 等待后重试 */
    }

    return 0;  /* 全部重试失败 */
}

void ESP01S_Init(void)
{
    if (_connectWiFi())
    {
        ESP01S_WiFiConnected = 1;
    }
    else
    {
        /* 连接失败，进入离线模式 */
        ESP01S_WiFiConnected = 0;
    }
}

void ESP01S_ReconnectWiFi(void)
{
    /* 仅在离线状态才尝试重连 */
    if (ESP01S_WiFiConnected) return;

    if (_connectWiFi())
    {
        ESP01S_WiFiConnected = 1;
    }
}

/* ===================== 协议帧构造 ===================== */

/**
 * @brief  构造并发送一帧协议数据
 * @param  cmd:     命令类型
 * @param  payload: 数据载荷
 * @param  len:     载荷长度
 */
static void Protocol_SendFrame(uint8_t cmd, const uint8_t *payload, uint8_t len)
{
    uint8_t frame[258];
    uint8_t sum = cmd + len;
    uint8_t i;

    frame[0] = 0xAA;  /* HEAD */
    frame[1] = cmd;
    frame[2] = len;

    for (i = 0; i < len; i++)
    {
        frame[3 + i] = payload[i];
        sum += payload[i];
    }

    frame[3 + len] = sum;
    frame[4 + len] = 0x55;  /* END */

    ESP01S_SendData(0, frame, 5 + len);
}

/**
 * @brief  将 uint16_t 按大端序写入缓冲区
 */
static void _put_u16_be(uint8_t *buf, uint16_t val)
{
    buf[0] = (val >> 8) & 0xFF;
    buf[1] = val & 0xFF;
}

/**
 * @brief  从缓冲区按大端序读取 uint16_t
 */
static uint16_t _get_u16_be(const uint8_t *buf)
{
    return ((uint16_t)buf[0] << 8) | buf[1];
}

/* ===================== 数据上报 ===================== */

void ESP01S_SendSensorData(void)
{
    if (!ESP01S_WiFiConnected) return;
    uint8_t payload[14];

    /* float 用 memcpy (STM32 小端序, 载荷也用小端序, APP 端同样处理) */
    memcpy(&payload[0], &FishTank_Data.WaterTemp, 4);
    memcpy(&payload[4], &FishTank_Data.PH_Value, 4);
    payload[8] = FishTank_Data.WaterLevel;
    _put_u16_be(&payload[9], FishTank_Data.AirQuality);
    payload[11] = FishTank_Flag.RunMode;
    _put_u16_be(&payload[12], (uint16_t)FishTank_Feed.Countdown);

    Protocol_SendFrame(0x01, payload, 14);
}

void ESP01S_SendDeviceStatus(void)
{
    if (!ESP01S_WiFiConnected) return;
    uint8_t payload[5];
    uint8_t relay_state = 0;

    /* bit0=加热 bit1=加水 bit2=排水 bit3=增氧 */
    if (Relay_GetState(RELAY_HEAT))   relay_state |= (1 << 0);
    if (Relay_GetState(RELAY_FILL))   relay_state |= (1 << 1);
    if (Relay_GetState(RELAY_DRAIN))  relay_state |= (1 << 2);
    if (Relay_GetState(RELAY_OXYGEN)) relay_state |= (1 << 3);

    payload[0] = relay_state;
    payload[1] = FishTank_Flag.Alarm_Enable;
    payload[2] = FishTank_Feed.Feeding;
    payload[3] = 0x00;  /* Reserved */
    payload[4] = 0x00;  /* Reserved */

    Protocol_SendFrame(0x02, payload, 5);
}

void ESP01S_SendAlarmEvent(uint8_t alarm_type, uint8_t alarm_status)
{
    if (!ESP01S_WiFiConnected) return;
    uint8_t payload[2];

    payload[0] = alarm_type;
    payload[1] = alarm_status;

    Protocol_SendFrame(0x03, payload, 2);
}

/* ===================== 协议命令处理 ===================== */

/**
 * @brief  处理 APP 下发的控制命令
 */
static void Protocol_HandleCommand(uint8_t cmd, const uint8_t *payload, uint8_t len)
{
    switch (cmd)
    {
        case 0x10:  /* 查询状态 */
            ESP01S_SendSensorData();
            ESP01S_SendDeviceStatus();
            break;

        case 0x11:  /* 切换模式 */
            if (len >= 1 && payload[0] <= 1)
            {
                FishTank_Flag.RunMode = payload[0];
                /* 切换到自动时关闭所有手动继电器 */
                if (payload[0] == 0)
                {
                    Relay_AllOff();
                }
                ESP01S_SendDeviceStatus();
            }
            break;

        case 0x12:  /* 设备控制 (仅手动模式) */
            if (len >= 2 && FishTank_Flag.RunMode == 1)
            {
                Relay_Control(payload[0], payload[1]);
                ESP01S_SendDeviceStatus();
            }
            break;

        case 0x13:  /* 设置阈值 */
            if (len >= 20)
            {
                memcpy(&FishTank_Set.TempLower, &payload[0], 4);
                memcpy(&FishTank_Set.TempUpper, &payload[4], 4);
                FishTank_Set.AirQuality_Max = _get_u16_be(&payload[8]);
                memcpy(&FishTank_Set.PH_Lower, &payload[10], 4);
                memcpy(&FishTank_Set.PH_Upper, &payload[14], 4);
                FishTank_Set.WaterLevel_Min = payload[18];
                FishTank_Set.WaterLevel_Max = payload[19];
            }
            break;

        case 0x14:  /* 触发喂食 */
            Feed_Trigger = 1;
            break;

        case 0x15:  /* 设置报警使能 */
            if (len >= 1)
            {
                FishTank_Flag.Alarm_Enable = payload[0];
                ESP01S_SendDeviceStatus();
            }
            break;

        case 0x16:  /* 设置喂食间隔 */
            if (len >= 2)
            {
                FishTank_Set.FeedInterval = _get_u16_be(payload);
            }
            break;

        default:
            break;
    }
}

/* ===================== 接收数据处理 ===================== */

/* 解析状态机 */
typedef enum
{
    RX_IDLE = 0,
    RX_IPD,         /* 正在解析 +IPD 头 */
    RX_FRAME_HEAD,  /* 等待帧头 0xAA */
    RX_FRAME_BODY,  /* 正在接收帧体 */
} RxState;

static RxState    _rx_state = RX_IDLE;
static uint8_t    _ipd_buf[32];
static uint8_t    _ipd_idx = 0;
static uint8_t    _frame_buf[258];
static uint8_t    _frame_idx = 0;
static uint8_t    _frame_len = 0;  /* 完整帧需要的字节数: 3(HEAD+CMD+LEN) + payload_len + 1(SUM) + 1(END) */
static uint8_t    _ipd_link_id = 0;
static uint16_t   _ipd_data_len = 0;

/**
 * @brief  处理一个接收到的完整协议帧
 */
static void _handle_frame(void)
{
    uint8_t cmd  = _frame_buf[1];
    uint8_t dlen = _frame_buf[2];
    uint8_t sum  = 0;
    uint8_t i;

    /* 校验长度 */
    if (_frame_idx < 4 + dlen) return;  /* 帧不完整 */

    /* 校验 SUM */
    for (i = 1; i <= 2 + dlen; i++)
    {
        sum += _frame_buf[i];
    }
    if (sum != _frame_buf[3 + dlen]) return;  /* 校验失败 */

    /* 校验 END */
    if (_frame_buf[4 + dlen] != 0x55) return;

    /* 执行命令 */
    Protocol_HandleCommand(cmd, &_frame_buf[3], dlen);
}

/**
 * @brief  处理接收缓冲区 (在 Timing_50ms 中调用)
 *
 * 解析两种数据:
 * 1. +IPD,<id>,<len>:<data>  (TCP 接收)
 * 2. AT 指令应答 (OK, ERROR, > 等)
 */
void ESP01S_ProcessReceived(void)
{
    uint8_t ch;

    while (_ring_pop(&ch))
    {
        switch (_rx_state)
        {
            /* ==================== IDLE: 等待 +IPD 或帧头 ==================== */
            case RX_IDLE:
            {
                if (ch == '+')
                {
                    _rx_state = RX_IPD;
                    _ipd_idx = 0;
                    _ipd_buf[_ipd_idx++] = ch;
                }
                else if (ch == 0xAA)
                {
                    /* 直接收到帧头 (可能透传模式) */
                    _rx_state = RX_FRAME_HEAD;
                    _frame_buf[0] = ch;
                    _frame_idx = 1;
                }
                /* 其他字符忽略 (AT 应答等) */
                break;
            }

            /* ==================== IPD 头解析 ==================== */
            case RX_IPD:
            {
                _ipd_buf[_ipd_idx++] = ch;

                /* 防止溢出 */
                if (_ipd_idx >= sizeof(_ipd_buf) - 1)
                {
                    _rx_state = RX_IDLE;
                    break;
                }

                /* 检查是否收到冒号 (IPD 头结束) */
                if (ch == ':')
                {
                    _ipd_buf[_ipd_idx] = '\0';

                    /* 解析 +IPD,<id>,<len>: */
                    if (sscanf((char *)_ipd_buf, "+IPD,%hhu,%hu:", &_ipd_link_id, &_ipd_data_len) == 2)
                    {
                        /* 准备接收数据: 等待帧头 0xAA */
                        _rx_state = RX_FRAME_HEAD;
                        _frame_idx = 0;
                    }
                    else
                    {
                        _rx_state = RX_IDLE;
                    }
                }
                break;
            }

            /* ==================== 等待帧头 ==================== */
            case RX_FRAME_HEAD:
            {
                if (ch == 0xAA)
                {
                    _frame_buf[0] = ch;
                    _frame_idx = 1;
                    _rx_state = RX_FRAME_BODY;
                }
                /* 不是帧头则继续等待 */
                break;
            }

            /* ==================== 接收帧体 ==================== */
            case RX_FRAME_BODY:
            {
                _frame_buf[_frame_idx++] = ch;

                /* 收到 CMD + LEN 后可以计算完整帧长 */
                if (_frame_idx == 3)
                {
                    _frame_len = 3 + _frame_buf[2] + 1 + 1;  /* HEAD+CMD+LEN + payload + SUM + END */
                    if (_frame_len > sizeof(_frame_buf))
                    {
                        /* 非法长度, 重置 */
                        _rx_state = RX_IDLE;
                        break;
                    }
                }

                /* 检查帧是否完整 */
                if (_frame_idx >= 3 && _frame_idx >= _frame_len)
                {
                    /* 校验并处理帧 */
                    if (_frame_buf[_frame_len - 1] == 0x55)
                    {
                        _handle_frame();
                    }
                    /* 处理完后回到 IDLE */
                    _rx_state = RX_IDLE;
                }
                break;
            }

            default:
                _rx_state = RX_IDLE;
                break;
        }
    }
}
