#include "app_data.h"
#include "app_wifi.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

volatile uint8_t g_wifi_connected = 0;
static UART_HandleTypeDef huart2;

/* 协议帧格式: 0xAA | CMD | LEN | DATA | SUM | 0x55 */
static uint8_t frame_buf[256];

static void build_frame(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    uint8_t sum = cmd + len;
    frame_buf[0] = 0xAA;
    frame_buf[1] = cmd;
    frame_buf[2] = len;
    for(uint8_t i = 0; i < len; i++) {
        frame_buf[3 + i] = data[i];
        sum += data[i];
    }
    frame_buf[3 + len] = sum;
    frame_buf[4 + len] = 0x55;
}

static void send_sensor_data(void)
{
    uint8_t payload[14];
    memcpy(&payload[0], &g_sensor.water_temp, 4);
    memcpy(&payload[4], &g_sensor.ph_value, 4);
    payload[8] = g_sensor.water_level;
    payload[9] = (g_sensor.air_quality >> 8) & 0xFF;
    payload[10] = g_sensor.air_quality & 0xFF;
    payload[11] = g_sensor.run_mode;
    payload[12] = (g_sensor.feed_countdown >> 8) & 0xFF;
    payload[13] = g_sensor.feed_countdown & 0xFF;
    build_frame(0x01, payload, 14);
    HAL_UART_Transmit(&huart2, frame_buf, 7, 100);
}

static void send_device_status(void)
{
    uint8_t payload[5] = {0};
    if(g_status.relay_heat)   payload[0] |= 0x01;
    if(g_status.relay_fill)   payload[0] |= 0x02;
    if(g_status.relay_drain)  payload[0] |= 0x04;
    if(g_status.relay_oxygen) payload[0] |= 0x08;
    payload[1] = g_status.alarm_enable;
    payload[2] = g_status.feeding;
    build_frame(0x02, payload, 5);
    HAL_UART_Transmit(&huart2, frame_buf, 7, 100);
}

static uint8_t wifi_try_connect(void)
{
    char cmd[128];
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT\r\n", 4, 100);
    rt_thread_mdelay(500);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CWMODE=1\r\n", 13, 100);
    rt_thread_mdelay(500);
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", "YOUR_SSID", "YOUR_PASSWORD");
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 100);
    rt_thread_mdelay(8000);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CIPMUX=1\r\n", 13, 100);
    rt_thread_mdelay(500);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CIPSERVER=1,8080\r\n", 21, 100);
    rt_thread_mdelay(500);
    return 1;
}

static void wifi_thread_entry(void *param)
{
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_2;
    g.Mode = GPIO_MODE_AF_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &g);

    g.Pin = GPIO_PIN_3;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &g);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart2);

    rt_thread_mdelay(1000);

    if(wifi_try_connect()) {
        g_wifi_connected = 1;
    }

    while(1) {
        if(g_wifi_connected) {
            send_sensor_data();
            rt_thread_mdelay(500);
            send_device_status();
        } else {
            rt_thread_mdelay(5000);
            if(wifi_try_connect()) g_wifi_connected = 1;
        }
        rt_thread_mdelay(500);
    }
}

void app_wifi_init(void)
{
    rt_thread_t tid = rt_thread_create("t_wifi", wifi_thread_entry, RT_NULL, 2048, 17, 10);
    if(tid) rt_thread_startup(tid);
}
