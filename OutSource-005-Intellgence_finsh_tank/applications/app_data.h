#ifndef APP_DATA_H
#define APP_DATA_H

#include <rtthread.h>
#include <stdint.h>

/* ========== 工作模式 ========== */
enum WorkMode { MODE_AUTO = 0, MODE_MANUAL = 1 };

/* ========== 传感器数据 ========== */
typedef struct {
    float    water_temp;
    float    ph_value;
    uint8_t  water_level;
    uint16_t air_quality;
    uint8_t  run_mode;
    uint32_t feed_countdown;
} sensor_data_t;

/* ========== 设备状态 ========== */
typedef struct {
    uint8_t relay_heat;
    uint8_t relay_fill;
    uint8_t relay_drain;
    uint8_t relay_oxygen;
    uint8_t alarm_enable;
    uint8_t feeding;
} device_status_t;

/* ========== 阈值设置 ========== */
typedef struct {
    float    temp_lower;
    float    temp_upper;
    uint16_t air_quality_max;
    float    ph_lower;
    float    ph_upper;
    uint8_t  water_level_min;
    uint8_t  water_level_max;
    uint32_t feed_interval;
} threshold_set_t;

/* ========== 报警类型 ========== */
enum AlarmType {
    ALARM_TEMP_LOW    = 0x01,
    ALARM_TEMP_HIGH   = 0x02,
    ALARM_WATER_LOW   = 0x03,
    ALARM_WATER_HIGH  = 0x04,
    ALARM_AIR_BAD     = 0x05,
    ALARM_PH_LOW      = 0x06,
    ALARM_PH_HIGH     = 0x07,
};

/* ========== 全局共享数据 ========== */
extern sensor_data_t    g_sensor;
extern device_status_t  g_status;
extern threshold_set_t  g_threshold;

/* ========== IPC 对象 ========== */
extern struct rt_mutex      mutex_data;
extern struct rt_mutex      mutex_oled;
extern struct rt_semaphore  sem_key;
extern struct rt_semaphore  sem_wifi_rx;
extern struct rt_mailbox    mb_alarm;

/* ========== ADC 通道 ========== */
#define ADC_AIR_CHANNEL   4
#define ADC_WATER_CHANNEL 1
#define ADC_PH_CHANNEL    0

/* ========== 函数声明 ========== */
void app_data_init(void);

#endif
