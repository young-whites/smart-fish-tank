/**
 * @file    app_display.c
 * @brief   OLED 显示模块 (16x16 汉字 + 6x12 ASCII 混合显示)
 *
 *  OLED: 128x64 = 8 pages, 每 page = 8px 高
 *  16x16 汉字占 2 pages (16px), 每页最多 4 行汉字
 *  超出行放入子页, KEY2 翻页
 *
 *  页面导航:
 *    KEY1 → 切换主页面 (主页面→阈值设置→手动控制→主页面)
 *    KEY2 → 切换子页 (Page 1/2)
 *    KEY3 → 阈值设置页: 当前项 +1  /  手动控制页: 开关切换
 *    KEY4 → 阈值设置页: 当前项 -1
 */

#include "app_display.h"
#include "app_data.h"
#include "app_key.h"
#include "drv_oled.h"
#include "font_lib.h"
#include <rtthread.h>
#include <stdint.h>
#include <stdio.h>

/* ===================== 页面状态 ===================== */
static uint8_t main_page   = 0;  /* 0=主页面, 1=阈值设置, 2=手动控制 */
static uint8_t sub_page    = 0;  /* 0=第一页, 1=第二页 */
static uint8_t edit_cursor = 0;  /* 阈值设置页当前编辑项 (0~6) */

/* 主页面最大子页数 */
#define MAIN_PAGE0_SUBS    2
#define MAIN_PAGE1_SUBS    2
#define MAIN_PAGE2_SUBS    1

static uint8_t get_max_subs(void)
{
    switch (main_page) {
        case 0: return MAIN_PAGE0_SUBS;
        case 1: return MAIN_PAGE1_SUBS;
        case 2: return MAIN_PAGE2_SUBS;
        default: return 1;
    }
}

/* ===================== 页码指示 ===================== */
static void draw_page_indicator(void)
{
    char buf[8];
    uint8_t max_s = get_max_subs();
    uint8_t cur_s = sub_page + 1;
    /* 底部 12px 区域: 在 page 6~7 显示 */
    /* 清除指示器区域 */
    oled_draw_string(0, 6, "                ");  /* 16 chars * 6px = 96px */
    /* 显示 "Page x/y" */
    rt_snprintf(buf, sizeof(buf), "P%d/%d", cur_s, max_s);
    oled_draw_string(90, 6, buf);
}

/* ===================== 主页面 ===================== */

static void show_main_page_0(void)
{
    char buf[24];

    /* 行0 (page 0-1): 温度: xx.x */
    rt_snprintf(buf, sizeof(buf), "%.1fC", g_sensor.water_temp);
    oled_draw_mix_line(0, "温度", buf);

    /* 行1 (page 2-3): 空气: xxx */
    rt_snprintf(buf, sizeof(buf), "%d", g_sensor.air_quality);
    oled_draw_mix_line(2, "空气", buf);

    /* 行2 (page 4-5): 水位: xx% */
    rt_snprintf(buf, sizeof(buf), "%d%%", g_sensor.water_level);
    oled_draw_mix_line(4, "水位", buf);

    /* 行3 (page 6-7上半): PH值: x.x */
    rt_snprintf(buf, sizeof(buf), "%.1f", g_sensor.ph_value);
    oled_draw_mix_line(6, "PH值", buf);
    /* page 6下半留给页码指示, 在 refresh 前绘制 */
}

