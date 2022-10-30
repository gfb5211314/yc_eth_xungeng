#include "net_com.h"
#include "main.h"
#include "stdio.h"
#include  "as608.h"
#include  "main.h"
#include "usart.h"
#include "as608_app.h"
#include "rc522_app.h"
#include "SX127X_Driver.h"
#include "speak_hal.h"
#include "main_app.h"
#include "ether_hal.h"
#include "string.h"

  COM_DATA_TYPE   com_data;
 net_in_flow  dev_net_in=Access_request;
    Zhiwen_flow_enum     Zhiwen_flow=IDLE_ZHIWEN;
    Business_flow_enum   Business_flow;
 ETH_ACK_TYPE  eth_ack;
//入网请求	
//入网请求	
//做回调函数，会提高代码的移植性（后面优化做）
/*uint8_t  eth_ring_net_in()
{
	    uint8_t  net_ta=0;

	 switch(net_in)
	 {		 
		   case Access_request : 
				              
			
			                        net_in=Access_request_ack;
				 break;		 
		 	 case Access_request_ack :





			 break;
		 	 case State_net :  
		              

			                        net_in=State_net_ack;

			 break;
		 	 case State_net_ack :  

                 


			 break;
		 	 case Equipment_report :
				              
		
			                      
			                        net_in=Equipment_report_ack;
				         
				 break;
		 	 case Equipment_report_ack : 
 
		
							 Line_1A_WT588S(12);//
		              HAL_Delay(4000);
							 		net_ta=1;					 
						 
                
			 break;
		 
		 
		 default :    break;		  
	 }
	        return net_ta;
}*/


ETH_ring_TYPE eth_ring;

uint8_t product_key[8]={0,0,0,0,0,0,0,1};
uint8_t sn_code[12]={0xaa,0,0,0,0,0,0,0,0,0,0,88};
uint8_t eth_ring_com_pack(uint8_t *rxpbuf,uint8_t *dev_buf,uint8_t function,uint8_t *eth_data,uint16_t pbuf_len,uint8_t *proctukey)
{
	  uint16_t index=0;
	 	  for(uint16_t i=0;i<8;i++)
	{
	 rxpbuf[index++]=proctukey[i]; //productkey
	}
	 	  for(uint16_t i=0;i<12;i++)
	{
	 rxpbuf[index++]=sn_code[i]; //SN码
	}
	 rxpbuf[index++]=Manufacturer_ID_1; //包头
	 rxpbuf[index++]=Manufacturer_ID_2;
	 rxpbuf[index++]=dev_buf[0];    //设备号
   rxpbuf[index++]=dev_buf[1];
	 rxpbuf[index++]=device_type;  //设备类型
	 rxpbuf[index++]=function;  //功能码
	 rxpbuf[index++]=pbuf_len/256;  //数据长度
	 rxpbuf[index++]=pbuf_len%256;  //数据长度
	 for(uint16_t i=0;i<pbuf_len;i++)
	{		
		 rxpbuf[index++]=eth_data[i];  //数据
	}	
	 rxpbuf[index++]=Manufacturer_ID_3; //厂家识别码
	 rxpbuf[index++]=Manufacturer_ID_4;	
	   return index;
}
/* 入网函数
 * 参数： 无
 *返回值 ： uint8_t
 */

