#include "bsp_led.h"   



static void                         LED_GPIO_Config                  ( void );



 /**
  * @brief  Configure LED GPIO pins
  * @param  None
  * @retval None
  */
static void LED_GPIO_Config ( void )
{		
	/* Define a GPIO_InitTypeDef type structure */
	GPIO_InitTypeDef GPIO_InitStructure;


	/* Configure LED1 PB9 */
	RCC_APB2PeriphClockCmd (macLED_1_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macLED_1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init (macLED_1_GPIO_PORT, & GPIO_InitStructure );
}






/*********************************************END OF FILE**********************/










#define		LED_MAX				(1)					// LED max count
//----------------------------------------------------------------------------
#define		LED_Name_First		(0x01)				// First LED enum value in bsp_led.h should match this, subsequent LED enum values increment from here
//----------------------------------------------------------------------------
#define		OUT_CYCLE			(100)				// LED output cycle
#define		GRAD_DEC			(5)					// LED dim speed: duty decrement timing cycles
#define		GRAD_INC			(5)					// LED brighten speed: duty increment timing cycles
#define		GRAD_OFF_TIME		(50)				// LED full-off pause time (unit ms, should be integer multiple of GRAD_DEC)
#define		GRAD_ON_TIME		(5)					// LED full-on pause time (unit ms, should be integer multiple of GRAD_INC)
//----------------------------------------------------------------------------
static	int8_t	_ledOutStt[LED_MAX]={0};			// LED output state
static	int8_t	_ledOutClk[LED_MAX]={0};			// LED output clock
static	int8_t	_ledOutDty[LED_MAX]={0};			// LED output duty (0~OUT_CYCLE, higher value = brighter LED)
//----------------------------------------------------------------------------
static	int8_t	_ledGrad[LED_MAX]={0};				// LED gradient enable
static	int8_t	_ledGradDir[LED_MAX]={0};			// LED gradient direction
static	int8_t	_ledGradCnt[LED_MAX]={0};			// LED full-off/full-on pause counter
//----------------------------------------------------------------------------
static	int8_t	_ledBlnkClk[LED_MAX]={0};			// LED blink clock (unit 0.1s)
static	int8_t	_ledBlnkCyc[LED_MAX]={0};			// LED blink cycle (one on + off time, unit 0.1s)
static	int8_t	_ledBlnkDty[LED_MAX]={0};			// LED blink duty (one on occupy time, unit 0.1s)
static	int8_t	_ledBlnkCnt[LED_MAX]={0};			// LED blink counter
static	int8_t	_ledBlnkCry[LED_MAX]={0};			// LED blink count (0 means infinite blink)
static	int8_t	_ledBlnkMut[LED_MAX]={0};			// LED pause blink count (0 means no pause)
static	int8_t	_ledBlnkCct[LED_MAX]={0};			// LED repeat counter
static	int8_t	_ledBlnkRep[LED_MAX]={0};			// LED repeat blink count (0 means no repeat, >=100 means infinite repeat)
//----------------------------------------------------------------------------
static	int16_t	msCnt=0;							// Output counter
//----------------------------------------------------------------------------

static void _Off(int8_t ledName)
{
	_ledOutDty[ledName-LED_Name_First]=0;
}

static void _On(int8_t ledName)
{
	_ledOutDty[ledName-LED_Name_First]=OUT_CYCLE;
}

void LED_Off(int8_t ledName)
{
	int8_t	i = ledName-LED_Name_First;
	_ledGrad[i]=0;
	_ledBlnkCry[i]=0;
	_Off(ledName);
	LED_Out(ledName,0);
}

void LED_On(int8_t ledName)
{
	int8_t	i = ledName-LED_Name_First;
	_ledGrad[i]=0;
	_ledBlnkCry[i]=0;
	_On(ledName);
	LED_Out(ledName,1);
}

void LED_Toggle(int8_t ledName)
{
	int8_t	i = ledName-LED_Name_First;
	if(_ledOutStt[i])	LED_Off(ledName);
	else				LED_On(ledName);
}

void LED_Grad(int8_t ledName)
{
	int8_t	i = ledName-LED_Name_First;
	_ledOutDty[i]=0;
	_ledGradDir[i]=0;
	_ledGradCnt[i]=0;
	_ledGrad[i]=1;
}

void LED_BlinkSetCycleDuty(int8_t ledName, int8_t Cycle, int8_t Duty)
{
	int8_t	i = ledName-LED_Name_First;
	_ledBlnkCyc[i]=Cycle;
	_ledBlnkDty[i]=Duty;
	_ledBlnkClk[i]=0;
	msCnt=0;
}

void LED_Blink(int8_t ledName, int8_t cry, int8_t mute, int8_t repeat)
{
	int8_t	i = ledName-LED_Name_First;
	_ledBlnkCry[i]=cry;
	_ledBlnkMut[i]=mute;
	_ledBlnkRep[i]=repeat;
	_ledBlnkCnt[i]=0;
	_ledBlnkCct[i]=0;
}

void LED_Fancy(int8_t mode)
{
	switch(mode){
	case 1:
		break;
	case 2:
		break;
	}
}

void LED_DrvScan(void)
{
	int8_t	i,num;
	
	if(++msCnt>=60000)	msCnt=0;
	
	num=LED_GetNumber();
	if(num>LED_MAX)	num=LED_MAX;
	for(i=0;i<num;i++){
		if(_ledBlnkCry[i]){
			if((msCnt%OUT_CYCLE)==0){
				if(++_ledBlnkClk[i] >= _ledBlnkCyc[i]){
					_ledBlnkClk[i]=0;
					if(++_ledBlnkCnt[i] >= (_ledBlnkCry[i]+_ledBlnkMut[i])){
						_ledBlnkCnt[i]=0;
						if(++_ledBlnkCct[i] >= _ledBlnkRep[i]){
							_ledBlnkCct[i]=0;
							if(_ledBlnkRep[i] < 100)	_ledBlnkCry[i]=0;
						}
					}
				}else if(_ledBlnkClk[i] >= _ledBlnkDty[i]){
					_Off(LED_Name_First+i);
				}else if(_ledBlnkCnt[i] < _ledBlnkCry[i]){
					_On(LED_Name_First+i);
				}
			}
		}
		
		if(_ledGrad[i]){
			if(_ledGradDir[i]){
				if((msCnt%GRAD_DEC)==0){
					if(_ledOutDty[i]>0){
						_ledOutDty[i]-=1;
						_ledGradCnt[i]=0;
					}
					else {
						_ledGradCnt[i]+=1;
						if(_ledGradCnt[i]>=(GRAD_OFF_TIME/GRAD_DEC)) _ledGradDir[i]=0;
					}
				}
			}else{
				if((msCnt%GRAD_INC)==0){
					if(_ledOutDty[i]<OUT_CYCLE){
						_ledOutDty[i]+=1;
						_ledGradCnt[i]=0;
					}
					else {
						_ledGradCnt[i]+=1;
						if(_ledGradCnt[i]>=(GRAD_ON_TIME/GRAD_INC))	_ledGradDir[i]=1;
					}
				}
			}
		}
		
		_ledOutClk[i]+=1;
		if(_ledOutClk[i]>=OUT_CYCLE)	_ledOutClk[i]=0;
		if(_ledOutClk[i]>=_ledOutDty[i]){
			_ledOutStt[i]=0;
		}else{
			_ledOutStt[i]=1;
		}
		LED_Out(LED_Name_First+i,_ledOutStt[i]);
	}
}



 /***************************************
  * @brief  LED initialization function
  * @param  None
  * @retval None
  ***************************************/
void LED_Init(void)
{
	int8_t	i,num;
	
	LED_GPIO_Config ();	
	
	num=LED_GetNumber();
	if(num>LED_MAX)	num=LED_MAX;
	for(i=0;i<num;i++){
		LED_BlinkSetCycleDuty(LED_Name_First+i, 3, 2);
	}
}



void LED_Out(int8_t ledName, int8_t ledState)
{
	switch(ledName)
	{
		case LED_Name_1:
			if(ledState)	macLED_1_ON();
			else			macLED_1_OFF();
			break;
	}
}

int8_t LED_GetNumber(void)
{
	return LED_NUM;
}

