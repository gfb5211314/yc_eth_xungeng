/**
  ******************************************************************************
  * �ļ��� ��   SX127X_Driver.c
  * ����   ��   LSD RF Team
  * �汾   ��   V1.0.0
  * ʱ��   ��   15-Aug-2018
  * �ļ�������
  *     ���ļ�ΪSX127Xģ��������㣬������SX127Xģ��Ĺ���ģʽ���ƣ�FIFO��������
  *�书�ʡ�Ƶ�ʵ����ã��Լ�һЩ���ݴ���Ĺ�������
  *    �ͻ���ʹ��SX127Xģ��ʱ����Ҫ��ֲ���ļ������SX127X_HAL.c�и����������ܶ���
  *ʵ���Һ������ȶ�û�иĶ����Ǳ��ļ����Բ���������ֱ��ʹ�ã��ͻ���Ӧ�ò�ֱ�ӵ�
  *�ñ��ļ������Ϳ���ʵ�ָ��ֲ�����
*******************************************************************************/

#include "SX127X_Driver.h"
#include "stm32l0xx_hal.h"
#include "main.h"
/*
  ע�⣬���ļ��ĺ���ʵ���˴�RF��������MCU�ļ���

*/
#define lora_rst_GPIO_Port  LORA_RST_GPIO_Port
#define lora_rst_Pin  LORA_RST_Pin
float G_BandWidthKHz = 500.0;//���ؼ���Symbol����ʹ��
float G_TsXms = 1.024;//1.024ms

/**
  * @��飺SX127X ���ò���
  * @��������
  * @����ֵ����
  */
S_LoRaConfig G_LoRaConfig = {
    470000000,       //�շ�Ƶ��
    BW500KHZ,      //����
    SF08,         //����
    CR_4_5,        //ѭ���������
    15,        //���书��
    true,
    true,
    true,
    64,  //���س���
};
S_LoRaPara G_LoRaPara;

//-------------------------SX127X �Ĵ���------------------------//
//�ò��ֺ���ΪMCU��SX127Xģ��Ĵ������ж�д��FIFO�Ķ�д
//--------------------------------------------------------------//

/**
  * @��飺RF  ��Ĵ�����ַд1�ֽ�����
  * @������uint8_t addr,�Ĵ�����ַ uint8_t data����
  * @����ֵ����
  */
void SX127X_Write( uint8_t addr, uint8_t data )
{
    SX127X_WriteBuffer( addr, &data, 1 );
}
/**
  * @��飺RF  ��Ĵ�����ַ��1�ֽ�����
  * @������uint8_t addr,�Ĵ�����ַ uint8_t *data�����ݴ洢��ַ
  * @����ֵ����
  */
void SX127X_Read( uint8_t addr, uint8_t *data )
{
    SX127X_ReadBuffer( addr, data, 1 );
}
/**
  * @��飺SX127X  ��FIFOд����
  * @������uint8_t *buffer,����ָ�� uint8_t size����
  * @����ֵ����
  */
void SX127X_WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX127X_WriteBuffer( 0, buffer, size );
}
/**
  * @��飺SX127X  ��FIFO������
  * @������uint8_t *buffer,����ָ�� uint8_t size����
  * @����ֵ��uint8_t *buffer �洢��ȡ����
  */
void SX127X_ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX127X_ReadBuffer( 0, buffer, size );
}

//-------------------------SX127X ��������-----------------------//
//�ò��ֺ���Ϊ��SX127Xģ�鸴λ�����书�ʡ�����Ƶ�ʵȲ������ã���
//��SX127X����ģʽ���á����ݰ���д��
//--------------------------------------------------------------//

/**
  * @��飺SX127X  ��λ����
  * @��������
  * @����ֵ����
  */
