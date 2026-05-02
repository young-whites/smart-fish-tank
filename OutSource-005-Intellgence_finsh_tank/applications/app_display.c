#include "app_data.h"
#include "app_display.h"
#include "drv_oled.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdio.h>

static uint8_t current_page = 0;

static void show_main_page(void)
{
    char buf[22];

    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    snprintf(buf, sizeof(buf), "Temp: %.1fC", g_sensor.water_temp);
    oled_draw_string(0, 0, buf);

    snprintf(buf, sizeof(buf), "Water: %d%%", g_sensor.water_level);
    oled_draw_string(0, 2, buf);

    snprintf(buf, sizeof(buf), "PH: %.1f", g_sensor.ph_value);
    oled_draw_string(0, 4, buf);

    snprintf(buf, sizeof(buf), "%s Air:%d",
             g_sensor.run_mode == 0 ? "Auto" : "Manual",
             g_sensor.air_quality);
    oled_draw_string(0, 6, buf);

    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

static void display_thread_entry(void *param)
{
    oled_init();
    rt_thread_mdelay(500);

    while(1) {
        switch(current_page) {
            case 0: show_main_page(); break;
            default: show_main_page(); break;
        }
        rt_thread_mdelay(1000);
    }
}

void app_display_init(void)
{
    rt_thread_t tid = rt_thread_create("t_disp", display_thread_entry, RT_NULL, 1024, 18, 10);
    if(tid) rt_thread_startup(tid);
}
