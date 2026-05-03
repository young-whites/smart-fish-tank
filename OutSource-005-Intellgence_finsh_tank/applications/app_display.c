/**
 * @file    app_display.c
 * @brief   OLED 显示模块 (局部刷新优化)
 *
 *  刷新策略:
 *    - 页面切换: oled_clear + 全量绘制 + oled_refresh (完整刷新)
 *    - 定时更新: 仅清除数据区域 + 局部绘制 + oled_refresh (无闪烁)
 *    - 无效按键: 不刷新
 */

#include "app_display.h"
#include "app_data.h"
#include "app_key.h"
#include "drv_oled.h"
#include "app_sensor.h"
#include <rtthread.h>
#include <stdint.h>
#include <stdio.h>

/* ===================== 页面状态 ===================== */
static uint8_t main_page   = 0;  /* 0=主页面, 1=阈值设置, 2=手动控制 */
static uint8_t sub_page    = 0;  /* 0=第一页, 1=第二页 */
static uint8_t edit_cursor = 0;  /* 阈值/手动页当前编辑项 */
static uint8_t page_changed = 1; /* 1=需要完整刷新, 0=仅局部刷新 */

static uint8_t get_max_subs(void)
{
    switch (main_page) {
        case 0: return 2;
        case 1: return 2;
        case 2: return 1;
        default: return 1;
    }
}

/* ===================== 工具函数 ===================== */

static void draw_page_indicator(void)
{
    char buf[8];
    rt_snprintf(buf, sizeof(buf), "P%d/%d", sub_page + 1, get_max_subs());
    oled_draw_string(90, 6, buf);
}

/* ===================== 主页面 (混合汉字+ASCII) ===================== */
/* 主页每行: 汉字 label 占 col 0~31, 数据占 col 32~127 */

static void draw_main_p0_data(void)
{
    char buf[24];

    oled_clear_region(0, 7, 0, 127);

    if(g_sensor_temp_valid) {
        int ti = (int)g_sensor.water_temp;
        int tf = ((int)(g_sensor.water_temp * 100)) % 100;
        if(tf < 0) tf = -tf;
        rt_snprintf(buf, sizeof(buf), "Temp:  %d.%02d C", ti, tf);
    } else
        rt_snprintf(buf, sizeof(buf), "Temp:  --.- C");
    oled_draw_string(0, 0, buf);

    rt_snprintf(buf, sizeof(buf), "Air:   %d", g_sensor.air_quality);
    oled_draw_string(0, 1, buf);

    rt_snprintf(buf, sizeof(buf), "Water: %d%%", g_sensor.water_level);
    oled_draw_string(0, 2, buf);

    {
        int pi = (int)g_sensor.ph_value;
        int pf = ((int)(g_sensor.ph_value * 100)) % 100;
        if(pf < 0) pf = -pf;
        rt_snprintf(buf, sizeof(buf), "PH:    %d.%02d", pi, pf);
    }
    oled_draw_string(0, 3, buf);

    draw_page_indicator();
}

static void draw_main_p1_data(void)
{
    char buf[24];

    oled_clear_region(0, 7, 0, 127);

    oled_draw_string(0, 0, (g_sensor.run_mode == MODE_AUTO) ? "Mode:  Auto" : "Mode:  Manu");

    if (g_sensor.feed_countdown > 0)
        rt_snprintf(buf, sizeof(buf), "Feed:  %02d:%02d",
                    (int)(g_sensor.feed_countdown / 60),
                    (int)(g_sensor.feed_countdown % 60));
    else
        rt_snprintf(buf, sizeof(buf), "Feed:  --:--");
    oled_draw_string(0, 1, buf);

    oled_draw_string(0, 2, "WiFi:  W-");

    rt_snprintf(buf, sizeof(buf), "Heat:%s  O2:%s",
                g_status.relay_heat ? "ON" : "OF",
                g_status.relay_oxygen ? "ON" : "OF");
    oled_draw_string(0, 3, buf);

    draw_page_indicator();
}

/* 完整绘制主页面 (页面切换时调用) */
static void show_main_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    if (sub_page == 0)
        draw_main_p0_data();
    else
        draw_main_p1_data();

    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

/* 局部更新主页面 (定时刷新时调用, 汉字不动) */
static void update_main_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);

    if (sub_page == 0)
        draw_main_p0_data();
    else
        draw_main_p1_data();

    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

