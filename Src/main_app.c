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
//ҵ���
System_State_Type   System_state=DEV_NET_IN;
System_Music_Type   System_Music;
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



//ҵ��״̬��

void Talk_Process_Fun()
{
	 
     	switch(System_state)
			{
				//����ģʽ
				case CONFIG  :
					
				
				break;
					
				case DEV_NET_IN : 
					      if(eth_ring_net_in()!=1)
								{
								
									
								}
								else
								{
									System_state=WORKING;
								}
				
				  break;
				case WORKING :
					        if(rc522_find_card(com_data.card_buf))         //���� ����ָ��
									{
											System_Music=DIDI;
									 	Business_flow=CARD_DAKA; //���Ϳ���										
									}
				         				
					break;
												
		 }
}

void test_zhiwen()
{
	//switch()
//	Add_FR(1);
	
	
}
//ͬ��ָ��״̬��
//void Updata_ZhiWen_Data()
//{
//	
//	  switch()
//		{
//			
//			  //д�����ݵ�ָ��
//			case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //ͬ��ָ�Ƶ�ָ��ģ��
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//����Ӧ���������
//		//	״̬��
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//			
//			//����ָ��
//				case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //ͬ��ָ�Ƶ�ָ��ģ��
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//����Ӧ���������
//		//	״̬��
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//				
//							//ɾ��ȫ��ָ��
//				case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //ͬ��ָ�Ƶ�ָ��ģ��
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//����Ӧ���������
//		//	״̬��
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//								//ɾ������ָ��
//				case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //ͬ��ָ�Ƶ�ָ��ģ��
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//����Ӧ���������
//		//	״̬��
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//			
//			
//			
			
			
			
			
			
			
			
//		}
	
	
	
	
	
	
	
	
	
	
	
	
	
//}