void SX127X_Reset(void)
{
  HAL_GPIO_WritePin(lora_rst_GPIO_Port, lora_rst_Pin, GPIO_PIN_RESET);//set LoRa RESET(PA_1) to low
	HAL_Delay(200);
	HAL_GPIO_WritePin(lora_rst_GPIO_Port, lora_rst_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
}
/**
  * @��飺SX127X  IO��ʼ��������
  * @��������
  * @����ֵ����
  */
void SX127X_InitIo(void)
{
    SX127X_DIO0_INPUT();
    SX127X_DIO1_INPUT();
    SX127X_DIO2_INPUT();
//    SX127X_SPIGPIO_Init();
    SX127X_RESET_OUTPUT(GPIO_PIN_SET);
    //����������LSD4RF-2F717N10�Լ�LSD4RF-2F717N01��Ҫ�õ���
    SX127X_TXE_OUTPUT(GPIO_PIN_RESET);
    SX127X_RXE_OUTPUT(GPIO_PIN_RESET);
}
/**
  * @��飺SX127X  TX/RX��PA�л�
  * @������bool txEnable  �л��߼����棺��ΪTX���٣���ΪRX   ΪӲ������PA����IO��
  * @����ֵ:��
  */
void SX127XWriteRxTx( bool txEnable )
{
    if( txEnable != 0 )       //���Ϊ�棬ΪTX
    {
        SX127X_RXE_OUTPUT(GPIO_PIN_RESET);
        SX127X_TXE_OUTPUT(GPIO_PIN_SET);
    }
    else  //Ϊ�٣�ΪRX
    {
        SX127X_RXE_OUTPUT(GPIO_PIN_SET);
        SX127X_TXE_OUTPUT(GPIO_PIN_RESET);
    }
}
/**
  * @��飺DIO0�жϿ���ʹ��
  * @��������
  * @����ֵ����
  */
void DIO0_EnableInterrupt(void)
{
    SX127X_DIO0_INTENABLE();
}
/**
  * @��飺DIO0�жϹر�ʹ��
  * @��������
  * @����ֵ����
  */
void DIO0_DisableInterrupt(void)
{
    SX127X_DIO0_INTDISABLE();
}
/**
  * @��飺DIO0����״̬��ȡ
  * @��������
  * @����ֵ��State��ʾDIO0��ȡ�ĵ�ƽ���ߵ�ƽ"1",�͵�ƽ"0"
  */
GPIO_PinState DIO0_GetState(void)
{
    GPIO_PinState State;
    State = SX127X_DIO0_GetState();
    return State;
}
/**
  * @��飺SX127X ����Ƶ�ʼ���ж�
  * @��������
  * @����ֵ��true or false
  */
bool LoRaConfig_Check(void)
{
    if((G_LoRaConfig.LoRa_Freq < 430000000) || (G_LoRaConfig.LoRa_Freq > 920000000))
        return false;
    G_LoRaConfig.BandWidth = (t_BandWidth)(G_LoRaConfig.BandWidth & 0xF0);
    if(G_LoRaConfig.BandWidth > BW500KHZ)
        return false;
    //����BandWidth
    switch(G_LoRaConfig.BandWidth) {
    case BW500KHZ:
        G_BandWidthKHz = 500.0;
        break;
    case BW250KHZ:
        G_BandWidthKHz = 250.0;
        break;
    case BW125KHZ:
        G_BandWidthKHz = 125.0;
        break;
    case BW62_50KHZ:
        G_BandWidthKHz = 62.5;
        break;
    case BW41_66KHZ:
        G_BandWidthKHz = 41.66;
        break;
    case BW31_25KHZ:
        G_BandWidthKHz = 31.25;
        break;
    case BW20_83KHZ:
        G_BandWidthKHz = 20.83;
        break;
    case BW15_62KHZ:
        G_BandWidthKHz = 15.62;
        break;
    case BW10_41KHZ:
        G_BandWidthKHz = 10.41;
        break;
    case BW7_81KHZ:
        G_BandWidthKHz = 7.81;
        break;
    }
    G_LoRaConfig.SpreadingFactor = (t_SpreadingFactor)(G_LoRaConfig.SpreadingFactor & 0xF0);
    if((G_LoRaConfig.SpreadingFactor > SF12) || (G_LoRaConfig.SpreadingFactor < SF06))
        return false;
    //����LoRa��Ԫ���ڣ���λms
    G_TsXms = (2 << ((G_LoRaConfig.SpreadingFactor >> 4) - 1)) / G_BandWidthKHz;

    G_LoRaConfig.CodingRate = (t_CodingRate)(G_LoRaConfig.CodingRate & 0x0E);
    if((G_LoRaConfig.CodingRate > CR_4_8) || (G_LoRaConfig.CodingRate < CR_4_5))
        return false;
    if(G_LoRaConfig.PowerCfig > 15)
        return false;
    if(G_LoRaConfig.PayloadLength > 127)
        return false;
    return true;
}
/**
* @���:SX127X��ʼ��
  * @��������
  * @����ֵ��tSX127xError   ����ö������
  */
tSX127xError SX127X_Lora_init(void)
{
    if(false == LoRaConfig_Check()) //��������������
    {
        return PARAMETER_INVALID;  
    }
//    SX127X_InitIo();               
     SX127X_Reset();              
//    SX127X_SPI_Init();       

    SX127X_Write( REG_LR_OPMODE, RFLR_OPMODE_SLEEP );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );

    //SPI ��֤
    uint8_t test = 0;
    SX127X_Write( REG_LR_HOPPERIOD, 0x91 ); //ѡһ���ò����ļĴ���������֤
    SX127X_Read( REG_LR_HOPPERIOD, &test);
    if(test != 0x91)
        return SPI_READCHECK_WRONG;
    SX127X_Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_01);
    SX127X_Write(REG_LR_LNA, 0x23);
    SX127X_FreqSet(); 
    SX127X_PoutSet();
    SX127X_Write( REG_LR_PARAMP, RFLR_PARAMP_0100_US);
    //  ��PA Ramp��ʱ�䣬����û�LDO���ܿ������������������������ʵ�����PA Rampʱ��
    //  �����Rampʱ����̳�����LDO������ʱ������ֽ���TX��ϵͳ����Ϊ�������������RF�źŲ����ֵ�����
    SX127X_Write( REG_LR_OCP, 0x20 | RFLR_OCP_TRIM_240_MA); //�������ر��� Over Current Protection
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    //ע�⣬��ͷģʽ��Implicit Header��ʱ��������ǰ�涨���շ�˫����PL

    //BW��CR��Header���ޣ���ʼ��
    SX127X_Write( REG_LR_MODEMCONFIG1, \
                  (((uint8_t)G_LoRaConfig.BandWidth) | ((uint8_t)G_LoRaConfig.CodingRate)) | (\
                          (true == G_LoRaConfig.ExplicitHeaderOn) ? 0x00 : 0x01));

    //SF��CRC��ʼ��
    SX127X_Write( REG_LR_MODEMCONFIG2, \
                  ((uint8_t)G_LoRaConfig.SpreadingFactor) | (\
                          (true == G_LoRaConfig.CRCON) ? 0x04 : 0x00));
    if(SF06 == G_LoRaConfig.SpreadingFactor) { //����SF = 6����Ҫ�����ú�����
        uint8_t temp = 0;
        SX127X_Read( 0x31, &temp);
        SX127X_Write(  0x31, (temp & 0xF8) | 0x05);
        SX127X_Write(  0x37, 0x0C);
    }

    //�������Ż������Ƿ�����AutoAGCĬ�Ͽ���
    SX127X_Write( REG_LR_MODEMCONFIG3, ((G_TsXms > 16.0f) ? \
                                        RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON : RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF\
                                       ) | RFLR_MODEMCONFIG3_AGCAUTO_ON);
    return NORMAL;
}
/**
  * @��飺RF�������ݰ�
  * @������uint8_t*data����������ָ��
  * @����ֵ����
  */
