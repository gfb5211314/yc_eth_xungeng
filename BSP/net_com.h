#ifndef  __NET_COM_H__
#define  __NET_COM_H__

#include "main.h"
//入网
#define   device_type              0x06 //有线巡更

#define  Access_request_function    0x01      //请求入网
	
#define	 State_net_function         0x02    //入网状态上报
	
#define	 Equipment_report_function  0x05  //设备号上报

#define  Equipment_run_state_function     0x08  //设备状态上报
 
//业务
#define  CARD_DAKA_CMD                  0x09
#define  ZHIWEN_DAKA_CMD                0x10
#define  XINTIAO_CMD                    0x12

//音频
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
	 Access_request=0U,   //入网请求
	 Access_request_ack,   //入网应答
	 State_net,     //入网状态
	 State_net_ack,   //入网状态应答
	 Equipment_report,  //设备上报
	 Equipment_report_ack  //上报应答
}net_in_flow;

typedef enum{
	 CARD_DAKA=0U,   //卡号打卡
	 ZHIWEN_DAKA,   //指纹打卡
	 DEV_XINTIAO,     //设备心跳

}Business_flow_enum;

//更新指纹

typedef enum{
	 UPDATA_ALL_ZHIWEN=0U,   //更新所有指纹
	 UPDATA_ONE_ZHIWEN,  //更新指纹
	 ZHIWEN_DATA_SEND,     //指纹数据交互
	 UPDATA_DATA_FISNISH,   //更新数据完毕
	 IDLE_ZHIWEN
}Zhiwen_flow_enum;
 extern   Zhiwen_flow_enum     Zhiwen_flow;
 extern   Business_flow_enum   Business_flow;
 extern   net_in_flow  dev_net_in;
#define  eth_ring_len  100
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

typedef struct
{
  	uint8_t   dev_buffer[2]; //设备号
    uint8_t  card_buf[16];        //IDLE receive flag
    uint8_t id_buf[2];          //receive length
	  uint8_t zhiwendaka_buf[4];          //receive length
	  uint8_t xintiao_buf[1];
	  uint8_t all_updata_zhiwen_data[1];  //全部更新数据
	  uint8_t one_updata_zhiwen_data[2];  //一个更新
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
