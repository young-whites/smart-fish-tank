#include "ds18b20.h"


/*******************************************************************************
* å‡? æ•? å?         : DS18B20_IO_IN
* å‡½æ•°åŠŸèƒ½		   : DS18B20_IOè¾“å…¥é…ç½®	   
* è¾?    å…?         : æ—?
* è¾?    å‡?         : æ—?
*******************************************************************************/
void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_IO_OUT
* å‡½æ•°åŠŸèƒ½		   : DS18B20_IOè¾“å‡ºé…ç½®	   
* è¾?    å…?         : æ—?
* è¾?    å‡?         : æ—?
*******************************************************************************/
void DS18B20_IO_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Reset
* å‡½æ•°åŠŸèƒ½		   : å¤ä½DS18B20  
* è¾?    å…?         : æ—?
* è¾?    å‡?         : æ—?
*******************************************************************************/
void DS18B20_Reset(void)	   
{                 
	DS18B20_IO_OUT(); //SET PA7 OUTPUT
	DS18B20_DQ_OUT=0; //æ‹‰ä½DQ
	delay_us(750);    //æ‹‰ä½750us
	DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Check
* å‡½æ•°åŠŸèƒ½		   : æ£?æµ‹DS18B20æ˜?å¦å­˜åœ?
* è¾?    å…?         : æ—?
* è¾?    å‡?         : 1:æœ?æ£?æµ‹åˆ°DS18B20çš„å­˜åœ?ï¼?0:å­˜åœ¨
*******************************************************************************/
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();//SET PA7 INPUT	 
	
	while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};
	
	if(retry>=200)
		return 1;
	else 
		retry=0;
	
	while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)
		return 1;	 
	
	return 0;
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Read_Bit
* å‡½æ•°åŠŸèƒ½		   : ä»DS18B20è¯»å–ä¸?ä¸?ä½?
* è¾?    å…?         : æ—?
* è¾?    å‡?         : 1/0
*******************************************************************************/
uint8_t DS18B20_Read_Bit(void) 			 // read one bit
{
	uint8_t data;
	DS18B20_IO_OUT();//SET PA7 OUTPUT
	DS18B20_DQ_OUT=0; 
	delay_us(2);
	DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PA7 INPUT
	delay_us(12);
	
	if(DS18B20_DQ_IN)
		data=1;
	else 
		data=0;	 
	
	delay_us(50);  
	
	return data;
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Read_Byte
* å‡½æ•°åŠŸèƒ½		   : ä»DS18B20è¯»å–ä¸?ä¸?å­—èŠ‚
* è¾?    å…?         : æ—?
* è¾?    å‡?         : ä¸?ä¸?å­—èŠ‚æ•°æ®
*******************************************************************************/
uint8_t DS18B20_Read_Byte(void)    // read one byte
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit();
		dat=(j<<7)|(dat>>1);
	}						    
	return dat;
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Write_Byte
* å‡½æ•°åŠŸèƒ½		   : å†™ä¸€ä¸?å­—èŠ‚åˆ°DS18B20
* è¾?    å…?         : datï¼šè?å†™å…¥çš„å­—èŠ‚
* è¾?    å‡?         : æ—?
*******************************************************************************/
void DS18B20_Write_Byte(uint8_t dat)     
{             
	uint8_t j;
	uint8_t testb;
	DS18B20_IO_OUT();//SET PA7 OUTPUT;
	for (j=1;j<=8;j++) 
	{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) 
		{
			DS18B20_DQ_OUT=0;// Write 1
			delay_us(2);                            
			DS18B20_DQ_OUT=1;
			delay_us(60);             
		}
		else 
		{
			DS18B20_DQ_OUT=0;// Write 0
			delay_us(60);             
			DS18B20_DQ_OUT=1;
			delay_us(2);                          
		}
	}
}

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Start
* å‡½æ•°åŠŸèƒ½		   : å¼?å§‹æ¸©åº¦è½¬æ?
* è¾?    å…?         : æ—?
* è¾?    å‡?         : æ—?
*******************************************************************************/
void DS18B20_Start(void)// ds1820 start convert
{   						               
	DS18B20_Reset();	   
	DS18B20_Check();	 
	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0x44);// convert
} 

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_Init
* å‡½æ•°åŠŸèƒ½		   : åˆå?‹åŒ–DS18B20çš„IOå? DQ åŒæ—¶æ£?æµ‹DSçš„å­˜åœ?
* è¾?    å…?         : æ—?
* è¾?    å‡?         : 1:ä¸å­˜åœ?ï¼?0:å­˜åœ¨
*******************************************************************************/   	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DS18B20_PORT_RCC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
 
 	DS18B20_Reset();
	return DS18B20_Check();
}  

/*******************************************************************************
* å‡? æ•? å?         : DS18B20_GetTemperture
* å‡½æ•°åŠŸèƒ½		   : ä»ds18b20å¾—åˆ°æ¸©åº¦å€?
* è¾?    å…?         : æ—?
* è¾?    å‡?         : æ¸©åº¦æ•°æ®
*******************************************************************************/ 
float DS18B20_GetTemperture(void)
{
	uint16_t temp;
	uint8_t a,b;
	float value;
	
	DS18B20_Start();         // ds1820 start convert
	DS18B20_Reset();
	DS18B20_Check();	 
	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0xbe);// convert	    
	a=DS18B20_Read_Byte(); // LSB   
	b=DS18B20_Read_Byte(); // MSB   
	temp=b;
	temp=(temp<<8)+a;
	
	if((temp&0xf800)==0xf800)
	{
		temp=(~temp)+1;
		value=temp*(-0.0625);
	}
	else
	{
		value=temp*0.0625;	
	}
	return value;    
}