void SX127X_TxPacket(uint8_t*data)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0);
    SX127X_Write( REG_LR_PREAMBLELSB, 10);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

    SX127XWriteRxTx(true);//LSD4RF-2F717N10�Լ�LSD4RF-2F717N01ʱ��Ҫ�õ�����Ƶ�л���

    SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
    SX127X_WriteBuffer(REG_LR_FIFO, data, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );
}
/**
  * @��飺RF�������״̬
  * @��������
  * @����ֵ����
  */
void SX127X_StartRx(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0);
    SX127X_Write( REG_LR_PREAMBLELSB, 10);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

//    SX127XWriteRxTx(false);//LSD4RF-2F717N10�Լ�LSD4RF-2F717N01ʱ��Ҫ�õ�����Ƶ�л���

    SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}
/**
  * @��飺RF�������ݰ�
  * @������uint8_t*cbuf��������ָ��
  * @����ֵ����
  */
void SX127X_RxPacket(uint8_t*cbuf)
{
    unsigned char Reg_PKTRSSI, Reg_PKTSNR;
    SX127X_Read( REG_LR_PKTSNRVALUE, &Reg_PKTSNR);

    if((Reg_PKTSNR & 0x80) != 0) {
        Reg_PKTSNR = ((~Reg_PKTSNR + 1) & 0xff) >> 2;
        //SNRǰ��ʾ�ӡ�-��
        G_LoRaPara.Packet_SNR = -Reg_PKTSNR;
    }
    else
    {
        Reg_PKTSNR = (Reg_PKTSNR & 0xff) >> 2;
        G_LoRaPara.Packet_SNR = Reg_PKTSNR;
    }

    SX127X_Read( REG_LR_PKTRSSIVALUE, &Reg_PKTRSSI);

    if(G_LoRaPara.Packet_SNR < 0)
    {
        G_LoRaPara.Packet_RSSI = -164 + Reg_PKTRSSI - G_LoRaPara.Packet_SNR;
    }
    else
    {
        G_LoRaPara.Packet_RSSI = -164 + Reg_PKTRSSI * 16 / 15;
    }

    if(true == G_LoRaConfig.ExplicitHeaderOn) {
        //��ͷ��ô�ӼĴ����ж���������G_LoRaConfig���ó��ȶ�ȡFIFO
        SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength);
        SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    }
    SX127X_ReadFifo(cbuf, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}
