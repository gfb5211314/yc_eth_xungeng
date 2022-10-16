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
#define PS_Sta     HAL_GPIO_ReadPin(Touch_Wkup_GPIO_Port, Touch_Wkup_Pin)//��ָ��ģ��״̬����
//#define PS_Sta     1  //���ԣ�����ע�͵�
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
extern u32 AS608Addr;//ģ���ַ

typedef struct  
{
	u16 pageID;//ָ��ID
	u16 mathscore;//ƥ��÷�
}SearchResult;

typedef struct
{
	u16 PS_max;//ָ���������
	u8  PS_level;//��ȫ�ȼ�
	u32 PS_addr;
	u8  PS_size;//ͨѶ���ݰ���С
	u8  PS_N;//�����ʻ���N
}SysPara;

void PS_StaGPIO_Init(void);//��ʼ��PA6��״̬����
	
u8 PS_GetImage(void); //¼��ͼ�� 
 
u8 PS_GenChar(u8 BufferID);//�������� 

u8 PS_Match(void);//��ȷ�ȶ���öָ������ 

u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//����ָ�� 
 
u8 PS_RegModel(void);//�ϲ�����������ģ�壩 
 
u8 PS_StoreChar(u8 BufferID,u16 PageID);//����ģ�� 

u8 PS_DeletChar(u16 PageID,u16 N);//ɾ��ģ�� 

u8 PS_Empty(void);//���ָ�ƿ� 

u8 PS_WriteReg(u8 RegNum,u8 DATA);//дϵͳ�Ĵ��� 
 
u8 PS_ReadSysPara(SysPara *p); //��ϵͳ�������� 

u8 PS_SetAddr(u32 addr);  //����ģ���ַ 

u8 PS_WriteNotepad(u8 NotePageNum,u8 *content);//д���±� 

u8 PS_ReadNotepad(u8 NotePageNum,u8 *note);//������ 

u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//�������� 
  
u8 PS_ValidTempleteNum(u16 *ValidN);//����Чģ����� 

u8 PS_HandShake(u32 *PS_Addr); //��AS608ģ������

const char *EnsureMessage(u8 ensure);//ȷ���������Ϣ����

#endif

