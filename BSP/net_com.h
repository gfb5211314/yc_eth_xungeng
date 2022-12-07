#ifndef  __NET_COM_H__
#define  __NET_COM_H__

#include "main.h"
//����
#define   device_type              0x06 //����Ѳ��

#define  Access_request_function    0x01      //��������
	
#define	 State_net_function         0x02    //����״̬�ϱ�
	
#define	 Equipment_report_function  0x05  //�豸���ϱ�

#define  Equipment_run_state_function     0x08  //�豸״̬�ϱ�
 
//ҵ��
#define  CARD_DAKA_CMD                  0x09
#define  ZHIWEN_DAKA_CMD                0x10
#define  XINTIAO_CMD                    0x12

//��Ƶ
#define  VOICE_MUTE_CMD                 0x40

#define  ALL_UPDATA_ZHIWEN_CMD          0x20
#define  ONE_UPDATA_ZHIWEN_CMD          0x21
#define  ZHIWEN_DATA_CMD                0x22
#define  ZHIWEN_UPDATA_FISNISH_CMD      0x23


uint8_t eth_ring_com_pack(uint8_t *rxpbuf,uint8_t *dev_buf,uint8_t function,uint8_t *eth_data,uint16_t pbuf_len,uint8_t *proctukey);
extern void Eth_business_Cammand_Task(void);
extern void Eth_Updata_Finger_Cammand_Task(void);
extern void Eth_Com_Data_Process(void);
typedef enum{
	 Access_request=0U,   //��������
	 Access_request_ack,   //����Ӧ��
	 State_net,     //����״̬
	 State_net_ack,   //����״̬Ӧ��
	 Equipment_report,  //�豸�ϱ�
	 Equipment_report_ack  //�ϱ�Ӧ��
}net_in_flow;

typedef enum{
	 CARD_DAKA=0U,   //���Ŵ�
	 ZHIWEN_DAKA,   //ָ�ƴ�
	 DEV_XINTIAO,     //�豸����

}Business_flow_enum;

//����ָ��

typedef enum{
	 UPDATA_ALL_ZHIWEN=0U,   //��������ָ��
	 UPDATA_ONE_ZHIWEN,  //����ָ��
	 ZHIWEN_DATA_SEND,     //ָ�����ݽ���
	 UPDATA_DATA_FISNISH,   //�����������
	 IDLE_ZHIWEN
}Zhiwen_flow_enum;
 extern   Zhiwen_flow_enum     Zhiwen_flow;
 extern   Business_flow_enum   Business_flow;
 extern   net_in_flow  dev_net_in;
#define  eth_ring_len  100
typedef struct
{
	  uint8_t flag;  //������ʱ����
	  uint32_t count; //��ʱ����
}ETH_ACK_TYPE;
extern ETH_ACK_TYPE  eth_ack;
typedef struct
{
    uint8_t  RX_flag;        //IDLE receive flag
    uint16_t RX_Size;          //receive length
	  uint16_t count;
    uint16_t dev_pData_len;          //receive length
    uint8_t  RX_pData[eth_ring_len];
  	uint8_t  dev_pData[eth_ring_len];
    uint8_t  TX_pData[eth_ring_len];
}ETH_ring_TYPE;

typedef struct
{
  	uint8_t   dev_buffer[2]; //�豸��
    uint8_t  card_buf[16];        //IDLE receive flag
    uint8_t id_buf[2];          //receive length
	  uint8_t zhiwendaka_buf[4];          //receive length
	  uint8_t xintiao_buf[1];
	  uint8_t all_updata_zhiwen_data[1];  //ȫ����������
	  uint8_t one_updata_zhiwen_data[2];  //һ������
	  uint8_t zhiwen_data_updata[1];
   	uint8_t zhiwen_data_updata_finish[1];
		uint8_t zhiwen_data_one_size[768];
}COM_DATA_TYPE;
extern  COM_DATA_TYPE   com_data;

#define   Manufacturer_ID_1        0xfe
#define   Manufacturer_ID_2        0xef
#define   Manufacturer_ID_3        0xef
#define   Manufacturer_ID_4        0xfe

extern uint8_t  eth_ring_net_in(void);

#endif