/**
  * @��飺��ȡ��ǰ�ŵ��е�RSSIֵ
  * @��������
  * @����ֵ��RSSIֵ
  */
int16_t SX127X_Current_RSSI(void)
{
    unsigned char Reg_RSSIValue;
    ///int16_t temp;
    SX127X_Read( REG_LR_RSSIVALUE, &Reg_RSSIValue);
    G_LoRaPara.Current_RSSI = -164 + Reg_RSSIValue; 
    return G_LoRaPara.Current_RSSI;
}
/**
  * @��飺SX127X����LORA˯��ģʽ
  * @��������
  * @����ֵ����
  */
void SX127X_SleepMode(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
}
/**
  * @��飺SX127X����LORA��STANDBY״̬
  * @��������
  * @����ֵ����
  */
void SX127X_StandbyMode(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
}
/**
  * @��飺SX127X����Ƶ��
  * @��������
  * @����ֵ��tSX127xError   ����ö������
  */
tSX127xError SX127X_FreqSet(void)
{

    if((G_LoRaConfig.LoRa_Freq > 920000000) || (G_LoRaConfig.LoRa_Freq < 430000000))
        return PARAMETER_INVALID;
    uint32_t freq_reg = (uint32_t)(G_LoRaConfig.LoRa_Freq / FREQ_STEP);
    uint8_t test_FRFMSB = 0, test_FRFMID = 0, test_FRFLSB = 0;
    SX127X_StandbyMode();
    SX127X_Write( REG_LR_FRFMSB, (uint8_t)(freq_reg >> 16)); 
    SX127X_Write( REG_LR_FRFMID, (uint8_t)(freq_reg >> 8) );
    SX127X_Write( REG_LR_FRFLSB, (uint8_t)(freq_reg) );

    SX127X_Read(REG_LR_FRFMSB, &test_FRFMSB);
    SX127X_Read(REG_LR_FRFMID, &test_FRFMID);
    SX127X_Read(REG_LR_FRFLSB, &test_FRFLSB);

    if(test_FRFMSB != (uint8_t)(freq_reg >> 16))
        return SPI_READCHECK_WRONG;
    if(test_FRFMID != (uint8_t)(freq_reg >> 8))
        return SPI_READCHECK_WRONG;
    if(test_FRFLSB != (uint8_t)(freq_reg))
        return SPI_READCHECK_WRONG;
    return NORMAL;
}
/**
  * @��飺SX127X���ù���
  * @��������
  * @����ֵ��tSX127xError   ����ö������
  */
