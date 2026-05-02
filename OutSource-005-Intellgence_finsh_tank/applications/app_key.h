#ifndef APP_KEY_H
#define APP_KEY_H

#include <rtthread.h>
#include <stdint.h>

void app_key_init(void);
extern uint8_t g_key_event;

#endif
