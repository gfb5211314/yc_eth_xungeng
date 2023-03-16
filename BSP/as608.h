#ifndef __AS608_H
#define __AS608_H
#include <stdio.h>
#include "stm32l0xx_hal.h"
#include "main.h"
#include "usart.h"

//#define  AS608_TX  
//#define  AS608_RX

#define  AS608_USART     hlpuart1


typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned      int u32;

//#define zhiwen_jianche_GPIO_Port

#define PS_Sta     HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)//��ָ��ģ��״̬����
//#define PS_Sta     1  //���ԣ�����ע�͵�
#define CharBuffer1 0x01
#define CharBuffer2 0x02

#define  data_len  1024

typedef struct
{
    uint8_t  RX_flag;           //IDLE receive flag
    uint16_t RX_Size;           //receive length
	  uint16_t count;
    uint8_t  RX_pData[data_len];
 //   uint8_t  TX_pData[data_len]; //ʡ�ڴ�
}AS608_USART_TYPE;
extern AS608_USART_TYPE  as608_usart_st;
void  AS608_DMA_START(void);
extern u32 AS608Addr;//ģ���ַ

typedef struct  
{
	u16 pageID;//ָ��ID
	u16 mathscore;//ƥ��÷�
}SearchResult;

#pragma pack(8)

typedef struct  as608_data_head_s
{
	u16 u16Header;//����ͷ
	u32 u32Addr;//��ַ
	u8  u8Flag;//��ʶ
	u16 u16Length;//���ݳ���
}AS608_DATA_HEAD_S;

typedef struct  as608_data_s
{
    AS608_DATA_HEAD_S stHeader;
	u8  *pData;
    u16 u16CheckSum;//У��
}AS608_DATA_S;

#pragma pack()


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