tSX127xError SX127X_PoutSet(void)
{
    if(G_LoRaConfig.PowerCfig > 15)
        return PARAMETER_INVALID;
    SX127X_StandbyMode();
    SX127X_Write( REG_LR_PACONFIG, 0xf0 | G_LoRaConfig.PowerCfig);
    uint8_t test = 0;
    SX127X_Read(REG_LR_PACONFIG, &test);
    if((0xf0 | G_LoRaConfig.PowerCfig) != test)
        return SPI_READCHECK_WRONG;
    if(true == G_LoRaConfig.MaxPowerOn)
        SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_ON );
    else
        SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_OFF );
    return NORMAL;
}

/**
  * @��飺SX127X CAD��ʼ��
  * @��������
  * @����ֵ����
  */
void SX127X_CADinit(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0xf0);
    SX127X_Write( REG_LR_PREAMBLELSB, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, \
                  ~(RFLR_IRQFLAGS_CADDONE | RFLR_IRQFLAGS_CADDETECTED));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_10);
    SX127XWriteRxTx(false);     //set RF switch to RX path
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}
/**
  * @��飺SX127X ����CAD�������ŵ����һ��   ����ʱ��ԼΪ(2^SF+32)/BW
  * @��������
  * @����ֵ����
  */
void SX127X_CAD_Sample(void)
{
    SX127XWriteRxTx(false);     //set RF switch to RX path
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_CAD );
}
/**
  * @��飺WOR��ʼ��
  * @��������
  * @����ֵ����
  */
void SX127X_WORInit(void)
{
    SX127X_CADinit();        //CAD���ܳ�ʼ��
}
/**
  * @��飺ִ��WOR����
  * @������uint8_t cclen  0������˯�ߡ�1������CAD���ģʽ
  * @����ֵ����
  */
void SX127X_WOR_Execute(uint8_t cclen)
{
    switch(cclen)
    {
    case 0:   //����˯��
    {
        SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        SX127X_SleepMode();      //����˯��ģʽ
    }
    break;
    case 1:   //����CAD���ģʽ
    {
        SX127X_CAD_Sample();     //����CADһ��
    }
    break;
    default:
        break;
    }
}
/**
  * @��飺WOR��RX
  * @��������
  * @����ֵ����
  */
void SX127X_WOR_Exit(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0xf0);
    SX127X_Write( REG_LR_PREAMBLELSB, 0xff);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
    SX127XWriteRxTx(false);                                    
    SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}


/**
  * @��飺SX127X���ͻ��Ѱ�
  * @������uint8_t*data����������ָ��,ǰ������
  * @����ֵ����
  */
void SX127X_Awake(uint8_t*cbuf, uint16_t Preamble_Length)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    SX127XWriteRxTx(true);
    SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
    SX127X_WriteBuffer(REG_LR_FIFO, cbuf, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
    SX127X_Write( REG_LR_PREAMBLEMSB, (uint8_t)(Preamble_Length >> 8)); //set preamble length
    SX127X_Write( REG_LR_PREAMBLELSB, (uint8_t)Preamble_Length); //set preamble length
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );

}


/***********************************************FSK********************************************************************/

/**
  * @��飺SX127X  FSK��ʼ��
  * @��������
  * @����ֵ����
  */
unsigned char SX127x_Fsk_init(void)
{
    SX127X_InitIo();                // PAIO�ڳ�ʼ��
    SX127X_Reset();                 //��λRF
//    SX127X_SPI_Init();              //SPI��ʼ��

    LSD_RF_SleepMode_FSK();
    LSD_RF_StandbyMode_FSK();
    LSD_RF_Config();                    
    // to test SPI
    uint8_t test = 0;
    SX127X_Write(REG_FSK_SYNCVALUE8, 0x55);
    SX127X_Read(REG_FSK_SYNCVALUE8, &test);
    if(test != 0x55)
    {
        return SPI_READCHECK_WRONG;// something wrong with SPI
    }
    SX127X_Write(REG_FSK_SYNCVALUE8, 0xD3);
    SX127X_Read(REG_FSK_SYNCVALUE8, &test);
    if(test != 0xD3)
    {
        return SPI_READCHECK_WRONG;// something wrong with SPI
    }
    return NORMAL;

}

