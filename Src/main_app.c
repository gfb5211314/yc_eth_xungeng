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
//业务层
System_State_Type   System_state=DEV_NET_IN;
System_Music_Type   System_Music;
uint8_t card_num[16]; //卡号缓存
//声音状态机
void Play_Music_Fun()
{
	  	switch(System_Music)
			{
				case MUTE_VOICE : //静音
//					        if(HAL_GetTick()%2000==0)
//									{
//					    	     Line_1A_WT588S(MUTE_VOICE);
//									}
				         
					break;
				case CLOCK_IN_SUCESS :
					       Line_1A_WT588S(CLOCK_IN_SUCESS);
			         	System_Music=MUTE_VOICE;
				break;
				case 	 CARD_NUMBER_NOT_ENTER://卡号没有录入  :
					       Line_1A_WT588S(CARD_NUMBER_NOT_ENTER);
				System_Music=MUTE_VOICE;
					
				break;
				case  FINGER_SUCCESS_RECORDED://指纹录入成功
					Line_1A_WT588S(FINGER_SUCCESS_RECORDED);
				System_Music=MUTE_VOICE;
					
				break;
				case  FINGER_PRINT_DELETED_SUCCESS://指纹删除成功
					Line_1A_WT588S(FINGER_PRINT_DELETED_SUCCESS);
				System_Music=MUTE_VOICE;
					
				break;
				case FINGER_PRINT_PUNCHED_IN_SUCCESS:  //指纹打卡成功
					Line_1A_WT588S(FINGER_PRINT_PUNCHED_IN_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
				case FINGER_PRINTS_NOT_RECORDED:		//指纹没有录入
					Line_1A_WT588S(System_Music);
				System_Music=MUTE_VOICE;
				break;
				case SYNCHRONIZING_DATA_SUCCESS: //同步数据成功
					Line_1A_WT588S(SYNCHRONIZING_DATA_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
				case  START_SYNCHRON_DATA:	//开始同步数据
					Line_1A_WT588S(START_SYNCHRON_DATA);
				System_Music=MUTE_VOICE;
				break;
				case FINGER_PRINT_WRITE_SUCCESS://写入指纹成功
					Line_1A_WT588S(FINGER_PRINT_WRITE_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
				case  DIDI: //滴滴
					Line_1A_WT588S(DIDI);
				System_Music=MUTE_VOICE;
				break;
					
					case  LOW_BATTERY_POWER://电池电量不足
					Line_1A_WT588S(LOW_BATTERY_POWER);
					System_Music=MUTE_VOICE;
				break;
					
      	case   ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS:		//接入网关成功，分配地址成功
					Line_1A_WT588S(ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
					
        case   SET_WORK:		//开始工作
					Line_1A_WT588S(SET_WORK);
				System_Music=MUTE_VOICE;
				break;
	      case   START_RECORD_FINGER_ONCE: //开始录入指纹一次
					Line_1A_WT588S(START_RECORD_FINGER_ONCE);
				System_Music=MUTE_VOICE;
				break;
		    case    START_RECORD_FINGER_TWICE:  //   //开始录入指纹二次
					Line_1A_WT588S(START_RECORD_FINGER_TWICE);
				System_Music=MUTE_VOICE;
				break;
			  case   CARD_NUM_BEING_VERIFIED:	//卡号正在校验
						Line_1A_WT588S(CARD_NUM_BEING_VERIFIED);
			 	System_Music=MUTE_VOICE;
				break;
				case  CARD_PUNCHED_OUT_SUCCESS:  //卡片打卡成功
					Line_1A_WT588S(CARD_PUNCHED_OUT_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
						
				case     PLEASE_RECORD_FINGER_PRINT:		//请录入指纹
					Line_1A_WT588S(PLEASE_RECORD_FINGER_PRINT);
				System_Music=MUTE_VOICE;
				break;
				case     FINGER_PRINT_RECORD_SUCCESS : //录入指纹成功
					Line_1A_WT588S(FINGER_PRINT_RECORD_SUCCESS);
				System_Music=MUTE_VOICE;
				break;
		}	
}	



//业务状态机

void Talk_Process_Fun()
{
	 
     	switch(System_state)
			{
				//配置模式
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
					        if(rc522_find_card(com_data.card_buf))         //读卡 或者指纹
									{
											System_Music=DIDI;
									 	Business_flow=CARD_DAKA; //发送卡号										
									}
				         				
					break;
												
		 }
}

void test_zhiwen()
{
	//switch()
//	Add_FR(1);
	
	
}
//同步指纹状态机
//void Updata_ZhiWen_Data()
//{
//	
//	  switch()
//		{
//			
//			  //写入数据到指纹
//			case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //同步指纹到指纹模组
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//发送应答给服务器
//		//	状态机
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//			
//			//导出指纹
//				case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //同步指纹到指纹模组
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//发送应答给服务器
//		//	状态机
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//				
//							//删除全部指纹
//				case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //同步指纹到指纹模组
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//发送应答给服务器
//		//	状态机
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//								//删除单个指纹
//				case :
//				
//        export_in_FR(0,buf,&down_len);   		//	PS_DownChar(0,buf,&down_len); //同步指纹到指纹模组
//         PS_StoreChar(0,id);//              PS_StoreChar(0,id);
//			//发送应答给服务器
//		//	状态机
//	    	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
//			
//			 break;
//			
//			
//			
			
			
			
			
			
			
			
//		}
	
	
	
	
	
	
	
	
	
	
	
	
	
//}
