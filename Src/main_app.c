#include "main_app.h"
#include "SX127X_Driver.h"
#include "SX127X_Hal.h"
#include "stdio.h"
#include "bsp_eeprom.h"
#include "string.h"
#include "ether_hal.h"
#include "stm_flash.h"
#include "net_com.h"
#include "speak_hal.h"
#include "rc522_app.h"
#include "as608.h"
//ҵ���
System_State_Type   System_state=DEV_NET_IN; //  DEV_NET_IN
System_Music_Type   System_Music;
zhiwen_cur_num      zhiwen_cur=ZHIWEN_IDLE;
uint8_t card_num[16]; //���Ż���
//����״̬��
void Play_Music_Fun()
{
	  	switch(System_Music)
			{
				case MUTE_VOICE : //����
//					        if(HAL_GetTick()%2000==0)
//									{
//					    	     Line_1A_WT588S(MUTE_VOICE);
//									}
				      
					break;
				case CLOCK_IN_SUCESS :
					       Line_1A_WT588S(CLOCK_IN_SUCESS);
			         	System_Music=MUTE_VOICE;
				break;
				case 	 CARD_NUMBER_NOT_ENTER://����û��¼��  :
					       Line_1A_WT588S(CARD_NUMBER_NOT_ENTER);
			      	System_Music=MUTE_VOICE;
					
				break;
				case  FINGER_SUCCESS_RECORDED://ָ��¼��ɹ�
					Line_1A_WT588S(FINGER_SUCCESS_RECORDED);
				System_Music=MUTE_VOICE;
					
				break;
				case  FINGER_PRINT_DELETED_SUCCESS://ָ��ɾ���ɹ�
					Line_1A_WT588S(FINGER_PRINT_DELETED_SUCCESS);
				System_Music=MUTE_VOICE;
					
				break;
				case FINGER_PRINT_PUNCHED_IN_SUCCESS:  //ָ�ƴ򿨳ɹ�
					Line_1A_WT588S(FINGER_PRINT_PUNCHED_IN_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
				case FINGER_PRINTS_NOT_RECORDED:		//ָ��û��¼��
					Line_1A_WT588S(System_Music);
				System_Music=MUTE_VOICE;
				break;
				case SYNCHRONIZING_DATA_SUCCESS: //ͬ�����ݳɹ�
					Line_1A_WT588S(SYNCHRONIZING_DATA_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
				case  START_SYNCHRON_DATA:	//��ʼͬ������
					Line_1A_WT588S(START_SYNCHRON_DATA);
				System_Music=MUTE_VOICE;
				break;
				case FINGER_PRINT_WRITE_SUCCESS://д��ָ�Ƴɹ�
					Line_1A_WT588S(FINGER_PRINT_WRITE_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
				case  DIDI: //�ε�
					Line_1A_WT588S(DIDI);
				System_Music=MUTE_VOICE;
				break;
					
					case  LOW_BATTERY_POWER://��ص�������
					Line_1A_WT588S(LOW_BATTERY_POWER);
					System_Music=MUTE_VOICE;
				break;
					
      	case   ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS:		//�������سɹ��������ַ�ɹ�
					Line_1A_WT588S(ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
					
        case   SET_WORK:		//��ʼ����
					Line_1A_WT588S(SET_WORK);
				System_Music=MUTE_VOICE;
				break;
	      case   START_RECORD_FINGER_ONCE: //��ʼ¼��ָ��һ��
					Line_1A_WT588S(START_RECORD_FINGER_ONCE);
				System_Music=MUTE_VOICE;
				break;
		    case    START_RECORD_FINGER_TWICE:  //   //��ʼ¼��ָ�ƶ���
					Line_1A_WT588S(START_RECORD_FINGER_TWICE);
				System_Music=MUTE_VOICE;
				break;
			  case   CARD_NUM_BEING_VERIFIED:	//��������У��
						Line_1A_WT588S(CARD_NUM_BEING_VERIFIED);
			 	System_Music=MUTE_VOICE;
				break;
				case  CARD_PUNCHED_OUT_SUCCESS:  //��Ƭ�򿨳ɹ�
					Line_1A_WT588S(CARD_PUNCHED_OUT_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
						
				case     PLEASE_RECORD_FINGER_PRINT:		//��¼��ָ��
					Line_1A_WT588S(PLEASE_RECORD_FINGER_PRINT);
				System_Music=MUTE_VOICE;
				break;
				case     FINGER_PRINT_RECORD_SUCCESS : //¼��ָ�Ƴɹ�
					Line_1A_WT588S(FINGER_PRINT_RECORD_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
		}	
}
uint16_t zhiwenid_temp;
uint8_t zhiwenid_temp_state=0;
 extern COM_DATA_TYPE   com_data;
uint8_t  test_tim_count=0;

void Test_Dev_Code()
{
    while(1)
{
  if(rc522_find_card(com_data.card_buf))         //���� ����ָ��
									{
										//	System_Music=DIDI;
										   											
                       	Line_1A_WT588S(DIDI);
                  										
                           HAL_Delay(500);
																		
									}
									zhiwenid_temp_state=press_FR(&zhiwenid_temp);
				         	  if(zhiwenid_temp_state==1)         //�ҵ�ָ��
									{
										//	System_Music=DIDI;
										  com_data.id_buf[0]=zhiwenid_temp>>8;
											com_data.id_buf[1]=zhiwenid_temp;
										printf(" com_data.id_buf[0]=%d", com_data.id_buf[0]);
										printf(" com_data.id_buf[1]=%d", com_data.id_buf[1]);
                       	Line_1A_WT588S(DIDI);                 										
                           HAL_Delay(500);
										
                       										
									}	else if	(zhiwenid_temp_state==2)
									{
										  	Line_1A_WT588S(FINGER_PRINTS_NOT_RECORDED);                 										
                           HAL_Delay(500);
									    
										
										
									}
				                  HAL_Delay(500);   
									   test_tim_count++;
									  if(test_tim_count>20)
										{
											test_tim_count=0;
											break;
										}
	}
}
//ҵ��״̬��
extern uint8_t net_state;  //����״̬
extern	uint8_t get_time_stramp();
uint8_t power_flag=0;
uint8_t zhiwen_runing_flag=0;
uint16_t power_count=0;
void Talk_Process_Fun()
{
	 
     	switch(System_state)
			{
				//����ģʽ ʱ��
				case CONFIG  :
					    
				break;
					  //��������
				case DEV_NET_IN : 
					      if(eth_ring_net_in()!=1)
								{
								
									
								}
								else
								{
									 get_time_stramp(0);
									  HAL_Delay(5);
									System_state=WORKING;
									Business_flow=DEV_XINTIAO; //����������
								}
				
				  break;
				case WORKING :
					        if(rc522_find_card(com_data.card_buf))         //���� ����ָ��
									{
                       	Line_1A_WT588S(DIDI);
                  										
                           HAL_Delay(500);
												Business_flow=CARD_DAKA; //���Ϳ���	
                              if(net_state==1)  //�����쳣
																{
																	 	Line_1A_WT588S(CLOCK_IN_SUCESS);	
																}
																else
																{										
                       	 Line_1A_WT588S(CARD_NUM_BEING_VERIFIED);	
																}
                           HAL_Delay(500);										
									}
									if(power_flag==0)
									{
										
										 	if(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)) //����
									   {
									  	power_flag=1;
										 open_zhiwen_vol();//�򿪵�Դ
											 
  									   HAL_Delay(500);

									   }
										 else if((zhiwen_cur==ZHIWEN_IDLE)&&(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)==0))
										 {
											 	 close_zhiwen_vol();
										 }
								}
									if(power_flag==1)
									{
								  	zhiwenid_temp_state=press_FR(&zhiwenid_temp);
				         	    if(zhiwenid_temp_state==1)         //�ҵ�ָ��
								   	 {
										//	System_Music=DIDI;
										  com_data.id_buf[0]=zhiwenid_temp>>8;
											com_data.id_buf[1]=zhiwenid_temp;
									//	printf(" com_data.id_buf[0]=%d", com_data.id_buf[0]);
									//	printf(" com_data.id_buf[1]=%d", com_data.id_buf[1]);
                       	Line_1A_WT588S(DIDI);                 										
                           HAL_Delay(500);
												Business_flow=ZHIWEN_DAKA; //���Ϳ���		
										       		  if(net_state==1)  //�����쳣
																{
																	 	Line_1A_WT588S(CLOCK_IN_SUCESS);	
																}
																else
																{
                       	Line_1A_WT588S(CARD_NUM_BEING_VERIFIED);	
																}
                           HAL_Delay(500);	
                          power_flag=2;	
                   															
									}	else if	(zhiwenid_temp_state==2)
									{
										  	Line_1A_WT588S(FINGER_PRINTS_NOT_RECORDED);                 										
                           HAL_Delay(500);
									       power_flag=2;
																		
									}
              	 	if(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)) //����
									   {
									 

									   }
										 else if((zhiwen_cur==ZHIWEN_IDLE)&&(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)==0))
										 {
											    power_flag=0;
											 	 close_zhiwen_vol();
										 }
								 }
								 else if(power_flag==2)
								 {
									 
									 	 	if(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)) //����
									   {
									 

									   }
										 else if((zhiwen_cur==ZHIWEN_IDLE)&&(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)==0))
										 {
											    power_flag=0;
											 	 close_zhiwen_vol();
										 }
									 
								 }

					break;
												
		 }
}