/**
  * @��飺SX127X  дFSK���ñ�
  * @��������
  * @����ֵ����
  */
void LSD_RF_Config(void)
{
    _SX12XX_REG const *p;
    unsigned char i;
    p = LSD_RFregConfig;
    for(i = sizeof(LSD_RFregConfig) / 2; i > 0; i--)
    {
        SX127X_Write(p->addr, p->val);
        p++;
    }
}
/**
  * @��飺SX127X  ����FSK��standbyģʽ
  * @��������
  * @����ֵ����
  */
void LSD_RF_StandbyMode_FSK(void)
{
    unsigned char cData;
    unsigned int nTimes = 65535;
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_STANDBY );  
    do
    {
        SX127X_Read(REG_FSK_OPMODE, &cData);
        nTimes--;
    }
    while(((cData & 0x07) != RFFSK_OPMODE_STANDBY) && nTimes);
}
/**
  * @��飺SX127X  ����FSK��Sleepģʽ
  * @��������
  * @����ֵ����
  */
void LSD_RF_SleepMode_FSK(void)
{
    unsigned char cData;
    unsigned int nTimes = 65535;
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_SLEEP );  
    do
    {
        SX127X_Read(REG_FSK_OPMODE, &cData);
        nTimes--;
    }
    while(((cData & 0x07) != RFFSK_OPMODE_SLEEP) && nTimes);
}
/**
  * @��飺SX127X  FSKģʽ����FIFO
  * @��������
  * @����ֵ����
  */
void LSD_RF_ClearFIFO(void)
{
    SX127X_Write(REG_FSK_IRQFLAGS2, 0x10);
}
/**
  * @��飺SX127X FSK�������ݰ�
  * @������txBuffer�������ݴ洢���ݵ��׵�ַ��size���ݰ�����
  * @����ֵ����
  */
void LSD_RF_SendPacket_FSK(uint8_t *txBuffer, uint8_t size)
{
    LSD_RF_StandbyMode_FSK();
    SX127X_Write(REG_FSK_DIOMAPPING1, RFFSK_DIOMAPPING1_DIO0_00); 
    SX127XWriteRxTx(true);
    LSD_RF_ClearFIFO();    
    SX127X_Write(REG_FSK_FIFO, size);
    for(unsigned int i = 0; i < size; i++)
    {
        SX127X_Write(REG_FSK_FIFO, txBuffer[i]);
    }
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_TRANSMITTER );
}
/**
  * @��飺SX127X  ����FSK��RXģʽ
  * @��������
  * @����ֵ����
  */
void LSD_RF_RXmode_FSK(void)
{
    LSD_RF_StandbyMode_FSK();                     
    SX127X_Write(REG_FSK_DIOMAPPING1, RFFSK_DIOMAPPING1_DIO0_00);
    LSD_RF_ClearFIFO();        
    SX127XWriteRxTx(false);
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_RECEIVER); 
}

/**
  * @��飺SX127X  FSK��ȡ���ݰ�
  * @������cRxBuf�������ݴ洢���ݵ��׵�ַ��c���ݰ�����
  * @����ֵ����
  */
void LSD_RF_RxPacket_FSK(uint8_t *cRxBuf, uint8_t *cLength)
{
    uint8_t i;
    uint8_t test = 0;
    LSD_RF_StandbyMode_FSK();     
    SX127X_Read(REG_FSK_FIFO, &test);
    *cLength = test;
    for(i = 0; i < *cLength; i++)
        SX127X_Read(REG_FSK_FIFO, &cRxBuf[i]);
    LSD_RF_ClearFIFO();   

}

/***********************************************FSK End********************************************************************/
