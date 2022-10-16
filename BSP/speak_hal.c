#include "speak_hal.h"
#define sda_H        HAL_GPIO_WritePin(SPEAK_DATA_PORT, SPEAK_DATA_PIN, GPIO_PIN_SET);
#define sda_L        HAL_GPIO_WritePin(SPEAK_DATA_PORT, SPEAK_DATA_PIN, GPIO_PIN_RESET);

uint8_t read_music_pin()
{
	
	  return  HAL_GPIO_ReadPin(SPEAK_BUSY_PORT, SPEAK_BUSY_PIN);
}
void delay_2us(uint32_t i)
{
	
    uint32_t temp;
    SysTick->LOAD=9*i;         //设置重装数值, 72MHZ时
    SysTick->CTRL=0X01;         //使能，减到零是无动作，采用外部时钟源
    SysTick->VAL=0;                //清零计数器
    do
    {
        temp=SysTick->CTRL;           //读取当前倒计数值
    }
    while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
    SysTick->CTRL=0;    //关闭计数器
    SysTick->VAL=0;        //清空计数器
}



void Line_1A_WT588S(uint8_t DDATA)
{
	
 HAL_GPIO_WritePin(SPEAK_DATA_PORT,  SPEAK_DATA_PIN, GPIO_PIN_SET);  //
	 HAL_GPIO_WritePin(BELL_EN_PORT,  BELL_EN_PIN, GPIO_PIN_SET);  //
uint8_t S_DATA,j;
uint8_t B_DATA; 
S_DATA = DDATA;
  sda_L;
HAL_Delay(5); //延时 5ms 
B_DATA = S_DATA&0X01;
for(j=0;j<8;j++)
{ 
	if(B_DATA == 1)
		{
			sda_H
			delay_2us(60); //延时 600us 
			sda_L
 delay_2us(20); //延时 200us 
			}else { 
				sda_H
				delay_2us(20); //延时 200us 
				sda_L
				delay_2us(60); //延时 600us 
				}
S_DATA = S_DATA>>1;
B_DATA = S_DATA&0X01; 
}
sda_H
}


uint8_t play_music_com(uint8_t addr)
{
	          uint8_t value=0;
	     static uint8_t start_music=0;
	  switch(start_music)
		{
			case 0 :  if(read_music_pin()==1)
		  	{
			start_music=1;
				value=0;
			  } 
		  break;
			case 1 :
				   Line_1A_WT588S(addr) ;
		
			      start_music=2;
			     	value=0;
			break;
			case 2 :   if(read_music_pin()==0)
			{
			
				 start_music=3;
					value=0;
			}break;
			case 3 : if(read_music_pin()==1)
			{
		
       start_music=0;
				value=1;
			}
				     
			break;
		}
    return	value;
	
}



