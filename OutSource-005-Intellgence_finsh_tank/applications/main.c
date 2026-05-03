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
#include "bsp_sys.h"


int main(void)
{

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();


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
