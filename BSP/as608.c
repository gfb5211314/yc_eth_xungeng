#include "stm32l0xx_hal.h"
#include <string.h>
#include "stdio.h"
#include "as608.h"
#include "main.h"
#include "gpio.h"

u32 AS608Addr = 0XFFFFFFFF; //默认
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

//开启DMA接收空闲中断
void  AS608_DMA_START()
{
	    __HAL_UART_ENABLE_IT(&AS608_USART, UART_IT_IDLE);
   HAL_UART_Receive_DMA(&AS608_USART,(uint8_t *)as608_usart_st.RX_pData, 50);  //不能启动打开

}
//开启接收空闲中断
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
//初始化PA6为下拉输入		    
//读摸出感应状态(触摸感应时输出高电平信号)
void PS_StaGPIO_Init(void)
{   
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
//  //初始化读状态引脚GPIOA
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//输入下拉模式
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO	
}
//串口发送一个字节
static void MYUSART_SendData(u8 data)
{
//	while((USART2->ISR&0X40)==0); 
//  	USART2->RDR = data;
	  HAL_UART_Transmit(&AS608_USART, (uint8_t *)&data, 1, 0xffff);
}
//发送包头
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//发送地址
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//发送包标识,
static void SendFlag(u8 flag)
{
	MYUSART_SendData(flag);
}
//发送包长度
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//发送指令码
static void Sendcmd(u8 cmd)
{
	MYUSART_SendData(cmd);
}
//发送数据
static void Senddata(u8 data)
{
	MYUSART_SendData(data);
}

//发送校验和
static void SendCheck(u16 check)
{
	MYUSART_SendData(check>>8);  //发送高8位
	MYUSART_SendData(check);    //发送低8位
}

//u8Type 8 结束包  2 传输包
static void SendLengthData(u8 u8Type,u8 *pData,u16 u16Len)
{

    u16 i;
    u16 u16CheckSum=0;
    
	SendHead();
	SendAddr();
	SendFlag(u8Type);//命令包标识
	SendLength(u16Len+2); //加上校验码
    
    for(i=0;i<u16Len;i++){
       Senddata(pData[i]);
       u16CheckSum=u16CheckSum+pData[i];
    }

    u16CheckSum = u8Type+u16Len+2+u16CheckSum;
//    printf("u8Type 0x%02X u16Len %d u16CheckSum 0x%04X\r\n",u8Type,u16Len,u16CheckSum);
    SendCheck(u16CheckSum);


}


//判断中断接收的数组有没有应答包
//waittime为等待中断接收数据的时间（单位1ms）
//返回值：数据包首地址
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
		if(as608_usart_st.RX_flag==1)//接收到一次数据
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
		if(as608_usart_st.RX_flag==1)//接收到一次数据
		{
		   as608_usart_st.RX_flag=0;
		   *pRecData=as608_usart_st.RX_pData;
		}
	}
	return as608_usart_st.RX_Size;
}

