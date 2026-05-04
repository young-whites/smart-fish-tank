#ifndef __BSP_ESP01S_H
#define __BSP_ESP01S_H
#include "stm32f10x.h"

extern uint8_t ESP01S_WiFiConnected;  // WiFi在线标志 0=离线, 1=在线

void ESP01S_Init(void);
void ESP01S_ReconnectWiFi(void);

#endif /*__BSP_ESP01S_H*/