uint8_t  eth_ring_net_in()
{
	    uint8_t  net_ta=0;
  uint16_t  temp_data_len=0;
	 switch(dev_net_in)
	 {		 
		   case Access_request :       //请求入网
				              eth_ring.dev_pData_len=1;
			                eth_ring.dev_pData[0]=0x00;
				temp_data_len= eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,Access_request_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key);
	       send_string_to_eth(eth_ring.TX_pData,temp_data_len);
			 	printf("net in\r\n");
			                          eth_ack.flag=1;  //等待应答
			                          ether_st.RX_flag=0;
			                          ether_st.RX_Size=0;
			                      //  memset(ether_st.TX_pData, 0, sizeof(ether_st.TX_pData));	
			                        dev_net_in=Access_request_ack;  //等待应答
			                             eth_ack.count=0;
				 break;		 
		 	 case Access_request_ack :  //入网应答
			 
				         if(eth_ack.flag==2)
						    {
							  dev_net_in=Access_request;  //超时重发
							 
						   }  
               if(ether_st.RX_flag==1)
							 {
								 	 	for(uint16_t i=0;i<ether_st.RX_Size-20;i++)
									{
										
										   ether_st.RX_pData[i]=ether_st.RX_pData[i+20];
											 printf("ether_st.RX_pData[%d]=%02x.",i,	ether_st.RX_pData[i]);
										
									}
									//测试返回的消息
    							//		send_string_to_eth(ether_st.RX_pData,ether_st.RX_Size); 
					
								     if(ether_st.RX_pData[4]==device_type)
									   {
											 
										     switch(ether_st.RX_pData[5])
												 {												
													  case 0x01 :  
                               Line_1A_WT588S(ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS);
                                 HAL_Delay(500);														
														   com_data.dev_buffer[0]=ether_st.RX_pData[8]; 
													     com_data.dev_buffer[1]=ether_st.RX_pData[9];
														 	 printf("get dev id %02x-%02x\r\n", com_data.dev_buffer[0],com_data.dev_buffer[1]);
								                     eth_ack.flag=0;
																		dev_net_in =State_net; break;
												}
											}
										 			 ether_st.RX_flag=0;
										
			                          ether_st.RX_Size=0;
											  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));	
							  }
			 break;
					case State_net :   //入网状态发送
		                    eth_ring.dev_pData_len=1;
			                  eth_ring.dev_pData[0]=0x01;
				temp_data_len =eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,State_net_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key);
	            send_string_to_eth(eth_ring.TX_pData,temp_data_len);
			                            eth_ack.flag=1;
			                           ether_st.RX_flag=0;
			                          ether_st.RX_Size=0;
			                        dev_net_in=State_net_ack;

			 break;
		 	 case State_net_ack :  //等待应答
                       if(eth_ack.flag==2)
						 {
							  dev_net_in=State_net;
							 
						 }  
               if(ether_st.RX_flag==1)
							 {
								  
								 	 	for(uint8_t i=0;i<ether_st.RX_Size-20;i++)
									{
										
										ether_st.RX_pData[i]=ether_st.RX_pData[i+20];
									}
								
								     if(ether_st.RX_pData[4]==device_type)
									   {
										     switch(ether_st.RX_pData[5])
												 {												
													  case State_net_function :  
                                     
																	
														     
								                    eth_ack.flag=0;
																		dev_net_in =Equipment_report;
														       break;
												}
											}
										      ether_st.RX_flag=0;
												  ether_st.RX_flag=0;
			                    ether_st.RX_Size=0;
											  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));	
								
							   }

			 break;
		 	 case Equipment_report : //设备号上传
				               eth_ring.dev_pData_len=2;
			                 eth_ring.dev_pData[0]=com_data.dev_buffer[0];
			                 eth_ring.dev_pData[1]=com_data.dev_buffer[1];
				temp_data_len =eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,Equipment_report_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key);
	       send_string_to_eth(eth_ring.TX_pData,temp_data_len);
			                         eth_ack.flag=1;
			                           ether_st.RX_flag=0;
			                          ether_st.RX_Size=0;
			                        dev_net_in=Equipment_report_ack;
				         
				 break;
		 	 case Equipment_report_ack : //等待应答
                      if(eth_ack.flag==2)
						      {
							  dev_net_in=Equipment_report;						 
						      }  
                     if(ether_st.RX_flag==1)
							    {
								  
								 	 	for(uint8_t i=0;i<ether_st.RX_Size-20;i++)
									{
										
										ether_st.RX_pData[i]=ether_st.RX_pData[i+20];
									}
						
								     if(ether_st.RX_pData[4]==device_type)
									   {
										     switch(ether_st.RX_pData[5])
												 {												
													  case Equipment_report_function :  
                                     
											
														           
								                    eth_ack.flag=0;
//																		net_in =Equipment_report;
														        net_ta=1;
													   //  	return  net_ta;
														       break;
												} 
											}
										 ether_st.RX_flag=0;
											
			             ether_st.RX_Size=0;
											  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));									
							   }
			 break;
		 
		 
		 default :    break;		  
	 }
	        return net_ta;
}
//500ms 频率
void  Time_out_Ack_fun()
{
  // printf(" eth_ack.count=%d", eth_ack.count);
	if(eth_ack.flag==1)
	{
 
       eth_ack.count++;
    if( eth_ack.count>60)
		{
				 
			eth_ack.flag=2;
			eth_ack.count=0;
		}
	}
else
{

eth_ack.count=0;

}	
	
}

/* 更新指纹数据 
  */
void Eth_Updata_Finger_Cammand_Task()
{
	 uint16_t temp_data_len=0;
	 switch(Zhiwen_flow)
	 {
		 case UPDATA_ALL_ZHIWEN :
			   temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,ALL_UPDATA_ZHIWEN_CMD,com_data.all_updata_zhiwen_data,sizeof(com_data.all_updata_zhiwen_data),product_key);
	       send_string_to_eth(eth_ring.TX_pData,temp_data_len); 	
          Zhiwen_flow=IDLE_ZHIWEN;
			   
			 break;
		 case UPDATA_ONE_ZHIWEN:
			  temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,ONE_UPDATA_ZHIWEN_CMD,com_data.one_updata_zhiwen_data,sizeof(com_data.one_updata_zhiwen_data),product_key);
	       send_string_to_eth(eth_ring.TX_pData,temp_data_len); 	
          Zhiwen_flow=IDLE_ZHIWEN;
			 break;
		 case ZHIWEN_DATA_SEND :
			 		  temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,ZHIWEN_DATA_CMD,com_data.zhiwen_data_updata,sizeof(com_data.zhiwen_data_updata),product_key);
	       send_string_to_eth(eth_ring.TX_pData,temp_data_len); 	
          Zhiwen_flow=IDLE_ZHIWEN;
			 break;
		 case UPDATA_DATA_FISNISH:
			 	  temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,ZHIWEN_UPDATA_FISNISH_CMD,com_data.zhiwen_data_updata_finish,sizeof(com_data.zhiwen_data_updata_finish),product_key);
	       send_string_to_eth(eth_ring.TX_pData,temp_data_len); 	
          Zhiwen_flow=IDLE_ZHIWEN;
		 break;
		 case IDLE_ZHIWEN : 
			 break;		 
	 }
}
/* 业务通信
  */
