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

 net_in_flow  dev_net_in=Access_request;
 ETH_ACK_TYPE  eth_ack;
//��������	
//��������	
//���ص�����������ߴ������ֲ�ԣ������Ż�����
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
uint8_t   dev_buffer[2]={0}; //�豸��
uint8_t product_key[8]={0,0,0,0,0,0,0,0};
uint8_t sn_code[12]={0,0,0,0,0,0,0,0};
uint8_t eth_ring_com_pack(uint8_t *rxpbuf,uint8_t *dev_buf,uint8_t function,uint8_t *eth_data,uint16_t pbuf_len,uint8_t *proctukey)
{
	  uint16_t index=0;
	 	  for(uint16_t i=0;i<8;i++)
	{
	 rxpbuf[index++]=proctukey[i]; //productkey
	}
	 	  for(uint16_t i=0;i<12;i++)
	{
	 rxpbuf[index++]=sn_code[i]; //SN��
	}
	 rxpbuf[index++]=Manufacturer_ID_1; //����ʶ����
	 rxpbuf[index++]=Manufacturer_ID_2;
	 rxpbuf[index++]=dev_buf[0];    //�豸��
   rxpbuf[index++]=dev_buf[1];
	 rxpbuf[index++]=device_type;  //�豸����
	 rxpbuf[index++]=function;  //������
	 for(uint16_t i=0;i<pbuf_len;i++)
	{		
		 rxpbuf[index++]=eth_data[i];  //������		
	}	
	 rxpbuf[index++]=Manufacturer_ID_3; //����ʶ����
	 rxpbuf[index++]=Manufacturer_ID_4;
	
	   return index;
}
/* ��������
 * ������ ��
 *����ֵ �� uint8_t
 */

uint8_t  eth_ring_net_in()
{
	    uint8_t  net_ta=0;

	 switch(dev_net_in)
	 {		 
		   case Access_request :       //��������
				              eth_ring.dev_pData_len=1;
			                eth_ring.dev_pData[0]=0x00;
				 eth_ring_com_pack(eth_ring.TX_pData,dev_buffer,Access_request_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key);
	    send_string_to_eth(eth_ring.TX_pData,eth_ring_com_pack(eth_ring.TX_pData,dev_buffer,Access_request_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key));
			                          eth_ack.flag=1;  //�ȴ�Ӧ��
			                          ether_st.RX_flag=0;
			                          ether_st.RX_Size=0;
			                      //  memset(ether_st.TX_pData, 0, sizeof(ether_st.TX_pData));	
			                        dev_net_in=Access_request_ack;  //�ȴ�Ӧ��
				 break;		 
		 	 case Access_request_ack :  //����Ӧ��
			 
				         if(eth_ack.flag==2)
						    {
							  dev_net_in=Access_request;  //��ʱ�ط�
							 
						   }  
               if(ether_st.RX_flag==1)
							 {
								 	 	for(uint16_t i=0;i<ether_st.RX_Size-20;i++)
									{
										
										   ether_st.RX_pData[i]=ether_st.RX_pData[i+20];
											 printf("ether_st.RX_pData[%d]=%02x.",i,	ether_st.RX_pData[i]);
										
									}
									//���Է��ص���Ϣ
    									send_string_to_eth(ether_st.RX_pData,ether_st.RX_Size); 
					
								     if(ether_st.RX_pData[4]==device_type)
									   {
											 
										     switch(ether_st.RX_pData[5])
												 {												
													  case 0x01 :  
                                       														
														   dev_buffer[0]=ether_st.RX_pData[6]; 
													     dev_buffer[1]=ether_st.RX_pData[7];
														 	 printf("get dev id %02x-%02x\r\n", dev_buffer[0],dev_buffer[1]);
								                     eth_ack.flag=0;
																		dev_net_in =State_net; break;
												}
											}
										 			 ether_st.RX_flag=0;
										
			                          ether_st.RX_Size=0;
											  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));	
							  }
			 break;
					case State_net :   //����״̬����
		                    eth_ring.dev_pData_len=1;
			                  eth_ring.dev_pData[0]=0x01;
				 eth_ring_com_pack(eth_ring.TX_pData,dev_buffer,State_net_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key);
	 send_string_to_eth(eth_ring.TX_pData,eth_ring_com_pack(eth_ring.TX_pData,dev_buffer,State_net_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key));
			                            eth_ack.flag=1;
			                           ether_st.RX_flag=0;
			                          ether_st.RX_Size=0;
			                        dev_net_in=State_net_ack;

			 break;
		 	 case State_net_ack :  //�ȴ�Ӧ��
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
		 	 case Equipment_report : //�豸���ϴ�
				               eth_ring.dev_pData_len=2;
			                 eth_ring.dev_pData[0]=dev_buffer[0];
			                 eth_ring.dev_pData[1]=dev_buffer[1];
				 eth_ring_com_pack(eth_ring.TX_pData,dev_buffer,Equipment_report_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key);
	       send_string_to_eth(eth_ring.TX_pData,eth_ring_com_pack(eth_ring.TX_pData,dev_buffer,Equipment_report_function,eth_ring.dev_pData,eth_ring.dev_pData_len,product_key));
			                         eth_ack.flag=1;
			                           ether_st.RX_flag=0;
			                          ether_st.RX_Size=0;
			                        dev_net_in=Equipment_report_ack;
				         
				 break;
		 	 case Equipment_report_ack : //�ȴ�Ӧ��
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
													     	return  net_ta;
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