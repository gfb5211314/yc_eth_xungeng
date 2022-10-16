#ifndef  __NET_COM_H__
#define  __NET_COM_H__

#include "main.h"
#define   device_type              0x06 //����Ѳ��

#define  Access_request_function    0x01      //��������
	
#define	 State_net_function         0x02    //����״̬�ϱ�
	
#define	 Equipment_report_function  0x05  //�豸���ϱ�

#define  Equipment_run_state_function      0x08  //�豸״̬�ϱ�
 



uint8_t eth_ring_com_pack(uint8_t *rxpbuf,uint8_t *dev_buf,uint8_t function,uint8_t *eth_data,uint16_t pbuf_len,uint8_t *proctukey);
typedef enum{
	 Access_request=0U,   //��������
	 Access_request_ack,   //����Ӧ��
	 State_net,     //����״̬
	 State_net_ack,   //����״̬Ӧ��
	 Equipment_report,  //�豸�ϱ�
	 Equipment_report_ack  //�ϱ�Ӧ��
}net_in_flow;


 extern net_in_flow  dev_net_in;
#define  eth_ring_len  50
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
#define   Manufacturer_ID_1        0xfe
#define   Manufacturer_ID_2        0xef
#define   Manufacturer_ID_3        0xef
#define   Manufacturer_ID_4        0xfe

extern uint8_t  eth_ring_net_in();

#endif