static void show_main_page_1(void)
{
    char buf[24];
    const char *mode_str;
    const char *wifi_str;

    /* 行0 (page 0-1): 模式: 自动/手动 */
    mode_str = (g_sensor.run_mode == MODE_AUTO) ? "自动" : "手动";
    oled_draw_mix_line(0, "模式", mode_str);

    /* 行1 (page 2-3): 喂食倒计时 */
    if (g_sensor.feed_countdown > 0) {
        uint32_t sec = g_sensor.feed_countdown;
        rt_snprintf(buf, sizeof(buf), "%02d:%02d",
                    (int)(sec / 60), (int)(sec % 60));
    } else {
        rt_snprintf(buf, sizeof(buf), "--:--");
    }
    oled_draw_mix_line(2, "喂食", buf);

    /* 行2 (page 4-5): WiFi 状态 */
    wifi_str = "W-";
    /* WiFi 状态由 app_wifi 模块更新 g_sensor, 这里简化 */
    oled_draw_mix_line(4, "WiFi", wifi_str);

    /* 行3 (page 6-7): 继电器总览 */
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

/* ===================== 阈值设置页面 ===================== */

/* 阈值项名称 (UTF-8), 共 7 项 */
static const char *threshold_names[] = {
    "温下限", "温上限", "空气值",
    "PH下限", "PH上限", "水位低", "水位高"
};
#define THRESHOLD_ITEMS  7

static void show_threshold_page_0(void)
{
    char buf[24];

    /* 行0: 标题 */
    oled_draw_mix_line(0, "阈值", "Set");

    /* 行1: 温度下限 */
    rt_snprintf(buf, sizeof(buf), "%.1fC%s",
                g_threshold.temp_lower,
                (sub_page == 0 && edit_cursor == 0) ? "<<" : "");
    oled_draw_mix_line(2, threshold_names[0], buf);

    /* 行2: 温度上限 */
    rt_snprintf(buf, sizeof(buf), "%.1fC%s",
                g_threshold.temp_upper,
                (sub_page == 0 && edit_cursor == 1) ? "<<" : "");
    oled_draw_mix_line(4, threshold_names[1], buf);

    /* 行3: 空气阈值 */
    rt_snprintf(buf, sizeof(buf), "%d%s",
                g_threshold.air_quality_max,
                (sub_page == 0 && edit_cursor == 2) ? "<<" : "");
    oled_draw_mix_line(6, threshold_names[2], buf);
}

static void show_threshold_page_1(void)
{
    char buf[24];

    /* 行0: PH下限 */
    rt_snprintf(buf, sizeof(buf), "%.1f%s",
                g_threshold.ph_lower,
                (edit_cursor == 3) ? "<<" : "");
    oled_draw_mix_line(0, threshold_names[3], buf);

    /* 行1: PH上限 */
    rt_snprintf(buf, sizeof(buf), "%.1f%s",
                g_threshold.ph_upper,
                (edit_cursor == 4) ? "<<" : "");
    oled_draw_mix_line(2, threshold_names[4], buf);

    /* 行2: 水位下限 */
    rt_snprintf(buf, sizeof(buf), "%%%d%s",
                g_threshold.water_level_min,
                (edit_cursor == 5) ? "<<" : "");
    oled_draw_mix_line(4, threshold_names[5], buf);

    /* 行3: 水位上限 */
    rt_snprintf(buf, sizeof(buf), "%%%d%s",
                g_threshold.water_level_max,
                (edit_cursor == 6) ? "<<" : "");
    oled_draw_mix_line(6, threshold_names[6], buf);
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

/* ===================== 手动控制页面 ===================== */

/* 手动控制项 */
static const char *manual_names[] = {
    "加热", "加水", "排水", "增氧", "报警", "喂食"
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
    uint8_t start_line = 0;
    uint8_t line;

    rt_mutex_take(&mutex_oled, RT_WAITING_FOREVER);
    oled_clear();

    /* 4 行汉字, 每行显示一个控制项 */
    for (line = 0; line < 4; line++) {
        i = start_line + line;
        if (i >= MANUAL_ITEMS) break;

        uint8_t is_cursor = (i == edit_cursor) ? 1 : 0;
        const char *state = (*manual_status[i]) ? "ON" : "OFF";

        if (is_cursor)
            rt_snprintf(buf, sizeof(buf), "%s>%s", state, "K3");
        else
            rt_snprintf(buf, sizeof(buf), "%s", state);

        oled_draw_mix_line(line * 2, manual_names[i], buf);
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
        return 0;  /* 手动页无子页, 按键无效 */

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
        return 0;  /* 主页面按 KEY3 无效 */

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
        return 0;  /* 主页/手动页按 KEY4 无效 */

    case 5: /* KEY5: 切换编辑项 (仅阈值页和手动页) */
        if (main_page == 1) {
            edit_cursor++;
            if (edit_cursor >= THRESHOLD_ITEMS) edit_cursor = 0;
            return 1;
        } else if (main_page == 2) {
            edit_cursor++;
            if (edit_cursor >= MANUAL_ITEMS) edit_cursor = 0;
            return 1;
        }
        return 0;  /* 主页面按 KEY5 无效 */

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
static rt_uint8_t display_stack[2048];

static void display_thread_entry(void *param)
{
    oled_init();
    rt_thread_mdelay(500);

    /* 首次显示 */
    refresh_display();

    while (1) {
        /* 等待按键事件 (最多等 1 秒, 超时则刷新数据) */
        rt_err_t result = rt_sem_take(&sem_key, rt_tick_from_millisecond(1000));

        int need_refresh = 0;

        if (result == RT_EOK) {
            /* 有按键事件, 判断是否需要刷新 */
            uint8_t key = g_key_event;
            g_key_event = 0;
            need_refresh = handle_key_event(key);
        } else {
            /* 超时: 定时刷新传感器数据 */
            need_refresh = 1;
        }

        /* 只有状态变更或定时超时才刷新, 无效按键不刷屏 */
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
