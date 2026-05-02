#include "app_data.h"
#include "app_display.h"
#include "drv_oled.h"
#include "stm32f1xx.h"
#include <stdint.h>

static uint8_t current_page = 0;

static void show_main_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    /* 第1行: Temp: xx.xC */
    oled_draw_string(0, 0, "Temp:");
    oled_draw_float(30, 0, g_sensor.water_temp, 1);
    oled_draw_char(66, 0, 'C');

    /* 第3行: Water: xx% */
    oled_draw_string(0, 2, "Water:");
    oled_draw_number(36, 2, (int32_t)g_sensor.water_level);
    oled_draw_char(60, 2, '%');

    /* 第5行: PH: x.x */
    oled_draw_string(0, 4, "PH:");
    oled_draw_float(18, 4, g_sensor.ph_value, 1);

    /* 第7行: Auto/Manual + Air */
    oled_draw_string(0, 6, g_sensor.run_mode == 0 ? "Auto" : "Manu");
    oled_draw_string(30, 6, "Air:");
    oled_draw_number(54, 6, (int32_t)g_sensor.air_quality);

    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

static struct rt_thread display_thread;
static rt_uint8_t display_stack[512];

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
    rt_thread_init(&display_thread,
        "t_disp", display_thread_entry, RT_NULL,
        &display_stack[0], sizeof(display_stack),
        18, 10);
    rt_thread_startup(&display_thread);
}
