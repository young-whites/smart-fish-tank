#include <rtthread.h>
#include <stdint.h>
#include "app_data.h"
#include "app_sensor.h"
#include "app_control.h"
#include "app_display.h"
#include "app_key.h"
#include "app_alarm.h"
#include "app_feed.h"
#include "app_wifi.h"

int main(void)
{
    app_data_init();
    app_sensor_init();
    app_control_init();
    app_display_init();
    app_key_init();
    app_alarm_init();
    app_feed_init();
    app_wifi_init();

    return RT_EOK;
}