extern uint32_t  local_timestamp;

uint8_t net_state=0;  //����״̬
uint32_t net_time_count=0;
uint8_t net_time_flag=0;
void Eth_Com_Data_Process_hal()
{
//	 	printf("ZHIWEN_DATA_CMD88");
	   if((ether_st.RX_flag==1)&&(System_state==WORKING))
			 {
		//		 	printf("ZHIWEN_DATA_CMD1");
				 //�жϰ�ͷ��β
				 if((ether_st.RX_pData[20]==Manufacturer_ID_1)&&(ether_st.RX_pData[21]==Manufacturer_ID_2)&&
					 (ether_st.RX_pData[22]==com_data.dev_buffer[0])&& (ether_st.RX_pData[23]==com_data.dev_buffer[1])
				 &&(ether_st.RX_pData[24]==device_type))
				 {
		//			 	printf("ZHIWEN_DATA_CMD2");
					      switch(ether_st.RX_pData[25])
								{
									case CARD_DAKA_CMD :
										          if(ether_st.RX_pData[28]==1)  
															{
																   printf("�򿨳ɹ�\r\n");
																   System_Music=CARD_PUNCHED_OUT_SUCCESS; //�򿨳ɹ�
																
															}
															else
															{
																
																   System_Music=CARD_NUMBER_NOT_ENTER;  //����û��¼��
																
															}
										break;
									
									case ZHIWEN_DAKA_CMD :
										      if(ether_st.RX_pData[28]==1)  
															{
																   printf("ָ�ƴ򿨳ɹ�\r\n");
																   System_Music=FINGER_PRINT_PUNCHED_IN_SUCCESS; //�򿨳ɹ�
																
															}
															else
															{
																
																  // System_Music=CARD_NUMBER_NOT_ENTER;  //����û��¼��
																
															}
										
										break;
									case XINTIAO_CMD :
										           net_state=0;   //��������
															net_time_flag=0; // ֹͣ����
									  break;
									 case ALL_UPDATA_ZHIWEN_CMD :
										         //��ָ�Ƶ�Դ
										               zhiwen_runing_flag=1;
									                open_zhiwen_vol();//�򿪵�Դ
											 
  									               HAL_Delay(200);  //��ָ�Ƶ�Դ
									 
										               zhiwen_cur=DEL_ALL_ZHIWEN;
										break;
										 case ONE_UPDATA_ZHIWEN_CMD :
										                  //��ָ�Ƶ�Դ
										               zhiwen_runing_flag=1;
									                open_zhiwen_vol();//�򿪵�Դ
											 
  									               HAL_Delay(200);  //��ָ�Ƶ�Դ
										                zhiwen_cur=DEL_ONE_ZHIWEN;
										               for(uint16_t i=0;i<2;i++)
										           {
											              com_data.id_buf[i]=ether_st.RX_pData[28+i];
										           }
										         
										break;
											case ZHIWEN_DATA_CMD :
											 	printf("ZHIWEN_DATA_CMD1");
											           //��ָ�Ƶ�Դ
										               zhiwen_runing_flag=1;
									                open_zhiwen_vol();//�򿪵�Դ
											 
  									               HAL_Delay(200);  //��ָ�Ƶ�Դ
												       for(uint16_t i=0;i<2;i++)
										           {
											              com_data.id_buf[i]=ether_st.RX_pData[28+i];
										           }
															     for(uint16_t i=0;i<768;i++)
										           {
											              com_data.zhiwen_data_one_size[i]=ether_st.RX_pData[28+2+i];
										           }
												           zhiwen_cur=WRITE_ONE_ZHIWEN;
										           
										         
										break;
										  case ZHIWEN_UPDATA_FISNISH_CMD :
											          //	zhiwen_cur=WRITE_ONE_ZHIWEN;
										              Zhiwen_flow=UPDATA_DATA_FISNISH;  
										         
										break;
													 //����
										  case VOICE_MUTE_CMD :
												   
											      	Line_1A_WT588S(ether_st.RX_pData[28]);
				                       send_string_to_eth(ether_st.RX_pData,ether_st.RX_Size);        
									  break;
											
																 //ʱ��У׼
										  case GET_TIMESTAMP_CMD :
												   
											      	      for(uint16_t i=0;i<4;i++)
										           {
											              com_data.timestamp[3-i]=ether_st.RX_pData[28+i];  //��˴洢  ת��ΪС�˴洢
										           }
													 memcpy(&local_timestamp, com_data.timestamp ,sizeof(com_data.timestamp));   //ͬ��ʱ��
				                       send_string_to_eth(ether_st.RX_pData,ether_st.RX_Size);        
									  break;
								}
					 
					 
				 }
				    ether_st.RX_Size=0;											
				    ether_st.RX_flag=0;				
  	  memset(ether_st.RX_pData, 0, sizeof(ether_st.RX_pData));					 
			 }
								  
}
uint16_t zhiwen_len_temp=768;
//ͬ��ָ��״̬��
void Updata_ZhiWen_Data()
{
	  uint16_t zhiwenid=0;
   	uint8_t  ensure=0x55;
	  zhiwenid=com_data.id_buf[0]<<8|com_data.id_buf[1];
	  switch(zhiwen_cur)
		{
			
			  //д�����ݵ�ָ��
			case WRITE_ONE_ZHIWEN :
				           
           export_in_FR(CharBuffer2,com_data.zhiwen_data_one_size,&zhiwen_len_temp);   		//	PS_DownChar(0,buf,&down_len); //ͬ��ָ�Ƶ�ָ��ģ��
                     PS_StoreChar(CharBuffer2,zhiwenid);//              PS_StoreChar(0,id);	
			      System_Music=FINGER_PRINT_WRITE_SUCCESS;
			//����Ӧ���������
		//	״̬��
			        com_data.zhiwen_data_updata[0]=0x01;
	         	
			        Zhiwen_flow=ZHIWEN_DATA_SEND; 
                 zhiwen_cur=ZHIWEN_IDLE;			
			 break;
			
			//����ָ��
				case EXPORT_OUT_ONE_ZHIWEN:
				
           export_out_FR(zhiwenid,com_data.zhiwen_data_one_size,768);   		//	PS_DownChar(0,buf,&down_len); //ͬ��ָ�Ƶ�ָ��ģ��
             	  zhiwen_cur=ZHIWEN_IDLE;
			//����Ӧ���������
		//	״̬��
	  //  	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
			
			 break;
								//ɾ������ָ��
				case  DEL_ALL_ZHIWEN:
		       	ensure=PS_Empty();//���ָ�ƿ�
             	if(ensure==0)
	         {	
						 com_data.all_updata_zhiwen_data[0]=0x01;
		    printf("Delete All_fingerprint success!!!");//ȫ��ɾ��ָ�Ƴɹ�		
	         }
					 else
					 {
						 com_data.all_updata_zhiwen_data[0]=0x00;
						 
					 }
		//	״̬��
					 
	    	    Zhiwen_flow   =	UPDATA_ALL_ZHIWEN;
			        	  zhiwen_cur=ZHIWEN_IDLE;
			 break;
					 						
			 case  DEL_ONE_ZHIWEN :
				 
		       ensure=PS_DeletChar(zhiwenid,1);//ɾ������ָ��
             	if(ensure==0)
	         {	
		    printf("Delete One_fingerprint success!!!");//ɾ������ָ�Ƴɹ�		
	         }
		//	״̬��
	    	    Zhiwen_flow =	UPDATA_ONE_ZHIWEN;
			         	  zhiwen_cur=ZHIWEN_IDLE;
			 break;
							
		}
}
	

