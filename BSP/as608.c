#include "stm32l0xx_hal.h"
#include <string.h>
#include "stdio.h"
#include "as608.h"
#include "main.h"
#include "gpio.h"

u32 AS608Addr = 0XFFFFFFFF; //Ĭ��
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;




AS608_USART_TYPE  as608_usart_st;

static void AS608_DebugPrintf(u8 *pData,u16 u16Size)
{
    for(uint16_t i=0;i<u16Size;i++)
    {
         
    printf("\r\n");
    printf("%03d 0x%02X",i,pData[i]);
    printf("\r\n");
                    
    }
}

//����DMA���տ����ж�
void  AS608_DMA_START()
{
	    __HAL_UART_ENABLE_IT(&AS608_USART, UART_IT_IDLE);
   HAL_UART_Receive_DMA(&AS608_USART,(uint8_t *)as608_usart_st.RX_pData, 50);  //����������

}
//�������տ����ж�
static void  As608_UsartReceive_IDLE()
{
    uint32_t temp=0;
    if((__HAL_UART_GET_FLAG(&AS608_USART,UART_FLAG_IDLE) != RESET))
    {
			  
        __HAL_UART_CLEAR_IDLEFLAG(&AS608_USART);
        HAL_UART_DMAStop(&AS608_USART);
        temp = AS608_USART.hdmarx->Instance->CNDTR;//	
          as608_usart_st.RX_Size = data_len - temp;
        as608_usart_st.RX_flag=1;
       HAL_UART_Receive_DMA(&AS608_USART,(uint8_t *)as608_usart_st.RX_pData, data_len);
    }
}

void LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN LPUART1_IRQn 0 */
    As608_UsartReceive_IDLE();
  /* USER CODE END LPUART1_IRQn 0 */
  HAL_UART_IRQHandler(&hlpuart1);
  /* USER CODE BEGIN LPUART1_IRQn 1 */

  /* USER CODE END LPUART1_IRQn 1 */
}
//��ʼ��PA6Ϊ��������		    
//��������Ӧ״̬(������Ӧʱ����ߵ�ƽ�ź�)
void PS_StaGPIO_Init(void)
{   
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
//  //��ʼ����״̬����GPIOA
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������ģʽ
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO	
}
//���ڷ���һ���ֽ�
static void MYUSART_SendData(u8 data)
{
//	while((USART2->ISR&0X40)==0); 
//  	USART2->RDR = data;
	  HAL_UART_Transmit(&AS608_USART, (uint8_t *)&data, 1, 0xffff);
}
//���Ͱ�ͷ
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//���Ͱ���ʶ,
static void SendFlag(u8 flag)
{
	MYUSART_SendData(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//����ָ����
static void Sendcmd(u8 cmd)
{
	MYUSART_SendData(cmd);
}
//��������
static void Senddata(u8 data)
{
	MYUSART_SendData(data);
}

//����У���
static void SendCheck(u16 check)
{
	MYUSART_SendData(check>>8);  //���͸�8λ
	MYUSART_SendData(check);    //���͵�8λ
}

//u8Type 8 ������  2 �����
static void SendLengthData(u8 u8Type,u8 *pData,u16 u16Len)
{

    u16 i;
    u16 u16CheckSum=0;
    
	SendHead();
	SendAddr();
	SendFlag(u8Type);//�������ʶ
	SendLength(u16Len+2); //����У����
    
    for(i=0;i<u16Len;i++){
       Senddata(pData[i]);
       u16CheckSum=u16CheckSum+pData[i];
    }

    u16CheckSum = u8Type+u16Len+2+u16CheckSum;
//    printf("u8Type 0x%02X u16Len %d u16CheckSum 0x%04X\r\n",u8Type,u16Len,u16CheckSum);
    SendCheck(u16CheckSum);


}


//�ж��жϽ��յ�������û��Ӧ���
//waittimeΪ�ȴ��жϽ������ݵ�ʱ�䣨��λ1ms��
//����ֵ�����ݰ��׵�ַ
static u8 *JudgeStr(u16 waittime)
{
	char *data;
	u8 str[8];
	str[0]=0xef;str[1]=0x01;str[2]=AS608Addr>>24;
	str[3]=AS608Addr>>16;str[4]=AS608Addr>>8;
	str[5]=AS608Addr;str[6]=0x07;str[7]='\0';
	 as608_usart_st.RX_flag=0;
	while(--waittime)
	{
		HAL_Delay(1);
		if(as608_usart_st.RX_flag==1)//���յ�һ������
		{
			as608_usart_st.RX_flag=0;
            #if 0
			 for(uint16_t i=0;i<ether_st.RX_Size;i++)
			{
				 
			printf("\r\n");
			printf("%03d 0x%02X",i,ether_st.RX_pData[i]);
			ether_st.RX_flag=0;
            printf("\r\n");
							
			}
            #endif
			data=strstr((const char*)as608_usart_st.RX_pData,(const char*)str);
			if(data)
				return (u8*)data;	
		}
	}
	return 0;
}

static u16 AS608_WaitReceive(u16 waittime,u8 **pRecData)
{
    as608_usart_st.RX_flag=0;
	while(--waittime)
	{
		HAL_Delay(1);
		if(as608_usart_st.RX_flag==1)//���յ�һ������
		{
		   as608_usart_st.RX_flag=0;
		   *pRecData=as608_usart_st.RX_pData;
		}
	}
	return as608_usart_st.RX_Size;
}

//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer�� 
//ģ�鷵��ȷ����
u8 PS_GetImage(void)
{
  u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
  temp =  0x01+0x03+0x01;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2			 
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//ģ�鷵��ȷ����
u8 PS_GenChar(u8 BufferID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ� 
//ģ�鷵��ȷ����
u8 PS_Match(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣			
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
	{
		ensure = data[9];
		p->pageID   =(data[10]<<8)+data[11];
		p->mathscore=(data[12]<<8)+data[13];	
	}
	else
		ensure = 0xff;
	return ensure;	
}
//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2	
//˵��:  ģ�鷵��ȷ����
u8 PS_RegModel(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}
//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�			
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
u8 PS_StoreChar(u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(u8)PageID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;	
}
//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
u8 PS_DeletChar(u16 PageID,u16 N)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x0C);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	MYUSART_SendData(N>>8);
	MYUSART_SendData(N);
	temp = 0x01+0x07+0x0C
	+(PageID>>8)+(u8)PageID
	+(N>>8)+(u8)N;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
u8 PS_Empty(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteReg(u8 RegNum,u8 DATA)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x05);
	Sendcmd(0x0E);
	MYUSART_SendData(RegNum);
	MYUSART_SendData(DATA);
	temp = RegNum+DATA+0x01+0x05+0x0E;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	if(ensure==0)
		printf("\r\n���ò����ɹ���");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
u8 PS_ReadSysPara(SysPara *p)
{
	u16 temp;
  u8  ensure=0;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(temp);
	data=JudgeStr(1000);
	if(data)
	{
		ensure = data[9];
		p->PS_max = (data[14]<<8)+data[15];
		p->PS_level = data[17];
		p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
		p->PS_size = data[23];
		p->PS_N = data[25];
	}		
	else
		ensure=0xff;
	if(ensure==0x00)
	{
		printf("\r\nģ�����ָ������=%d\r\n",p->PS_max);
		printf("\r\n�Աȵȼ�=%d\r\n",p->PS_level);
		printf("\r\n��ַ=%x\r\n",p->PS_addr);
		printf("\r\n������=%d\r\n",p->PS_N*9600);
	}
	else 
			printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
u8 PS_SetAddr(u32 PS_addr)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead(); 
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x15);
	MYUSART_SendData(PS_addr>>24);
	MYUSART_SendData(PS_addr>>16);
	MYUSART_SendData(PS_addr>>8);
	MYUSART_SendData(PS_addr);
	temp = 0x01+0x07+0x15
	+(u8)(PS_addr>>24)+(u8)(PS_addr>>16)
	+(u8)(PS_addr>>8) +(u8)PS_addr;				
	SendCheck(temp);
	AS608Addr=PS_addr;//������ָ�������ַ
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;	
		AS608Addr = PS_addr;
	if(ensure==0x00)
		printf("\r\n���õ�ַ�ɹ���");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteNotepad(u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(36);
	Sendcmd(0x18);
	MYUSART_SendData(NotePageNum);
	for(i=0;i<32;i++)
	 {
		 MYUSART_SendData(Byte32[i]);
		 temp += Byte32[i];
	 }
  temp =0x01+36+0x18+NotePageNum+temp;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
u8 PS_ReadNotepad(u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		for(i=0;i<32;i++)
		{
			Byte32[i]=data[10+i];
		}
	}
	else
		ensure=0xff;
	return ensure;
}
//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x1b);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
 	if(data)
	{
		ensure=data[9];
		p->pageID 	=(data[10]<<8) +data[11];
		p->mathscore=(data[12]<<8) +data[13];
	}
	else
		ensure=0xff;
	return ensure;
}
//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
u8 PS_ValidTempleteNum(u16 *ValidN)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		*ValidN = (data[10]<<8) +data[11];
	}		
	else
		ensure=0xff;
	
	if(ensure==0x00)
	{
		printf("\r\n��Чָ�Ƹ���=%d",(data[10]<<8)+data[11]);
	}
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��	
u8 PS_HandShake(u32 *PS_Addr)
{
	SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);	
	HAL_Delay(200);
	if(as608_usart_st.RX_flag==1)//���յ�����
	{		
		
//		  printf("ether_st.RX_Size=%d\r\n",as608_usart_st.RX_Size);
//		  for(uint16_t i=0;i<as608_usart_st.RX_Size;i++)
//		{
//			
//			  printf("ether_st.RX_Size[%d]=%02x\r\n",i,as608_usart_st.RX_pData[i]);
//		}
		if(//�ж��ǲ���ģ�鷵�ص�Ӧ���				
					as608_usart_st.RX_pData[0]==0XEF
				&&as608_usart_st.RX_pData[1]==0X01
				&&as608_usart_st.RX_pData[6]==0X07
			)
			{
				*PS_Addr=(as608_usart_st.RX_pData[2]<<24) + (as608_usart_st.RX_pData[3]<<16)
								+(as608_usart_st.RX_pData[4]<<8) + (as608_usart_st.RX_pData[5]);
			as608_usart_st.RX_flag=0;
				return 0;
			}
			as608_usart_st.RX_flag=0;					
	}
	return 1;		
}
//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
const char *EnsureMessage(u8 ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="OK";break;		
		case  0x01:
			p="���ݰ����մ���";break;
		case  0x02:
			p="��������û����ָ";break;
		case  0x03:
			p="¼��ָ��ͼ��ʧ��";break;
		case  0x04:
			p="ָ��ͼ��̫�ɡ�̫��������������";break;
		case  0x05:
			p="ָ��ͼ��̫ʪ��̫��������������";break;
		case  0x06:
			p="ָ��ͼ��̫�Ҷ�����������";break;
		case  0x07:
			p="ָ��ͼ����������������̫�٣������̫С��������������";break;
		case  0x08:
			p="ָ�Ʋ�ƥ��";break;
		case  0x09:
			p="û������ָ��";break;
		case  0x0a:
			p="�����ϲ�ʧ��";break;
		case  0x0b:
			p="����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ";
		case  0x10:
			p="ɾ��ģ��ʧ��";break;
		case  0x11:
			p="���ָ�ƿ�ʧ��";break;	
		case  0x15:
			p="��������û����Чԭʼͼ��������ͼ��";break;
		case  0x18:
			p="��д FLASH ����";break;
		case  0x19:
			p="δ�������";break;
		case  0x1a:
			p="��Ч�Ĵ�����";break;
		case  0x1b:
			p="�Ĵ����趨���ݴ���";break;
		case  0x1c:
			p="���±�ҳ��ָ������";break;
		case  0x1f:
			p="ָ�ƿ���";break;
		case  0x20:
			p="��ַ����";break;
		default :
			p="ģ�鷵��ȷ��������";break;
	}
 return p;	
}


