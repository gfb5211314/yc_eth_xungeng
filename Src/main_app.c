#include "main_app.h"
#include "SX127X_Driver.h"
#include "SX127X_Hal.h"
#include "stdio.h"
#include "bsp_eeprom.h"
#include "string.h"
#include "ether_hal.h"
#include "stm_flash.h"
//业务层
System_State_Type   System_state=DEV_NET_IN;
System_Music_Type   System_Music;

//声音状态机
void Play_Music_Fun()
{
	  	switch(System_Music)
			{
				case MUTE_VOICE : //静音
					break;
				case CLOCK_IN_SUCESS :
					
				break;
				case 	 CARD_NUMBER_NOT_ENTER://卡号没有录入  :
					
				break;
				case  FINGER_SUCCESS_RECORDED://指纹录入成功
					
				break;
				case  FINGER_PRINT_DELETED_SUCCESS://指纹删除成功
					
				break;
				case FINGER_PRINT_PUNCHED_IN_SUCCESS:  //指纹打卡成功
					
				break;
				case FINGER_PRINTS_NOT_RECORDED:		//指纹没有录入
					
				break;
				case SYNCHRONIZING_DATA_SUCCESS: //同步数据成功
					
				break;
				case  START_SYNCHRON_DATA:	//开始同步数据
					
				break;
				case FINGER_PRINT_WRITE_SUCCESS://写入指纹成功
					
				break;
				case  DIDI: //滴滴
					
				break;
					
					case  LOW_BATTERY_POWER://电池电量不足
					
				break;
					
      	case   ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS:		//接入网关成功，分配地址成功
					
				break;
					
        case   SET_WORK:		//开始工作
					
				break;
	      case   START_RECORD_FINGER_ONCE: //开始录入指纹一次
					
				break;
		    case    START_RECORD_FINGER_TWICE:  //   //开始录入指纹二次
					
				break;
			  case   CARD_NUM_BEING_VERIFIED:	//卡号正在校验
					
				break;
				case  CARD_PUNCHED_OUT_SUCCESS:  //卡片打卡成功
					
				break;
						
				case     PLEASE_RECORD_FINGER_PRINT:		//请录入指纹
					
				break;
				case     FINGER_PRINT_RECORD_SUCCESS : //录入指纹成功
					
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
					                //读卡 或者指纹
				         
				
					break;
												
		 }
}
//同步指纹状态机

