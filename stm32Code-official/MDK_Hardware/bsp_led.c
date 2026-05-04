#include "bsp_led.h"   



static void                         LED_GPIO_Config                  ( void );



 /**
  * @brief  配置LED用到的GPIO引脚
  * @param  无
  * @retval 无
  */
static void LED_GPIO_Config ( void )
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* 配置LED1 PB9 */
	RCC_APB2PeriphClockCmd (macLED_1_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macLED_1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init (macLED_1_GPIO_PORT, & GPIO_InitStructure );
}






/*********************************************END OF FILE**********************/










#define		LED_MAX				(1)					// LED最大数量
//----------------------------------------------------------------------------
#define		LED_Name_First		(0x01)				// bsp_led.h中的第一个LED枚举值应与此值一致，后续LED枚举值应在此基础上依次递增(枚举定义)
//----------------------------------------------------------------------------
#define		OUT_CYCLE			(100)				// LED输出周期
#define		GRAD_DEC			(5)					// LED渐暗速度：占空比递减的计时周期数
#define		GRAD_INC			(5)					// LED渐亮速度：占空比递增的计时周期数
#define		GRAD_OFF_TIME		(50)				// LED全灭后停顿时间（单位毫秒，应为GRAD_DEC的整数倍）
#define		GRAD_ON_TIME		(5)					// LED全亮后停顿时间（单位毫秒，应为GRAD_INC的整数倍）
//----------------------------------------------------------------------------
static	int8_t	_ledOutStt[LED_MAX]={0};			// LED输出状态
static	int8_t	_ledOutClk[LED_MAX]={0};			// LED输出时钟
static	int8_t	_ledOutDty[LED_MAX]={0};			// LED输出占空（0~OUT_CYCLE，数值越大LED越亮）
//----------------------------------------------------------------------------
static	int8_t	_ledGrad[LED_MAX]={0};				// LED渐变开关
static	int8_t	_ledGradDir[LED_MAX]={0};			// LED渐变方向
static	int8_t	_ledGradCnt[LED_MAX]={0};			// LED全灭/全亮停顿计数
//----------------------------------------------------------------------------
static	int8_t	_ledBlnkClk[LED_MAX]={0};			// LED闪灯时钟（量纲0.1s）
static	int8_t	_ledBlnkCyc[LED_MAX]={0};			// LED闪烁周期（一个亮灯+灭灯时间，量纲0.1s）
static	int8_t	_ledBlnkDty[LED_MAX]={0};			// LED闪烁占空（一个亮灯占用时间，量纲0.1s）
static	int8_t	_ledBlnkCnt[LED_MAX]={0};			// LED闪烁计数
static	int8_t	_ledBlnkCry[LED_MAX]={0};			// LED闪烁次数（0表示无限闪烁）
static	int8_t	_ledBlnkMut[LED_MAX]={0};			// LED停闪次数（0表示不停歇）
static	int8_t	_ledBlnkCct[LED_MAX]={0};			// LED重复次数
static	int8_t	_ledBlnkRep[LED_MAX]={0};			// LED重复闪烁次数（0表示不重复，100以上表示无限重复）
//----------------------------------------------------------------------------
static	int16_t	msCnt=0;							// 输出计数器
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
  * @brief  LED 初始化函数
  * @param  无
  * @retval 无
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