//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
  printf("ShowErrMessage =%d\r\n",ensure);
}
//����ģ�� PS_LoadChar 
//����˵���� �� flash ���ݿ���ָ�� ID �ŵ�ָ��ģ����뵽ģ�建�� 
////�� CharBuffer1 �� CharBuffer2 
//��������� BufferID(��������)��PageID(ָ�ƿ�ģ���) 
//���ز����� ȷ���� 
// ָ����룺 07H 
//ָ�����ʽ��
u8 PS_LoadChar(u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x06);
	Sendcmd(0x07);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x07+BufferID
	+(PageID>>8)+(u8)PageID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}


static u8 AS806_ExtractChar(u8 *pRecv,u16 pRecvSize,u8 *pFgchar,u16 *u16FgLength)
{
  char *pSegData;
  u16 u16Offset = 0;
  AS608_DATA_S stRecv;

  printf("AS806_ExtractChar %d\r\n",pRecvSize);

  pSegData = (char *)pRecv;
  stRecv.stHeader.u16Header = 0x01EF; //��С��
  stRecv.pData = pFgchar;
 // AS608_DebugPrintf(pSegData,pRecvSize-u16Offset);
  while((pSegData = strstr((const char *)pSegData , (const char *)&stRecv.stHeader.u16Header ))!=NULL){
   
    stRecv.stHeader.u8Flag = pSegData[6];
		stRecv.stHeader.u16Length = pSegData[8];
    printf("u16Length %d\r\n",stRecv.stHeader.u16Length);
    if(stRecv.stHeader.u8Flag==0x02||stRecv.stHeader.u8Flag==0x08){
			
     //��ȡ��Чָ������
       memcpy(&stRecv.pData[u16Offset],&pSegData[9],stRecv.stHeader.u16Length-2); //����У��λ
       u16Offset = u16Offset+stRecv.stHeader.u16Length-2;
    }
	pSegData = pSegData + sizeof(AS608_DATA_HEAD_S) + stRecv.stHeader.u16Length -3; //�����ö���������
//	AS608_DebugPrintf(pSegData,pRecvSize-u16Offset);
  }
  memset(pRecv,0,pRecvSize); //���Recv�ڴ棬��ֹ��һ�����ݶԱȴ���
  *u16FgLength = u16Offset;
  //AS608_DebugPrintf(pFgchar,*u16FgLength);  
  return 0;
}


