#include "bsp_beep.h" 


static void     Beep_GPIO_Config                        ( void );



void Beep_Init ( void )
{		
 	Beep_GPIO_Config ();
  	BEEP_SetCycleDuty(200,100);
}


static void Beep_GPIO_Config ( void )
{		
	/* Define a GPIO_InitTypeDef type structure */
	GPIO_InitTypeDef GPIO_InitStructure;

	
	/* Enable GPIOB clock */
	macBEEP_GPIO_APBxClock_FUN  ( macBEEP_GPIO_CLK, ENABLE ); 

	/* Select GPIO pin to control */															   
	GPIO_InitStructure.GPIO_Pin = macBEEP_PIN;	

	/* Set pin mode to push-pull output */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/* Set pin speed to 50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/* Initialize GPIOB15 with library function */
	GPIO_Init ( macBEEP_PORT, & GPIO_InitStructure );

}

/* END BEEP Config******************************************************************/



static	int16_t	beepClk=0;							// Buzzer scan clock (unit: ms)
static	int16_t	beepCyc=0;							// Buzzer scan cycle (one ring + one mute time, unit: ms)
static	int16_t	beepDty=0;							// Buzzer scan duty (one ring occupy time, unit: ms)
static	int8_t	beepCnt=0;							// Buzzer ring counter
static	int8_t	beepCry=0;							// Buzzer ring count (0 means not ringing)
static	int8_t	beepMut=0;							// Buzzer mute count (0 means no mute)
static	int8_t	beepCct=0;							// Buzzer repeat counter
static	int8_t	beepRep=0;							// Buzzer repeat count (0 means no repeat, >=100 means infinite repeat)



/*****************************************************************************
* Name:     Buzzer off
*****************************************************************************/
void BEEP_Off(void)
{
	beepCry=0;										// Stop buzzer ringing
	macBEEP_OFF();
}



/*****************************************************************************
* Name:     Buzzer on
*****************************************************************************/
void BEEP_On(void)
{
	beepCry=0;										// Stop buzzer ringing
	macBEEP_ON();
}




/*****************************************************************************
* Name:     Set buzzer cycle and duty
* Params:   Cycle - Cycle (one ring + one mute time, unit: ms)
*           Duty  - Duty (one ring occupy time, unit: ms)
*****************************************************************************/
void BEEP_SetCycleDuty(int16_t Cycle, int16_t Duty)
{
	beepCyc=Cycle;
	beepDty=Duty;
	beepClk=0;
}



/*****************************************************************************
* Name:     Set buzzer running, specified ring count
* Params:   cry    - Buzzer ring count (0 means not ringing)
*           mute   - Buzzer mute count (0 means no mute)
*           repeat - Buzzer repeat count (0 means no repeat, >=100 means infinite repeat)
*****************************************************************************/
void BEEP_Blink(int8_t cry, int8_t mute, int8_t repeat)
{
	beepCry=cry;
	beepMut=mute;
	beepRep=repeat;
	beepCnt=0;
	beepCct=0;
}





/*****************************************************************************
* Name:     Buzzer scan
* Note:     Scan period: 1ms
*****************************************************************************/
void BEEP_DrvScan(void)
{
	if(beepCry){									// Need to ring
		if(++beepClk >= beepCyc){ 					// Sub-cycle reached
			beepClk=0;
			if(++beepCnt >= (beepCry+beepMut)){		// Ring cycle reached
				beepCnt=0;
				if(++beepCct >= beepRep){			// Repeat count reached
					beepCct=0;
					if(beepRep < 100)	beepCry=0;	// Repeat done, stop ringing
				}
			}
		}else if(beepClk >= beepDty){				// Sub-cycle: stop
			macBEEP_OFF();
		}else if(beepCnt < beepCry){				// Ring period: active
			macBEEP_ON();
		}
	}
}