//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer。 
//模块返回确认字
u8 PS_GetImage(void)
{
  u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2			 
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//模块返回确认字
u8 PS_GenChar(u8 BufferID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件 
//模块返回确认字
u8 PS_Match(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。			
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//说明:  模块返回确认字，页码（相配指纹模板）
u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2	
//说明:  模块返回确认字
u8 PS_RegModel(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//储存模板 PS_StoreChar
//功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。			
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  模块返回确认字
u8 PS_StoreChar(u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模板
//参数:  PageID(指纹库模板号)，N删除的模板个数。
//说明:  模块返回确认字
u8 PS_DeletChar(u16 PageID,u16 N)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//清空指纹库 PS_Empty
//功能:  删除flash数据库中所有指纹模板
//参数:  无
//说明:  模块返回确认字
u8 PS_Empty(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//写系统寄存器 PS_WriteReg
//功能:  写模块寄存器
//参数:  寄存器序号RegNum:4\5\6
//说明:  模块返回确认字
u8 PS_WriteReg(u8 RegNum,u8 DATA)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
		printf("\r\n设置参数成功！");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//读系统基本参数 PS_ReadSysPara
//功能:  读取模块的基本参数（波特率，包大小等)
//参数:  无
//说明:  模块返回确认字 + 基本参数（16bytes）
u8 PS_ReadSysPara(SysPara *p)
{
	u16 temp;
  u8  ensure=0;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
		printf("\r\n模块最大指纹容量=%d\r\n",p->PS_max);
		printf("\r\n对比等级=%d\r\n",p->PS_level);
		printf("\r\n地址=%x\r\n",p->PS_addr);
		printf("\r\n波特率=%d\r\n",p->PS_N*9600);
	}
	else 
			printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//设置模块地址 PS_SetAddr
//功能:  设置模块地址
//参数:  PS_addr
//说明:  模块返回确认字
u8 PS_SetAddr(u32 PS_addr)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead(); 
	SendAddr();
	SendFlag(0x01);//命令包标识
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
	AS608Addr=PS_addr;//发送完指令，更换地址
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;	
		AS608Addr = PS_addr;
	if(ensure==0x00)
		printf("\r\n设置地址成功！");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
//参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
//说明:  模块返回确认字
u8 PS_WriteNotepad(u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//读记事PS_ReadNotepad
//功能：  读取FLASH用户区的128bytes数据
//参数:  NotePageNum(0~15)
//说明:  模块返回确认字+用户信息
u8 PS_ReadNotepad(u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//高速搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
//参数:  BufferID， StartPage(起始页)，PageNum（页数）
//说明:  模块返回确认字+页码（相配指纹模板）
u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//读有效模板个数 PS_ValidTempleteNum
//功能：读有效模板个数
//参数: 无
//说明: 模块返回确认字+有效模板个数ValidN
u8 PS_ValidTempleteNum(u16 *ValidN)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
		printf("\r\n有效指纹个数=%d",(data[10]<<8)+data[11]);
	}
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//与AS608握手 PS_HandShake
//参数: PS_Addr地址指针
//说明: 模块返新地址（正确地址）	
u8 PS_HandShake(u32 *PS_Addr)
{
	SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);	
	HAL_Delay(200);
	if(as608_usart_st.RX_flag==1)//接收到数据
	{		
		
//		  printf("ether_st.RX_Size=%d\r\n",as608_usart_st.RX_Size);
//		  for(uint16_t i=0;i<as608_usart_st.RX_Size;i++)
//		{
//			
//			  printf("ether_st.RX_Size[%d]=%02x\r\n",i,as608_usart_st.RX_pData[i]);
//		}
		if(//判断是不是模块返回的应答包				
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
//模块应答包确认码信息解析
//功能：解析确认码错误信息返回信息
//参数: ensure
const char *EnsureMessage(u8 ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="OK";break;		
		case  0x01:
			p="数据包接收错误";break;
		case  0x02:
			p="传感器上没有手指";break;
		case  0x03:
			p="录入指纹图像失败";break;
		case  0x04:
			p="指纹图像太干、太淡而生不成特征";break;
		case  0x05:
			p="指纹图像太湿、太糊而生不成特征";break;
		case  0x06:
			p="指纹图像太乱而生不成特征";break;
		case  0x07:
			p="指纹图像正常，但特征点太少（或面积太小）而生不成特征";break;
		case  0x08:
			p="指纹不匹配";break;
		case  0x09:
			p="没搜索到指纹";break;
		case  0x0a:
			p="特征合并失败";break;
		case  0x0b:
			p="访问指纹库时地址序号超出指纹库范围";
		case  0x10:
			p="删除模板失败";break;
		case  0x11:
			p="清空指纹库失败";break;	
		case  0x15:
			p="缓冲区内没有有效原始图而生不成图像";break;
		case  0x18:
			p="读写 FLASH 出错";break;
		case  0x19:
			p="未定义错误";break;
		case  0x1a:
			p="无效寄存器号";break;
		case  0x1b:
			p="寄存器设定内容错误";break;
		case  0x1c:
			p="记事本页码指定错误";break;
		case  0x1f:
			p="指纹库满";break;
		case  0x20:
			p="地址错误";break;
		default :
			p="模块返回确认码有误";break;
	}
 return p;	
}


//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
  printf("ShowErrMessage =%d\r\n",ensure);
}
//读出模板 PS_LoadChar 
//功能说明： 将 flash 数据库中指定 ID 号的指纹模板读入到模板缓冲 
////区 CharBuffer1 或 CharBuffer2 
//输入参数： BufferID(缓冲区号)，PageID(指纹库模板号) 
//返回参数： 确认字 
// 指令代码： 07H 
//指令包格式：
u8 PS_LoadChar(u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
  stRecv.stHeader.u16Header = 0x01EF; //大小端
  stRecv.pData = pFgchar;
 // AS608_DebugPrintf(pSegData,pRecvSize-u16Offset);
  while((pSegData = strstr((const char *)pSegData , (const char *)&stRecv.stHeader.u16Header ))!=NULL){
   
    stRecv.stHeader.u8Flag = pSegData[6];
		stRecv.stHeader.u16Length = pSegData[8];
    printf("u16Length %d\r\n",stRecv.stHeader.u16Length);
    if(stRecv.stHeader.u8Flag==0x02||stRecv.stHeader.u8Flag==0x08){
			
     //提取有效指纹数据
       memcpy(&stRecv.pData[u16Offset],&pSegData[9],stRecv.stHeader.u16Length-2); //除掉校验位
       u16Offset = u16Offset+stRecv.stHeader.u16Length-2;
    }
	pSegData = pSegData + sizeof(AS608_DATA_HEAD_S) + stRecv.stHeader.u16Length -3; //跳过该段所有数据
//	AS608_DebugPrintf(pSegData,pRecvSize-u16Offset);
  }
  memset(pRecv,0,pRecvSize); //清空Recv内存，防止下一次数据对比错误
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
   
      if(u16FgLenTem>0x80){ //传输包
        SendLengthData(2, &pFgchar[u16PkgCnt*0x80] , 0x80);
        u16PkgCnt++;
        u16FgLenTem = u16FgLenTem - 0x80;
        printf("u16FgLenTem %d\r\n",u16FgLenTem);
      }else{ //结束包
        SendLengthData(8, &pFgchar[u16PkgCnt*0x80] , u16FgLenTem);
        printf("u16PkgCnt %d\r\n",u16PkgCnt);
        u16FgLenTem = 0;
      } 
   }
  return 0;
}


//上传特征或模板 PS_UpChar 
//功能说明： 将特征缓冲区中的特征文件上传给上位机 
//输入参数： BufferID(缓冲区号) 
//返回参数： 确认字 
// 指令代码： 08H 
// 指令包格式：

u8 PS_UpChar(u8 BufferID,u8 *pFgchar , u16 *pLength)
{
	u16 temp;
  u8  ensure=0; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
	SendLength(0x04);
	Sendcmd(0x08);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x08+BufferID;
	SendCheck(temp);
	temp=AS608_WaitReceive(2000,&data);
    //提取指纹特征值
    AS806_ExtractChar(data,temp,pFgchar,pLength);

	return ensure;		
} 

//下载特征或模板 PS_DownChar 
//功能说明： 上位机下载特征文件到模块的一个特征缓冲区 
// 输入参数： BufferID(缓冲区号) 
// 返回参数： 确认字 
// 指令代码： 09H
u8 PS_DownChar(u8 BufferID,u8 *pFgchar , u16 *pLength)
{
	u16 temp;
  u8  ensure; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
	SendLength(0x04);
	Sendcmd(0x09);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x09+BufferID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data){
		ensure=data[9];
        AS806_ExportInChar(pFgchar,pLength); //向指纹模块发送特征值
	}
	else
		ensure=0xff;
	return ensure;		
}

//上传图像 PS_UpImage 
//功能说明： 将图像缓冲区中的数据上传给上位机 
// 输入参数： none 
//返回参数： 确认字 
// 指令代码： 0aH 
//指令包格式：
u8 PS_UpImage( )
{
	u16 temp;
  u8  ensure; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//下载图像 PS_DownImage 
//功能说明： 上位机下载图像数据给模块 
//输入参数： none 
//返回参数： 确认字 
//指令代码： 0bH 
//指令包格式：
u8 PS_DownImage( )
{
	u16 temp;
  u8  ensure; 
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
