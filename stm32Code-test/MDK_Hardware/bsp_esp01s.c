#include "bsp_esp01s.h"

uint8_t ESP01S_WiFiConnected = 0;  // 默认离线

void ESP01S_Init(void)
{
    // TODO: ESP-01S初始化 (AT指令配置WiFi+TCP服务器)
    ESP01S_WiFiConnected = 0;
}

void ESP01S_ReconnectWiFi(void)
{
    // TODO: 后台重连WiFi
}
