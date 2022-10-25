#include "rc522_app.h"
#include "stdio.h"
#include "spi.h"
#include "stdio.h"
#include "main.h"
//#include "string.h"

#define SPI_RC522 &hspi1

#define Rc522_cs_en        HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_RESET);
#define Rc522_cs_disen     HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_SET);

#define Rc522_Rst_L        HAL_GPIO_WritePin(RFID_RST_GPIO_Port, RFID_RST_Pin, GPIO_PIN_RESET);
#define Rc522_Rst_H        HAL_GPIO_WritePin(RFID_RST_GPIO_Port, RFID_RST_Pin, GPIO_PIN_SET);
//SPI读一个数据
uint8_t  SPI_ReadData()
{
    uint8_t tenrxdata;
    uint8_t txdata=0;
    if(HAL_SPI_TransmitReceive(SPI_RC522,&txdata,&tenrxdata, 1,0xffff)==0)
    {
//			   printf("spi send read s\r\n");
        return tenrxdata;
    }
    else
    {
        printf("spi send read fail\r\n");
    }

}

//SPI传输一个byte数据
void  SPI_WriteData(uint8_t txData)
{
    uint8_t tenrxdata;
    if(HAL_SPI_TransmitReceive(SPI_RC522,&txData,&tenrxdata,1,0xffff)==0)
    {

    }
    else
    {
        printf("spi send fail\r\n");
    }
}
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
//  unsigned char xTest ;
   ClearBitMask(Status2Reg,0x08);
   WriteRawRC(BitFramingReg,0x07);

   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

   if ((status == MI_OK) && (unLen == 0x10))
   {   
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   
   return status;
}
/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+2] = *(pSnr+i);
        ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {
        ucComMF522Buf[i+2] = *(pKey+i);
    }
    for (i=0; i<6; i++)
    {
        ucComMF522Buf[i+8] = *(pSnr+i);
    }
//   memcpy(&ucComMF522Buf[2], pKey, 6);
//   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
//   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {
            *(pData+i) = ucComMF522Buf[i];
        }
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {
            ucComMF522Buf[i] = *(pData+i);
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        // memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {
            ucComMF522Buf[i] = *(pValue+i);
        }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status != MI_OK)
    {
        return MI_ERR;
    }

    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIndata+i));
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////

char PcdReset(void)
{
    //unsigned char i;
    Rc522_Rst_H;
    HAL_Delay(10);
    Rc522_Rst_L;
    HAL_Delay(10);
    Rc522_Rst_H;
    HAL_Delay(10);

//    if(ReadRawRC(2) == 0x80)
//    {
//        printf("read right\r\n");
//    }
//	 HAL_Delay(1000);
//		 HAL_Delay(1000);
  //  printf("0x%02x\r\n",ReadRawRC(CommandReg));
    WriteRawRC(CommandReg,PCD_RESETPHASE);
		while(ReadRawRC(0x27) != 0x88)
		{
		//	printf("huanxing");
			
		}; //wait chip start ok
//   	  printf("CommandReg=0x%02x\r\n",ReadRawRC(CommandReg));

    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
// printf("ModeReg=%02x\r\n",ReadRawRC(ModeReg));
    WriteRawRC(TReloadRegL,30);
// printf("TReloadRegL=%02x\r\n",ReadRawRC(TReloadRegL));
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
   //  printf("TPrescalerReg=%02x",ReadRawRC(TPrescalerReg));
    WriteRawRC(TxAutoReg,0x40);

	//	 printf("TxAutoReg=%02x",ReadRawRC(TxAutoReg));
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
    unsigned char i, ucAddr;
    unsigned char ucResult=0;
    Rc522_cs_en;
    ucAddr = ((Address<<1)&0x7E)|0x80;
    SPI_WriteData(ucAddr);
    ucResult= SPI_ReadData();
    Rc522_cs_disen;
    return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{
    unsigned char i, ucAddr;

    Rc522_cs_en;
    ucAddr = ((Address<<1)&0x7E);
    SPI_WriteData(ucAddr);
	
	
    SPI_WriteData(value);
    Rc522_cs_disen;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
    case PCD_AUTHENT:
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);

    for (i=0; i<InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pInData[i]);
    }
    WriteRawRC(CommandReg, Command);


    if (Command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg,0x80);
    }

    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg);
                lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {
                    *pOutLenBit = (n-1)*8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n*8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i=0; i<n; i++)
                {
                    pOutData[i] = ReadRawRC(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }

    }


    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE);
    return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////

void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式
//////////////////////////////////////////////////////////////////////