extern uint8_t card_num[16]; //卡号缓存
uint32_t xintiao_count=0;
extern System_State_Type   System_state;
void Eth_business_Cammand_Task()
{    
	      uint16_t temp_data_len=0;
	 switch(Business_flow)
	 {
		 case CARD_DAKA :	
			    //  memcpy ( eth_ring.dev_pData, const void * src, size_t num );
       temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,CARD_DAKA_CMD,com_data.card_buf,sizeof(com_data.card_buf),product_key);
	     send_string_to_eth(eth_ring.TX_pData,temp_data_len); 	
          Business_flow=DEV_XINTIAO;
			 break;
		 case ZHIWEN_DAKA:
			      memcpy (com_data.zhiwendaka_buf, com_data.dev_buffer, sizeof(com_data.dev_buffer));
		        memcpy (com_data.zhiwendaka_buf+2, com_data.id_buf, sizeof(com_data.id_buf));
			 temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,ZHIWEN_DAKA_CMD,com_data.zhiwendaka_buf, sizeof(com_data.zhiwendaka_buf),product_key);
	     send_string_to_eth(eth_ring.TX_pData,temp_data_len);
		      Business_flow=DEV_XINTIAO;
			 break;
		 case DEV_XINTIAO :
			 xintiao_count++;
			    if((xintiao_count>2000)&&(System_state==WORKING))
					{
						xintiao_count=0;
			 temp_data_len=eth_ring_com_pack(eth_ring.TX_pData,com_data.dev_buffer,XINTIAO_CMD,com_data.xintiao_buf,sizeof(com_data.xintiao_buf),product_key);
	     send_string_to_eth(eth_ring.TX_pData,temp_data_len);
					}
			 break;
	 
	 }
}

/* 接收数据处理
*/

//void Eth_Com_Data_Process()
//{
//	
//	   if((ether_st.RX_flag==1)&&(System_state==WORKING))
//			 {
//				 //判断包头包尾
//				 if((ether_st.RX_pData[20]==Manufacturer_ID_1)&&(ether_st.RX_pData[21]==Manufacturer_ID_2)&&(ether_st.RX_pData[24]==device_type))
//				 {
//					      switch(ether_st.RX_pData[25])
//								{
//									case CARD_DAKA_CMD :
//										          if(ether_st.RX_pData[28]==1)  
//															{
//																   printf("打卡成功\r\n");
//																   System_Music=CARD_PUNCHED_OUT_SUCCESS; //打卡成功
//															}
//															else
//															{
//																
//																   System_Music=CARD_NUMBER_NOT_ENTER;  //卡号没有录入
//																
//															}
//										break;
//									
//									case ZHIWEN_DAKA_CMD :
//										break;
//									case XINTIAO_CMD :
//										break;
//									
//								}
//					 
//					 
//				 }
//				    ether_st.RX_Size=0;											
//				    ether_st.RX_flag=0;				
//  	  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));					 
//			 }
//								  
//}
//
//void Eth_Com_Data_Process_hal()
//{
//	
//	   if((ether_st.RX_flag==1)&&(System_state==WORKING))
//			 {
//				 //判断包头包尾
//				 if((ether_st.RX_pData[20]==Manufacturer_ID_1)&&(ether_st.RX_pData[21]==Manufacturer_ID_2)&&(ether_st.RX_pData[24]==device_type))
//				 {
//					      switch(ether_st.RX_pData[25])
//								{
//									case CARD_DAKA_CMD :
//										          if(ether_st.RX_pData[28]==1)  
//															{
//																   printf("打卡成功\r\n");
//																   System_Music=CARD_PUNCHED_OUT_SUCCESS; //打卡成功
//																
//															}
//															else
//															{
//																
//																   System_Music=CARD_NUMBER_NOT_ENTER;  //卡号没有录入
//																
//															}
//										break;
//									
//									case ZHIWEN_DAKA_CMD :
//										break;
//									case XINTIAO_CMD :
//										break;
//									
//								}
//					 
//					 
//				 }
//				    ether_st.RX_Size=0;											
//				    ether_st.RX_flag=0;				
//  	  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));					 
//			 }
//								  
//}