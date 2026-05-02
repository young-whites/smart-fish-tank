#include "bsp_led.h"   



static void                         LED_GPIO_Config                  ( void );



static void LED_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* LED (PB9, 与蜂鸣器共用) */
	RCC_APB2PeriphClockCmd(macLED_1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macLED_1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macLED_1_GPIO_PORT, &GPIO_InitStructure);
}




#define		LED_MAX				(4)					
//----------------------------------------------------------------------------
#define		LED_Name_First		(0x01)				
//----------------------------------------------------------------------------
#define		OUT_CYCLE			(100)				
#define		GRAD_DEC			(5)					
#define		GRAD_INC			(5)					
#define		GRAD_OFF_TIME		(50)				
#define		GRAD_ON_TIME		(5)					
//----------------------------------------------------------------------------
static	int8_t	_ledOutStt[LED_MAX]={0};			
static	int8_t	_ledOutClk[LED_MAX]={0};			
static	int8_t	_ledOutDty[LED_MAX]={0};			
//----------------------------------------------------------------------------
static	int8_t	_ledGrad[LED_MAX]={0};				
static	int8_t	_ledGradDir[LED_MAX]={0};			
static	int8_t	_ledGradCnt[LED_MAX]={0};			
//----------------------------------------------------------------------------
//static	int8_t	_ledBlnkLag[LED_MAX]={0};		
static	int8_t	_ledBlnkClk[LED_MAX]={0};			
static	int8_t	_ledBlnkCyc[LED_MAX]={0};			
static	int8_t	_ledBlnkDty[LED_MAX]={0};			
static	int8_t	_ledBlnkCnt[LED_MAX]={0};			
static	int8_t	_ledBlnkCry[LED_MAX]={0};		
static	int8_t	_ledBlnkMut[LED_MAX]={0};		
static	int8_t	_ledBlnkCct[LED_MAX]={0};		
static	int8_t	_ledBlnkRep[LED_MAX]={0};			
//----------------------------------------------------------------------------
static	int16_t	msCnt=0;							
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
	if(ledState) macLED_1_ON();
	else         macLED_1_OFF();
}


int8_t LED_GetNumber(void)
{
	return LED_NUM;
}