/* ===================== 阈值设置页面 (纯ASCII) ===================== */

static void draw_threshold_p0_data(void)
{
    char buf[24];

    oled_clear_region(0, 3, 0, 127);

    oled_draw_string(0, 0, "-- Threshold Set --");

    rt_snprintf(buf, sizeof(buf), "TLo:%d.%dC %s",
                (int)g_threshold.temp_lower,
                ((int)(g_threshold.temp_lower * 10)) % 10,
                (edit_cursor == 0) ? "<<" : "");
    oled_draw_string(0, 1, buf);

    rt_snprintf(buf, sizeof(buf), "THi:%d.%dC %s",
                (int)g_threshold.temp_upper,
                ((int)(g_threshold.temp_upper * 10)) % 10,
                (edit_cursor == 1) ? "<<" : "");
    oled_draw_string(0, 2, buf);

    rt_snprintf(buf, sizeof(buf), "Air:%d %s",
                g_threshold.air_quality_max,
                (edit_cursor == 2) ? "<<" : "");
    oled_draw_string(0, 3, buf);

    draw_page_indicator();
}

static void draw_threshold_p1_data(void)
{
    char buf[24];

    oled_clear_region(0, 3, 0, 127);

    rt_snprintf(buf, sizeof(buf), "PHL:%d.%d %s",
                (int)g_threshold.ph_lower,
                ((int)(g_threshold.ph_lower * 10)) % 10,
                (edit_cursor == 3) ? "<<" : "");
    oled_draw_string(0, 0, buf);

    rt_snprintf(buf, sizeof(buf), "PHH:%d.%d %s",
                (int)g_threshold.ph_upper,
                ((int)(g_threshold.ph_upper * 10)) % 10,
                (edit_cursor == 4) ? "<<" : "");
    oled_draw_string(0, 1, buf);

    rt_snprintf(buf, sizeof(buf), "WLM:%d%% %s",
                g_threshold.water_level_min,
                (edit_cursor == 5) ? "<<" : "");
    oled_draw_string(0, 2, buf);

    rt_snprintf(buf, sizeof(buf), "WLX:%d%% %s",
                g_threshold.water_level_max,
                (edit_cursor == 6) ? "<<" : "");
    oled_draw_string(0, 3, buf);

    draw_page_indicator();
}

static void show_threshold_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    if (sub_page == 0)
        draw_threshold_p0_data();
    else
        draw_threshold_p1_data();

    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

static void update_threshold_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);

    if (sub_page == 0)
        draw_threshold_p0_data();
    else
        draw_threshold_p1_data();

    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

/* ===================== 手动控制页面 (纯ASCII) ===================== */

static const char *manual_labels[] = {
    "Heat", "Fill", "Drain", "Oxygen", "Alarm", "Feed"
};
static uint8_t *manual_status[] = {
    &g_status.relay_heat, &g_status.relay_fill,
    &g_status.relay_drain, &g_status.relay_oxygen,
    &g_status.alarm_enable, &g_status.feeding
};
#define MANUAL_ITEMS  6

static void draw_manual_data(void)
{
    char buf[24];
    uint8_t i;

    oled_clear_region(0, 6, 0, 127);

    oled_draw_string(0, 0, "-- Manual Control --");

    for (i = 0; i < MANUAL_ITEMS; i++) {
        uint8_t row = 1 + i;
        char mark = (i == edit_cursor) ? '>' : ' ';
        rt_snprintf(buf, sizeof(buf), "%c %s: %3s %s",
                    mark, manual_labels[i],
                    (*manual_status[i]) ? "ON" : "OFF",
                    (i == edit_cursor) ? "<<" : "");
        oled_draw_string(0, row, buf);
    }

    draw_page_indicator();
}

static void show_manual_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();
    draw_manual_data();
    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

static void update_manual_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    draw_manual_data();
    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

/* ===================== 按键处理 ===================== */

static int handle_key_event(uint8_t key)
{
    switch (key) {
    case 1: /* KEY1: 切换主页面 */
        main_page++;
        if (main_page > 2) main_page = 0;
        sub_page = 0;
        edit_cursor = 0;
        page_changed = 1;
        return 1;

    case 2: /* KEY2: 切换子页 */
        if (main_page == 0 || main_page == 1) {
            sub_page++;
            if (sub_page >= get_max_subs()) sub_page = 0;
            page_changed = 1;
            return 1;
        }
        return 0;

    case 3: /* KEY3: 阈值+1 / 手动开关 */
        if (main_page == 1) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            switch (edit_cursor) {
                case 0: g_threshold.temp_lower += 1.0f; break;
                case 1: g_threshold.temp_upper += 1.0f; break;
                case 2: g_threshold.air_quality_max += 10; break;
                case 3: g_threshold.ph_lower += 0.1f; break;
                case 4: g_threshold.ph_upper += 0.1f; break;
                case 5: g_threshold.water_level_min += 5; break;
                case 6: g_threshold.water_level_max += 5; break;
            }
            rt_mutex_release(&mutex_data);
            return 1;
        } else if (main_page == 2) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            *manual_status[edit_cursor] = !(*manual_status[edit_cursor]);
            rt_mutex_release(&mutex_data);
            return 1;
        }
        return 0;

    case 4: /* KEY4: 阈值-1 */
        if (main_page == 1) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            switch (edit_cursor) {
                case 0: if (g_threshold.temp_lower > 0) g_threshold.temp_lower -= 1.0f; break;
                case 1: if (g_threshold.temp_upper > 0) g_threshold.temp_upper -= 1.0f; break;
                case 2: if (g_threshold.air_quality_max >= 10) g_threshold.air_quality_max -= 10; break;
                case 3: if (g_threshold.ph_lower > 0) g_threshold.ph_lower -= 0.1f; break;
                case 4: if (g_threshold.ph_upper > 0) g_threshold.ph_upper -= 0.1f; break;
                case 5: if (g_threshold.water_level_min >= 5) g_threshold.water_level_min -= 5; break;
                case 6: if (g_threshold.water_level_max >= 5) g_threshold.water_level_max -= 5; break;
            }
            rt_mutex_release(&mutex_data);
            return 1;
        }
        return 0;

    case 5: /* KEY5: 切换编辑项 */
        if (main_page == 1) {
            edit_cursor++;
            if (edit_cursor >= 7) edit_cursor = 0;
            return 1;
        } else if (main_page == 2) {
            edit_cursor++;
            if (edit_cursor >= MANUAL_ITEMS) edit_cursor = 0;
            return 1;
        }
        return 0;

    default:
        return 0;
    }
}

/* ===================== 显示调度 ===================== */

static void full_refresh(void)
{
    switch (main_page) {
        case 0: show_main_page();     break;
        case 1: show_threshold_page(); break;
        case 2: show_manual_page();    break;
    }
}

static void partial_refresh(void)
{
    switch (main_page) {
        case 0: update_main_page();     break;
        case 1: update_threshold_page(); break;
        case 2: update_manual_page();    break;
    }
}

/* ===================== 线程 ===================== */

static struct rt_thread display_thread;
static rt_uint8_t display_stack[1024];

static void display_thread_entry(void *param)
{
    oled_init();
    rt_thread_mdelay(500);

    full_refresh();

    while (1) {
        rt_err_t result = rt_sem_take(&sem_key, rt_tick_from_millisecond(1000));
        int need_refresh = 0;

        if (result == RT_EOK) {
            uint8_t key = g_key_event;
            g_key_event = 0;
            need_refresh = handle_key_event(key);
        } else {
            need_refresh = 1; /* 超时: 定时刷新数据 */
        }

        if (need_refresh) {
            if (page_changed) {
                page_changed = 0;
                full_refresh();    /* 页面切换: 完整清除+刷新 */
            } else {
                partial_refresh(); /* 定时/编辑: 仅数据区域刷新 */
            }
        }
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

#include <finsh.h>

static void dpage(void)
{
    rt_kprintf("main_page=%d sub_page=%d cursor=%d changed=%d\n",
               main_page, sub_page, edit_cursor, page_changed);
}
MSH_CMD_EXPORT(dpage, show display page state);

static void dset(int p)
{
    main_page = p;
    sub_page = 0;
    edit_cursor = 0;
    page_changed = 1;
    rt_kprintf("page set to %d\n", p);
}
MSH_CMD_EXPORT(dset, set display page (0=main 1=thresh 2=manual));