//等待卡离开
void WaitCardOff(void)
{
	char          status;
  unsigned char	TagType[2];

	while(1)
	{
		status = PcdRequest(REQ_ALL, TagType);
		if(status)
		{
			status = PcdRequest(REQ_ALL, TagType);
			if(status)
			{
				status = PcdRequest(REQ_ALL, TagType);
				if(status)
				{
					return;
				}
			}
		}
		HAL_Delay(10);
	}
}
char M500PcdConfigISOType(unsigned char type)
{
    if (type == 'A')                     //ISO14443_A
    {
        ClearBitMask(Status2Reg,0x08);

        /*     WriteRawRC(CommandReg,0x20);    //as default
              WriteRawRC(ComIEnReg,0x80);     //as default
              WriteRawRC(DivlEnReg,0x0);      //as default
           WriteRawRC(ComIrqReg,0x04);     //as default
           WriteRawRC(DivIrqReg,0x0);      //as default
           WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
           WriteRawRC(WaterLevelReg,0x08); //as default
              WriteRawRC(ControlReg,0x20);    //as default
           WriteRawRC(CollReg,0x80);    //as default
        */
        WriteRawRC(ModeReg,0x3D);//3F
        /*	   WriteRawRC(TxModeReg,0x0);      //as default???
        	   WriteRawRC(RxModeReg,0x0);      //as default???
        	   WriteRawRC(TxControlReg,0x80);  //as default???

        	   WriteRawRC(TxSelReg,0x10);      //as default???
           */
        WriteRawRC(RxSelReg,0x86);//84
//      WriteRawRC(RxThresholdReg,0x84);//as default
//      WriteRawRC(DemodReg,0x4D);      //as default

//      WriteRawRC(ModWidthReg,0x13);//26
        WriteRawRC(RFCfgReg,0x7F);   //4F
        /*   WriteRawRC(GsNReg,0x88);        //as default???
           WriteRawRC(CWGsCfgReg,0x20);    //as default???
           WriteRawRC(ModGsCfgReg,0x20);   //as default???
        */
        WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        WriteRawRC(TReloadRegH,0);
        WriteRawRC(TModeReg,0x8D);
        WriteRawRC(TPrescalerReg,0x3E);


        //     PcdSetTmo(106);
        HAL_Delay(1);
        PcdAntennaOn();
    }
    else {
        return (char)-1;
    }

    return MI_OK;
}
void open_fird_vol();
void Rc522_Init()
{  
	  open_fird_vol(); //打开射频电源供电
    PcdReset();
    PcdAntennaOff();
    PcdAntennaOn();
    M500PcdConfigISOType('A');

}
unsigned char  TagType[2]; //卡类型
unsigned char  SelectedSnr[4]; //卡号
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //密钥
unsigned char snr;
unsigned char buf[16];  //FRID数据
unsigned char RFID1[16]={0x88,0x34,0x56,0x78,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0x66};
//打开射频电压
void open_fird_vol()
{
	  HAL_GPIO_WritePin(RF_POW_EN_GPIO_Port, RF_POW_EN_Pin, GPIO_PIN_SET);
		
}
void close_fird_vol()
{
	  HAL_GPIO_WritePin(RF_POW_EN_GPIO_Port, RF_POW_EN_Pin, GPIO_PIN_RESET);
		
}
uint8_t rc522_find_card(uint8_t * card_buf)
{

    unsigned char status,i;
    unsigned int temp=0;
	//寻卡
    	status= PcdRequest(REQ_ALL,TagType);
		if(!status)
		{
			
					   if((TagType[0]==0x04)&&(TagType[1]==0x00))
        {
            printf("卡的类型:S50\r\n");
        }
        else if((TagType[0]==0x02)&&(TagType[1]==0x00))
        {
            printf("卡的类型:S70\r\n");
        }
        else if((TagType[0]==0x08)&&(TagType[1]==0x00))
        {
            printf("卡的类型:X\r\n");
        }
			//防冲撞
			status = PcdAnticoll(SelectedSnr);
			if(!status)
			{
			//	printf("ic卡号:\r\n");
				for(uint8_t i=0;i<4;i++)
				{
					card_buf[i]=SelectedSnr[i];
			//	printf("%02x",SelectedSnr[i]);
					
				}
			
			    	//选定卡片
			//	printf("防冲撞成功\r\n");
				status=PcdSelect(SelectedSnr);
			//	   	printf("选卡成功\r\n");
				if(!status)
				{
					snr = 2;  //扇区号1
          status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);// 校验1扇区密码，密码位于每一扇区第3块
					{
						if(!status)
						{
							
					//		status = PcdWrite((snr*4+0), RFID1);  // 写卡，将buf[0]-buf[16]写入1扇区0块
							status = PcdRead((snr*4+0), buf);  // 读卡，读取1扇区0块数据到buf[0]-buf[16] 
							if(!status)
							{
								temp=1;
								//读写成功，点亮LED
							//	  printf("读出的数据\r\n");
								for(uint8_t i=0;i<16;i++)
								{
							   	card_buf[i]=buf[i];  //读取16个卡号
	               printf("%02x",buf[i]);
								}
								WaitCardOff();  //等待卡离开，可要可不要
							//	status  =PcdHalt(); //不休眠
										if(!status)
							{
					//			printf("进入休眠\r\n");
								  
							}
						 }
					}
					}
				}
			}
				     
			
		}
		else
		{

			temp=0;
		}
		return temp;
	}