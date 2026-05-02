/**
 * @file    bsp_esp01s.h
 * @brief   ESP-01S WiFi 模块 AT 命令驱动 + 通信协议
 * @version 1.0
 */
#ifndef __BSP_ESP01S_H
#define __BSP_ESP01S_H

#include "sys.h"

/* ===================== WiFi 配置 ===================== */
#define ESP01S_WIFI_SSID        "YOUR_SSID"       /* WiFi 名称 */
#define ESP01S_WIFI_PASSWORD    "YOUR_PASSWORD"   /* WiFi 密码 */
#define ESP01S_TCP_PORT         8080              /* TCP 服务器端口 */

/* ===================== 缓冲区配置 ===================== */
#define ESP01S_RX_BUF_SIZE      256               /* 接收环形缓冲区大小 */
#define ESP01S_WIFI_RETRY_MAX   3                  /* WiFi 重试最大次数 */
#define ESP01S_WIFI_RETRY_MS    5000               /* 每次重试间隔 (ms) */

/* ===================== WiFi 状态 ===================== */
extern volatile uint8_t ESP01S_WiFiConnected;       /* 0=离线 1=已连接 */

/* ===================== 函数声明 ===================== */

/**
 * @brief  初始化 ESP-01S: AT 测试 -> Station 模式 -> 连接 WiFi -> 开 TCP 服务器
 * @note   连接失败时进入离线模式，后续通过 ESP01S_ReconnectWiFi() 重试
 */
void ESP01S_Init(void);

/**
 * @brief  后台重连 WiFi (在 Timing_1s 中调用)
 * @note   仅在离线状态下尝试重连，成功后开启 TCP 服务器并更新 WiFi 状态标志
 */
void ESP01S_ReconnectWiFi(void);

/**
 * @brief  发送 AT 指令并等待应答
 * @param  cmd:  AT 指令字符串 (不含 \r\n)
 * @param  ack:  期望应答字符串
 * @param  timeout_ms: 超时 (ms)
 * @retval 1=成功  0=超时
 */
uint8_t ESP01S_SendATCmd(const char *cmd, const char *ack, uint16_t timeout_ms);

/**
 * @brief  通过 TCP 连接发送数据
 * @param  link_id: 连接 ID
 * @param  data:    数据指针
 * @param  len:     数据长度
 * @retval 1=成功  0=失败
 */
uint8_t ESP01S_SendData(uint8_t link_id, const uint8_t *data, uint16_t len);

/**
 * @brief  处理接收缓冲区中的数据 (建议在 Timing_50ms 中调用)
 */
void ESP01S_ProcessReceived(void);

/* ===================== 协议上报接口 ===================== */

/**
 * @brief  上报传感器数据 (CMD 0x01)
 */
void ESP01S_SendSensorData(void);

/**
 * @brief  上报设备状态 (CMD 0x02)
 */
void ESP01S_SendDeviceStatus(void);

/**
 * @brief  上报报警事件 (CMD 0x03)
 */
void ESP01S_SendAlarmEvent(uint8_t alarm_type, uint8_t alarm_status);

#endif /* __BSP_ESP01S_H */
