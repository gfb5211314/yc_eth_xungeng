#include "main_app.h"
#include "SX127X_Driver.h"
#include "SX127X_Hal.h"
#include "stdio.h"
#include "bsp_eeprom.h"
#include "string.h"
#include "ether_hal.h"
#include "stm_flash.h"
//ҵ���
System_State_Type   System_state=DEV_NET_IN;
System_Music_Type   System_Music;

//����״̬��
void Play_Music_Fun()
{
	  	switch(System_Music)
			{
				case MUTE_VOICE : //����
					break;
				case CLOCK_IN_SUCESS :
					
				break;
				case 	 CARD_NUMBER_NOT_ENTER://����û��¼��  :
					
				break;
				case  FINGER_SUCCESS_RECORDED://ָ��¼��ɹ�
					
				break;
				case  FINGER_PRINT_DELETED_SUCCESS://ָ��ɾ���ɹ�
					
				break;
				case FINGER_PRINT_PUNCHED_IN_SUCCESS:  //ָ�ƴ򿨳ɹ�
					
				break;
				case FINGER_PRINTS_NOT_RECORDED:		//ָ��û��¼��
					
				break;
				case SYNCHRONIZING_DATA_SUCCESS: //ͬ�����ݳɹ�
					
				break;
				case  START_SYNCHRON_DATA:	//��ʼͬ������
					
				break;
				case FINGER_PRINT_WRITE_SUCCESS://д��ָ�Ƴɹ�
					
				break;
				case  DIDI: //�ε�
					
				break;
					
					case  LOW_BATTERY_POWER://��ص�������
					
				break;
					
      	case   ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS:		//�������سɹ��������ַ�ɹ�
					
				break;
					
        case   SET_WORK:		//��ʼ����
					
				break;
	      case   START_RECORD_FINGER_ONCE: //��ʼ¼��ָ��һ��
					
				break;
		    case    START_RECORD_FINGER_TWICE:  //   //��ʼ¼��ָ�ƶ���
					
				break;
			  case   CARD_NUM_BEING_VERIFIED:	//��������У��
					
				break;
				case  CARD_PUNCHED_OUT_SUCCESS:  //��Ƭ�򿨳ɹ�
					
				break;
						
				case     PLEASE_RECORD_FINGER_PRINT:		//��¼��ָ��
					
				break;
				case     FINGER_PRINT_RECORD_SUCCESS : //¼��ָ�Ƴɹ�
					
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
					                //���� ����ָ��
				         
				
					break;
												
		 }
}
//ͬ��ָ��״̬��

