/**
 * @file    app_display.c
 * @brief   OLED 显示模块 (主页汉字+阈值/手动页纯ASCII，节省RAM)
 *
 *  OLED: 128x64 = 8 pages, 每 page = 8px 高
 *  主页面: 16x16 汉字标签 + 6x12 ASCII (混合行)
 *  阈值/手动页: 纯 6x12 ASCII (每页 8 行, 最多 8 行内容)
 *
 *  页面导航:
 *    KEY1 → 切换主页面 (主页面→阈值设置→手动控制→主页面)
 *    KEY2 → 切换子页 (Page 1/2)
 *    KEY3 → 阈值设置页: 当前项 +1  /  手动控制页: 开关切换
 *    KEY4 → 阈值设置页: 当前项 -1
 *    KEY5 → 阈值/手动页: 切换当前编辑项
 */

#include "app_display.h"
#include "app_data.h"
#include "app_key.h"
#include "drv_oled.h"
#include <rtthread.h>
#include <stdint.h>
#include <stdio.h>

/* 仅主页使用汉字库 */
#include "font_lib.h"

/* ===================== 页面状态 ===================== */
static uint8_t main_page   = 0;  /* 0=主页面, 1=阈值设置, 2=手动控制 */
static uint8_t sub_page    = 0;  /* 0=第一页, 1=第二页 */
static uint8_t edit_cursor = 0;  /* 阈值/手动页当前编辑项 */

/* 主页面最大子页数 */
static uint8_t get_max_subs(void)
{
    switch (main_page) {
        case 0: return 2;
        case 1: return 2;
        case 2: return 1;
        default: return 1;
    }
}

/* ===================== 页码指示 ===================== */
static void draw_page_indicator(void)
{
    char buf[8];
    rt_snprintf(buf, sizeof(buf), "P%d/%d", sub_page + 1, get_max_subs());
    oled_draw_string(90, 6, buf);
}

/* ===================== 主页面 (混合汉字+ASCII) ===================== */

static void show_main_page_0(void)
{
    char buf[24];

    rt_snprintf(buf, sizeof(buf), "%.1fC", g_sensor.water_temp);
    oled_draw_mix_line(0, "温度", buf);

    rt_snprintf(buf, sizeof(buf), "%d", g_sensor.air_quality);
    oled_draw_mix_line(2, "空气", buf);

    rt_snprintf(buf, sizeof(buf), "%d%%", g_sensor.water_level);
    oled_draw_mix_line(4, "水位", buf);

    rt_snprintf(buf, sizeof(buf), "%.1f", g_sensor.ph_value);
    oled_draw_mix_line(6, "PH值", buf);
}

static void show_main_page_1(void)
{
    char buf[24];
    const char *mode_str;

    mode_str = (g_sensor.run_mode == MODE_AUTO) ? "Auto" : "Manu";
    oled_draw_mix_line(0, "模式", mode_str);

    if (g_sensor.feed_countdown > 0)
        rt_snprintf(buf, sizeof(buf), "%02d:%02d",
                    (int)(g_sensor.feed_countdown / 60),
                    (int)(g_sensor.feed_countdown % 60));
    else
        rt_snprintf(buf, sizeof(buf), "--:--");
    oled_draw_mix_line(2, "喂食", buf);

    oled_draw_mix_line(4, "WiFi", "W-");

    rt_snprintf(buf, sizeof(buf), "%s %s",
                g_status.relay_heat ? "ON" : "OF",
                g_status.relay_oxygen ? "ON" : "OF");
    oled_draw_mix_line(6, "热氧", buf);
}

static void show_main_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    if (sub_page == 0)
        show_main_page_0();
    else
        show_main_page_1();

    draw_page_indicator();
    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

/* ===================== 阈值设置页面 (纯ASCII) ===================== */

static void show_threshold_page_0(void)
{
    char buf[24];
    char cur;

    oled_draw_string(0, 0, "-- Threshold Set --");

    cur = (edit_cursor == 0) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "TempLow: %.1fC%c", g_threshold.temp_lower, cur);
    oled_draw_string(0, 2, buf);

    cur = (edit_cursor == 1) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "TempUp:  %.1fC%c", g_threshold.temp_upper, cur);
    oled_draw_string(0, 3, buf);

    cur = (edit_cursor == 2) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "AirMax:  %d%c", g_threshold.air_quality_max, cur);
    oled_draw_string(0, 4, buf);

    cur = (edit_cursor == 3) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "PH Low:  %.1f%c", g_threshold.ph_lower, cur);
    oled_draw_string(0, 5, buf);
}

static void show_threshold_page_1(void)
{
    char buf[24];
    char cur;

    cur = (edit_cursor == 3) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "PH Low:  %.1f%c", g_threshold.ph_lower, cur);
    oled_draw_string(0, 0, buf);

    cur = (edit_cursor == 4) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "PH Up:   %.1f%c", g_threshold.ph_upper, cur);
    oled_draw_string(0, 1, buf);

    cur = (edit_cursor == 5) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "WL Min:  %d%%%c", g_threshold.water_level_min, cur);
    oled_draw_string(0, 2, buf);

    cur = (edit_cursor == 6) ? '<' : ' ';
    rt_snprintf(buf, sizeof(buf), "WL Max:  %d%%%c", g_threshold.water_level_max, cur);
    oled_draw_string(0, 3, buf);
}

static void show_threshold_page(void)
{
    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    if (sub_page == 0)
        show_threshold_page_0();
    else
        show_threshold_page_1();

    draw_page_indicator();
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

static void show_manual_page(void)
{
    char buf[24];
    uint8_t i;

    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    oled_draw_string(0, 0, "-- Manual Control --");

    for (i = 0; i < MANUAL_ITEMS && i < 6; i++) {
        uint8_t row = 1 + i;
        if (row > 6) break;

        char mark = (i == edit_cursor) ? '>' : ' ';
        rt_snprintf(buf, sizeof(buf), "%c %s: %s",
                    mark, manual_labels[i],
                    (*manual_status[i]) ? "ON " : "OFF");
        oled_draw_string(0, row, buf);
    }

    draw_page_indicator();
    oled_refresh();
    rt_mutex_release(&mutex_oled);
}

/* ===================== 按键处理 ===================== */

/* @return 1=状态变更需刷新, 0=无效按键不刷新 */
static int handle_key_event(uint8_t key)
{
    switch (key) {
    case 1: /* KEY1: 切换主页面 (所有页面通用) */
        main_page++;
        if (main_page > 2) main_page = 0;
        sub_page = 0;
        edit_cursor = 0;
        return 1;

    case 2: /* KEY2: 切换子页 (仅主页和阈值页) */
        if (main_page == 0 || main_page == 1) {
            sub_page++;
            if (sub_page >= get_max_subs()) sub_page = 0;
            return 1;
        }
        return 0;

    case 3: /* KEY3: 阈值+1 或 手动开关切换 */
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

    case 4: /* KEY4: 阈值-1 (仅阈值页) */
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

    case 5: /* KEY5: 切换编辑项 (仅阈值页和手动页) */
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

/* ===================== 显示刷新 ===================== */

static void refresh_display(void)
{
    switch (main_page) {
        case 0: show_main_page();     break;
        case 1: show_threshold_page(); break;
        case 2: show_manual_page();    break;
        default: show_main_page();     break;
    }
}

/* ===================== 线程 ===================== */

static struct rt_thread display_thread;
static rt_uint8_t display_stack[512];

static void display_thread_entry(void *param)
{
    oled_init();
    rt_thread_mdelay(500);

    refresh_display();

    while (1) {
        rt_err_t result = rt_sem_take(&sem_key, rt_tick_from_millisecond(1000));

        int need_refresh = 0;

        if (result == RT_EOK) {
            uint8_t key = g_key_event;
            g_key_event = 0;
            need_refresh = handle_key_event(key);
        } else {
            need_refresh = 1;
        }

        if (need_refresh) {
            refresh_display();
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
