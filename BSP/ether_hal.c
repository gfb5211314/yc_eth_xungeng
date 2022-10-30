#include "ether_hal.h"
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#define  eth_usart        huart2
#define  eth_RST_L        HAL_GPIO_WritePin(eth_rst_GPIO_Port, eth_rst_Pin, GPIO_PIN_RESET);
#define  eth_RST_H        HAL_GPIO_WritePin(eth_rst_GPIO_Port, eth_rst_Pin, GPIO_PIN_SET);


ETH_TYPE  ether_st;

uint8_t  local_eth_ip[40]="at+LANIp=192.168.1.90\r\n";
//uint8_t   local_eth_port[30]="at+NRPort0=6962\r\n";
uint8_t  Remote_eth_ip[40]="at+NDomain0=192.168.1.147\r\n";
uint8_t  Remote_eth_port[30]="at+NRPort0=6962\r\n";


//ģ������
void ETH_Rst ( void )
{
		 eth_RST_H;
	HAL_Delay(1000);
	 eth_RST_L;
	 HAL_Delay( 2000 ); 
	 eth_RST_H;
	HAL_Delay(2000);
	HAL_Delay(2000);
  HAL_Delay(2000);
}
//����1�ֽڵ�WIFIģ��
void send_byte_to_eth(uint8_t p)
{

    HAL_UART_Transmit(&eth_usart, (uint8_t*)&p,1, 0xffff);

}
//����һ���ַ���wifiģ��,���ܷ�������0
void eth_send_string(uint8_t *p) {

    while(*p!= '\0') {

        send_byte_to_eth(*p);
        p++;
    }
//	length=sizeof(data)/sizeof(data[0])
}

void send_string_to_eth(uint8_t *p,uint16_t plen)
{
	  HAL_UART_Transmit(&eth_usart, p,plen, 0xffffffff);
	
}
//ֻ��������AT����ͣ��ַ�Ϊ0��û������! �����������㶼����
uint8_t atk_eth_send_cmd(uint8_t *cmd,uint8_t *ack,uint32_t waittime)
{
	     uint8_t sucess_state=0;
	    static  uint8_t comumtate=0; 
	    static  uint32_t eth_wait_tim_sta=0; 
	    static  uint32_t eth_wait_tim_end=0; 
	   switch(comumtate)
		 {
			 //���뷢��״̬
			 case 0 : 
				                  eth_send_string(cmd);
//		  __HAL_UART_ENABLE_IT(&eth_usart, UART_IT_IDLE);
		  HAL_UART_Receive_DMA(&eth_usart,(uint8_t *)ether_st.RX_pData, 50);//�����������
			                    comumtate=1; 
			                    eth_wait_tim_sta=HAL_GetTick();
			              printf("wait_tim_sta=%d\r\n",eth_wait_tim_sta);
//			                   HAL_Delay(1000);
			               break;
			 //����ack״̬
			 case 1 :  if(ether_st.RX_flag==1)
			         {
				 
				           printf("ack:%s\r\n",(uint8_t*)ether_st.RX_pData);
				           if(strstr((const char*)ether_st.RX_pData,(const char*)ack))
			            {
								          ether_st.RX_flag=0;
								        	ether_st.RX_Size=0;
								           sucess_state=1;
								             comumtate=0;
                      memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));					        
				           }
									    ether_st.RX_flag=0;
								     ether_st.RX_Size=0;
                      memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));	
			         }
							 else
							 {
								         eth_wait_tim_end=HAL_GetTick();
//								       printf("wait_tim_end=%d\r\n",wait_tim_end);
								         if((eth_wait_tim_end-eth_wait_tim_sta)>waittime)
												 {
													 
													    comumtate=0;
													   
												 }								 
							 }
							 break;			 
		 }
              return sucess_state;
} 
/*****************************************************************
                       *����HAL*
******************************************************************/
//����DMA���տ����ж�
void  ETH_DMA_START()
{
	   __HAL_UART_ENABLE_IT(&eth_usart, UART_IT_IDLE);
     HAL_UART_Receive_DMA(&eth_usart,(uint8_t *)ether_st.RX_pData,1000);  //����������

}
void Eth_Init()
{
   ETH_Rst();
   ETH_DMA_START();
}
//�������տ����ж�
void  ETH_UsartReceive_IDLE()
{
    uint32_t temp=0;
    if((__HAL_UART_GET_FLAG(&eth_usart,UART_FLAG_IDLE) != RESET))
    {
			  
        __HAL_UART_CLEAR_IDLEFLAG(&eth_usart);//
        HAL_UART_DMAStop(&eth_usart);	
        temp = eth_usart.hdmarx->Instance->CNDTR;//	
          ether_st.RX_Size = eth_data_len - temp;
        ether_st.RX_flag=1;
       HAL_UART_Receive_DMA(&eth_usart,(uint8_t *)ether_st.RX_pData, eth_data_len);
    }
}

