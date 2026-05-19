/**
 * @file    bsp_esp01s.h
 * @brief   ESP-01S WiFi module driver - public interface
 *
 * Hardware: ESP-01S on USART2 (PA2=TX, PA3=RX), 115200 baud
 * Note:     RST/EN pins are NOT physically connected on this board.
 */
#ifndef __BSP_ESP01S_H
#define __BSP_ESP01S_H

#include "stm32f10x.h"

extern uint8_t ESP01S_WiFiConnected;

void ESP01S_Init(void);                    /* Initialize USART2 + AT config sequence */
void ESP01S_Process(void);                 /* Main-loop: parse +IPD / CONNECT / DISCONNECT */
void ESP01S_SendSensorData(void);          /* Send sensor data frame (CMD 0x01, 14 bytes) to APP */
void ESP01S_SendDeviceStatus(void);        /* Send device status frame (CMD 0x02, 5 bytes) to APP */
uint8_t ESP01S_IsClientConnected(void);    /* Check if a TCP client is connected */
void ESP01S_DumpRingBuf(void);             /* Debug: print ring buffer raw hex */
void ESP01S_FlushRingBuf(void);            /* Force flush ring buffer */
void ESP01S_QueryStatus(void);             /* Query ESP for connection status via AT+CIPSTATUS */
void ESP01S_EnsureServer(void);            /* Check if TCP server is running, restart if not */

#endif /* __BSP_ESP01S_H */