static u8 AS806_ExportInChar(u8 *pFgchar,u16 *pFgLength)
{
   u16 u16FgLenTem =0;
   u16 u16PkgCnt =0;
   u16FgLenTem = *pFgLength;
  
   while(u16FgLenTem>0){
   
      if(u16FgLenTem>0x80){ //�����
        SendLengthData(2, &pFgchar[u16PkgCnt*0x80] , 0x80);
        u16PkgCnt++;
        u16FgLenTem = u16FgLenTem - 0x80;
        printf("u16FgLenTem %d\r\n",u16FgLenTem);
      }else{ //������
        SendLengthData(8, &pFgchar[u16PkgCnt*0x80] , u16FgLenTem);
        printf("u16PkgCnt %d\r\n",u16PkgCnt);
        u16FgLenTem = 0;
      } 
   }
  return 0;
}


//�ϴ�������ģ�� PS_UpChar 
//����˵���� �������������е������ļ��ϴ�����λ�� 
//��������� BufferID(��������) 
//���ز����� ȷ���� 
// ָ����룺 08H 
// ָ�����ʽ��

u8 PS_UpChar(u8 BufferID,u8 *pFgchar , u16 *pLength)
{
	u16 temp;
  u8  ensure=0; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x08);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x08+BufferID;
	SendCheck(temp);
	temp=AS608_WaitReceive(2000,&data);
    //��ȡָ������ֵ
    AS806_ExtractChar(data,temp,pFgchar,pLength);

	return ensure;		
} 

//����������ģ�� PS_DownChar 
//����˵���� ��λ�����������ļ���ģ���һ������������ 
// ��������� BufferID(��������) 
// ���ز����� ȷ���� 
// ָ����룺 09H
u8 PS_DownChar(u8 BufferID,u8 *pFgchar , u16 *pLength)
{
	u16 temp;
  u8  ensure; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x09);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x09+BufferID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data){
		ensure=data[9];
        AS806_ExportInChar(pFgchar,pLength); //��ָ��ģ�鷢������ֵ
	}
	else
		ensure=0xff;
	return ensure;		
}

//�ϴ�ͼ�� PS_UpImage 
//����˵���� ��ͼ�񻺳����е������ϴ�����λ�� 
// ��������� none 
//���ز����� ȷ���� 
// ָ����룺 0aH 
//ָ�����ʽ��
u8 PS_UpImage( )
{
	u16 temp;
  u8  ensure; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0a);
	temp = 0x01+0x03+0x0a;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}
//����ͼ�� PS_DownImage 
//����˵���� ��λ������ͼ�����ݸ�ģ�� 
//��������� none 
//���ز����� ȷ���� 
//ָ����룺 0bH 
//ָ�����ʽ��
u8 PS_DownImage( )
{
	u16 temp;
  u8  ensure; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0b);
	temp = 0x01+0x03+0x0b;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}
