#ifndef  __NET_COM_H__
#define  __NET_COM_H__

#include "main.h"
#define   device_type              0x06 //有线巡更

#define  Access_request_function    0x01      //请求入网
	
#define	 State_net_function         0x02    //入网状态上报
	
#define	 Equipment_report_function  0x05  //设备号上报

#define  Equipment_run_state_function      0x08  //设备状态上报
 



uint8_t eth_ring_com_pack(uint8_t *rxpbuf,uint8_t *dev_buf,uint8_t function,uint8_t *eth_data,uint16_t pbuf_len,uint8_t *proctukey);
typedef enum{
	 Access_request=0U,   //入网请求
	 Access_request_ack,   //入网应答
	 State_net,     //入网状态
	 State_net_ack,   //入网状态应答
	 Equipment_report,  //设备上报
	 Equipment_report_ack  //上报应答
}net_in_flow;


 extern net_in_flow  dev_net_in;
#define  eth_ring_len  50
typedef struct
{
	  uint8_t flag;  //开启超时发送
	  uint32_t count; //超时计数
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