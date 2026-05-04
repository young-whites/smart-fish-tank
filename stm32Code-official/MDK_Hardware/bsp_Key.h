#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "sys.h"



/****************************** KEY1 = PA8 Config ***************************************/
#define             macKEY_A_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_A_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macKEY_A_GPIO_PORT                       GPIOA
#define             macKEY_A_GPIO_PIN                        GPIO_Pin_8

/****************************** KEY2 = PB3 Config ***************************************/
#define             macKEY_B_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_B_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_B_GPIO_PORT                       GPIOB
#define             macKEY_B_GPIO_PIN                        GPIO_Pin_3

/****************************** KEY3 = PB4 Config ***************************************/
#define             macKEY_C_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_C_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_C_GPIO_PORT                       GPIOB
#define             macKEY_C_GPIO_PIN                        GPIO_Pin_4

/****************************** KEY4 = PB8 Config ***************************************/
#define             macKEY_D_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_D_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_D_GPIO_PORT                       GPIOB
#define             macKEY_D_GPIO_PIN                        GPIO_Pin_8

/****************************** KEY5 = PB1 Config ***************************************/
#define             macKEY_E_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_E_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_E_GPIO_PORT                       GPIOB
#define             macKEY_E_GPIO_PIN                        GPIO_Pin_1




#define KEYA        GPIO_ReadInputDataBit(macKEY_A_GPIO_PORT, macKEY_A_GPIO_PIN)     /* KEY1 Pin PA8 */
#define KEYB        GPIO_ReadInputDataBit(macKEY_B_GPIO_PORT, macKEY_B_GPIO_PIN)     /* KEY2 Pin PB3 */
#define KEYC        GPIO_ReadInputDataBit(macKEY_C_GPIO_PORT, macKEY_C_GPIO_PIN)     /* KEY3 Pin PB4 */
#define KEYD        GPIO_ReadInputDataBit(macKEY_D_GPIO_PORT, macKEY_D_GPIO_PIN)     /* KEY4 Pin PB8 */
#define KEYE        GPIO_ReadInputDataBit(macKEY_E_GPIO_PORT, macKEY_E_GPIO_PIN)     /* KEY5 Pin PB1 */

#define		KEY_NUM			(5)		// Key count

typedef	enum	                    //  Key value
{					
	KEY_A	= (0x01),		        //  KEY1 - PA8 - Page switch
    KEY_B,                          //  KEY2 - PB3 - Threshold param select
    KEY_C,                          //  KEY3 - PB4 - Manual/mode switch
    KEY_D,                          //  KEY4 - PB8 - Decrease
    KEY_E,                          //  KEY5 - PB1 - Feed trigger
}KEY_Val_TypeDef;
//------------------------------------------------------------------------

typedef	enum {					    // Key event type
    KEY_Evt_Press   = (0x80),		// Press
    KEY_Evt_Release = (0x40),		// Release
    KEY_Evt_Long2S  = (0x20),		// Long press 2s
    KEY_Evt_Long4S  = (0x10),		// Long press 4s
}KEY_Evt_TypeDef;
//------------------------------------------------------------------------

// Threshold param count (temp lower, temp upper, air upper, water level lower, water level upper, PH lower, PH upper = 7)
#define THRESHOLD_PARAM_NUM  3

// Threshold adjustment step
#define THRESHOLD_TEMP_STEP    1.0f    // Temperature step 1
#define THRESHOLD_PH_STEP      0.1f    // PH step 0.1
#define THRESHOLD_AIR_STEP     10      // Air quality step 10
#define THRESHOLD_WL_STEP      1       // Water level step 1%

void KEY_GPIO_Config(void);
void KEY_Scan(void);
void KEY_DrvScan(void);
uint8_t KEY_Read(void);
void KEY_Write(uint8_t value);


#endif /*__BSP_KEY_H*/
