#ifndef APP_SENSOR_H
#define APP_SENSOR_H

#include <rtthread.h>
#include <stdint.h>

void app_sensor_init(void);

/* 传感器状态 */
extern uint8_t g_sensor_temp_valid;  /* 1=DS18B20在线 */

#endif
