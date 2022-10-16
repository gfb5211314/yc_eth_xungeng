#ifndef __AS608_H
#define __AS608_H
#include <stdio.h>
#include "stm32L0xx_hal.h"
#include "main.h"
#include "usart.h"
typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned      int u32;

//#define zhiwen_jianche_GPIO_Port

#define AS608_USART   hlpuart1
#define PS_Sta     HAL_GPIO_ReadPin(Touch_Wkup_GPIO_Port, Touch_Wkup_Pin)//读指纹模块状态引脚
//#define PS_Sta     1  //测试，后面注释掉
#define CharBuffer1 0x01
#define CharBuffer2 0x02

#define  data_len  2048

typedef struct
{
    uint8_t  RX_flag;           //IDLE receive flag
    uint16_t RX_Size;           //receive length
	  uint16_t count;
    uint16_t tem_RX_Size;       //receive length
    uint8_t  RX_pData[data_len];
}AS608_USART_TYPE;

   extern   AS608_USART_TYPE         AS608_USART_ST;
void  ETH_DMA_START();
extern u32 AS608Addr;//模块地址

typedef struct  
{
	u16 pageID;//指纹ID
	u16 mathscore;//匹配得分
}SearchResult;

typedef struct
{
	u16 PS_max;//指纹最大容量
	u8  PS_level;//安全等级
	u32 PS_addr;
	u8  PS_size;//通讯数据包大小
	u8  PS_N;//波特率基数N
}SysPara;

void PS_StaGPIO_Init(void);//初始化PA6读状态引脚
	
u8 PS_GetImage(void); //录入图像 
 
u8 PS_GenChar(u8 BufferID);//生成特征 

u8 PS_Match(void);//精确比对两枚指纹特征 

u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//搜索指纹 
 
u8 PS_RegModel(void);//合并特征（生成模板） 
 
u8 PS_StoreChar(u8 BufferID,u16 PageID);//储存模板 

u8 PS_DeletChar(u16 PageID,u16 N);//删除模板 

u8 PS_Empty(void);//清空指纹库 

u8 PS_WriteReg(u8 RegNum,u8 DATA);//写系统寄存器 
 
u8 PS_ReadSysPara(SysPara *p); //读系统基本参数 

u8 PS_SetAddr(u32 addr);  //设置模块地址 

u8 PS_WriteNotepad(u8 NotePageNum,u8 *content);//写记事本 

u8 PS_ReadNotepad(u8 NotePageNum,u8 *note);//读记事 

u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//高速搜索 
  
u8 PS_ValidTempleteNum(u16 *ValidN);//读有效模板个数 

u8 PS_HandShake(u32 *PS_Addr); //与AS608模块握手

const char *EnsureMessage(u8 ensure);//确认码错误信息解析

#endif

