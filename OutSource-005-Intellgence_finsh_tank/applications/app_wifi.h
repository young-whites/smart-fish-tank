#ifndef APP_WIFI_H
#define APP_WIFI_H

#include <rtthread.h>
#include <stdint.h>

void app_wifi_init(void);
extern volatile uint8_t g_wifi_connected;

#endif
