#include "app_data.h"
#include <string.h>

sensor_data_t    g_sensor;
device_status_t  g_status;
threshold_set_t  g_threshold;

struct rt_mutex      mutex_data;
struct rt_mutex      mutex_oled;
struct rt_semaphore  sem_key;
struct rt_semaphore  sem_wifi_rx;
struct rt_mailbox    mb_alarm;
static rt_ubase_t    mb_pool[8];

void app_data_init(void)
{
    memset(&g_sensor, 0, sizeof(g_sensor));
    memset(&g_status, 0, sizeof(g_status));
    memset(&g_threshold, 0, sizeof(g_threshold));

    g_sensor.run_mode = MODE_AUTO;
    g_status.alarm_enable = 1;
    g_threshold.temp_lower = 20.0f;
    g_threshold.temp_upper = 30.0f;
    g_threshold.air_quality_max = 500;
    g_threshold.ph_lower = 6.5f;
    g_threshold.ph_upper = 8.5f;
    g_threshold.water_level_min = 40;
    g_threshold.water_level_max = 80;
    g_threshold.feed_interval = 43200;

    rt_mutex_init(&mutex_data, "m_data", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mutex_oled, "m_oled", RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_key, "s_key", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_wifi_rx, "s_wifi", 0, RT_IPC_FLAG_FIFO);
    rt_mb_init(&mb_alarm, "mb_alrm", mb_pool, 8, RT_IPC_FLAG_FIFO);
}