//
void USART2_IRQHandler(void)
{
    /* USER CODE BEGIN USART3_IRQn 0 */
      ETH_UsartReceive_IDLE();

    /* USER CODE END USART3_IRQn 0 */
      HAL_UART_IRQHandler(&eth_usart);
    /* USER CODE BEGIN USART3_IRQn 1 */
	
    /* USER CODE END USART3_IRQn 1 */
}
//
void eth_at_open()
{
		HAL_GPIO_WritePin(GPIOB, eth_at_Pin, GPIO_PIN_SET);
  	 HAL_Delay(1000);
	  HAL_GPIO_WritePin(GPIOB, eth_at_Pin, GPIO_PIN_RESET);
	  HAL_Delay(500);
	  HAL_GPIO_WritePin(GPIOB, eth_at_Pin, GPIO_PIN_SET);
	   HAL_Delay(1000);
	
}
//��ʼ��ģ��
uint8_t eth_init()
{

	  uint8_t sucess_stetc=0;
	   static uint8_t wifi_state_num=0;
	   switch(wifi_state_num)
		 {
	
			                  
			 case 0 : 
				                if(atk_eth_send_cmd((uint8_t *)"at+Netmode=0\r\n",(uint8_t *)"at+RNetmode=0",5000)==1)
											  {
												  wifi_state_num=1;											 
											  }
			   break;
			 case 1 :  
				                    printf("����ģʽ\r\n");
				 if(atk_eth_send_cmd((uint8_t *)"at+NProType0=2\r\n",(uint8_t *)"at+RNProType0",5000)==1)
											 {
												  wifi_state_num=2;											 
											 }  
			   break; 
											 
			 	 case 2 :       printf("DHCP����������\r\n");
					 if(atk_eth_send_cmd((uint8_t *)"at+Dhcpd=0\r\n",(uint8_t *)"at+RDhcpd",5000)==1)
											 {
							  wifi_state_num=11;											 
											 }
			   break; 
											 ///////////////
		   case 11:       printf("����WAN��IP��ַ��ȡ��ʽ\r\n");
					 if(atk_eth_send_cmd((uint8_t *)"at+Dhcpc=1\r\n",(uint8_t *)"at+RDhcpc",5000)==1)
											 {
							  wifi_state_num=12;											 
											 }
			   break; 			
	   case 12:       printf("���ñ���IP��ַ\r\n");
					 if(atk_eth_send_cmd((uint8_t *)local_eth_ip,(uint8_t *)"at+RLANIp",5000)==1)
											 {
							  wifi_state_num=13;											 
											 }
			   break; 		
      case 13:       printf("���ñ������ص�ַ��ַ\r\n");
					 if(atk_eth_send_cmd((uint8_t *)"at+LANIpMask=255.255.255.0\r\n",(uint8_t *)"at+RLANIpMask",5000)==1)
											 {
							  wifi_state_num=3;											 
											 }
			   break; 												 
			  	 case 3 :        printf("123\r\n");
						 if(atk_eth_send_cmd((uint8_t *)Remote_eth_ip,(uint8_t *)"at+RNDomain0",5000)==1)
											 {
							  wifi_state_num=4;											 
											 }
    		   break; 
			  	 case 4 :    
						    printf("����IP��ַ\r\n");
						 if(atk_eth_send_cmd((uint8_t *)Remote_eth_port,(uint8_t *)"at+RNRPort0",5000)==1)
											 {
							     wifi_state_num=5;											 
											 }
			   break; 
			  	   case 5 :         if(atk_eth_send_cmd((uint8_t *)"at+NLPort0=8888\r\n",(uint8_t *)"at+RNLPort0",5000)==1)
											    {
							               wifi_state_num=6;											 
											    }
			   break; 
           case 6 :         if(atk_eth_send_cmd((uint8_t *)"at+Save=1\r\n",(uint8_t *)"at+RSave=1",5000)==1)
											    {
							               wifi_state_num=7;											 
											    }
			      break; 
						 case 7 :         if(atk_eth_send_cmd((uint8_t *)"at+Apply=1\r\n",(uint8_t *)"at+RApply=1",5000)==1)
											    {
							               wifi_state_num=8;											 
											    }
			      break; 
		       case 8 :         atk_eth_send_cmd((uint8_t *)"at+Reboot=1\r\n",(uint8_t *)"at+Reboot",1000);
											    
							               wifi_state_num=9;		
                               												
											       HAL_Delay(1000);
			      break; 									 
				
					 
			 	  	 case 9 :        
                           HAL_Delay(1000);
                           HAL_Delay(1000);
                           HAL_Delay(1000);
                           HAL_Delay(1000);	
                           HAL_Delay(1000);
                           HAL_Delay(1000);
                           HAL_Delay(1000);
                           HAL_Delay(1000);	
                           HAL_Delay(1000);
                           HAL_Delay(1000);
                           HAL_Delay(1000);
  HAL_Delay(1000);							 
sucess_stetc=1;								 
//							 printf("����͸���ɹ�\r\n");
//							 if(atk_eth_send_cmd((uint8_t *)"at+SAtMode=0\r\n",(uint8_t *)"at+RSAtMode",5000)==1)
//											    {
//													
//							               wifi_state_num=8;											 
//											    }
													
			   break; 		 
		 }
	     return  sucess_stetc;
}


//�ָ���������
void reset_ethdevinit()
{
    	HAL_GPIO_WritePin(eth_at_GPIO_Port, eth_at_Pin, GPIO_PIN_SET);
  	 HAL_Delay(1000);
	  HAL_GPIO_WritePin(eth_at_GPIO_Port, eth_at_Pin, GPIO_PIN_RESET);
	  HAL_Delay(1000);
		 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
		 HAL_Delay(1000);
	  HAL_GPIO_WritePin(eth_at_GPIO_Port, eth_at_Pin, GPIO_PIN_SET);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	 HAL_Delay(1000);
	

}
