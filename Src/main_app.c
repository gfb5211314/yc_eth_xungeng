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
//业务层
System_State_Type   System_state=DEV_NET_IN; //  DEV_NET_IN
System_Music_Type   System_Music;
zhiwen_cur_num      zhiwen_cur=ZHIWEN_IDLE;
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
uint16_t zhiwenid_temp;
uint8_t zhiwenid_temp_state=0;
 extern COM_DATA_TYPE   com_data;
uint8_t  test_tim_count=0;

void Test_Dev_Code()
{
    while(1)
{
  if(rc522_find_card(com_data.card_buf))         //读卡 或者指纹
									{
										//	System_Music=DIDI;
										   											
                       	Line_1A_WT588S(DIDI);
                  										
                           HAL_Delay(500);
																		
									}
									zhiwenid_temp_state=press_FR(&zhiwenid_temp);
				         	  if(zhiwenid_temp_state==1)         //找到指纹
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
//业务状态机
extern uint8_t net_state;  //网络状态
extern	uint8_t get_time_stramp();
uint8_t power_flag=0;
uint8_t zhiwen_runing_flag=0;
uint16_t power_count=0;
void Talk_Process_Fun()
{
	 
     	switch(System_state)
			{
				//配置模式 时间
				case CONFIG  :
					    
				break;
					  //入网流程
				case DEV_NET_IN : 
					      if(eth_ring_net_in()!=1)
								{
								
									
								}
								else
								{
									 get_time_stramp(0);
									  HAL_Delay(5);
									System_state=WORKING;
									Business_flow=DEV_XINTIAO; //发送心跳包
								}
				
				  break;
				case WORKING :
					        if(rc522_find_card(com_data.card_buf))         //读卡 或者指纹
									{
                       	Line_1A_WT588S(DIDI);
                  										
                           HAL_Delay(500);
												Business_flow=CARD_DAKA; //发送卡号	
                              if(net_state==1)  //网络异常
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
										
										 	if(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)) //新增
									   {
									  	power_flag=1;
										 open_zhiwen_vol();//打开电源
											 
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
				         	    if(zhiwenid_temp_state==1)         //找到指纹
								   	 {
										//	System_Music=DIDI;
										  com_data.id_buf[0]=zhiwenid_temp>>8;
											com_data.id_buf[1]=zhiwenid_temp;
									//	printf(" com_data.id_buf[0]=%d", com_data.id_buf[0]);
									//	printf(" com_data.id_buf[1]=%d", com_data.id_buf[1]);
                       	Line_1A_WT588S(DIDI);                 										
                           HAL_Delay(500);
												Business_flow=ZHIWEN_DAKA; //发送卡号		
										       		  if(net_state==1)  //网络异常
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
              	 	if(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)) //新增
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
									 
									 	 	if(HAL_GPIO_ReadPin(TP_WKUP_GPIO_Port, TP_WKUP_Pin)) //新增
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

uint8_t net_state=0;  //网络状态
uint32_t net_time_count=0;
uint8_t net_time_flag=0;
void Eth_Com_Data_Process_hal()
{
//	 	printf("ZHIWEN_DATA_CMD88");
	   if((ether_st.RX_flag==1)&&(System_state==WORKING))
			 {
		//		 	printf("ZHIWEN_DATA_CMD1");
				 //判断包头包尾
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
																   printf("打卡成功\r\n");
																   System_Music=CARD_PUNCHED_OUT_SUCCESS; //打卡成功
																
															}
															else
															{
																
																   System_Music=CARD_NUMBER_NOT_ENTER;  //卡号没有录入
																
															}
										break;
									
									case ZHIWEN_DAKA_CMD :
										      if(ether_st.RX_pData[28]==1)  
															{
																   printf("指纹打卡成功\r\n");
																   System_Music=FINGER_PRINT_PUNCHED_IN_SUCCESS; //打卡成功
																
															}
															else
															{
																
																  // System_Music=CARD_NUMBER_NOT_ENTER;  //卡号没有录入
																
															}
										
										break;
									case XINTIAO_CMD :
										           net_state=0;   //网络正常
															net_time_flag=0; // 停止计数
									  break;
									 case ALL_UPDATA_ZHIWEN_CMD :
										         //打开指纹电源
										               zhiwen_runing_flag=1;
									                open_zhiwen_vol();//打开电源
											 
  									               HAL_Delay(200);  //打开指纹电源
									 
										               zhiwen_cur=DEL_ALL_ZHIWEN;
										break;
										 case ONE_UPDATA_ZHIWEN_CMD :
										                  //打开指纹电源
										               zhiwen_runing_flag=1;
									                open_zhiwen_vol();//打开电源
											 
  									               HAL_Delay(200);  //打开指纹电源
										                zhiwen_cur=DEL_ONE_ZHIWEN;
										               for(uint16_t i=0;i<2;i++)
										           {
											              com_data.id_buf[i]=ether_st.RX_pData[28+i];
										           }
										         
										break;
											case ZHIWEN_DATA_CMD :
											 	printf("ZHIWEN_DATA_CMD1");
											           //打开指纹电源
										               zhiwen_runing_flag=1;
									                open_zhiwen_vol();//打开电源
											 
  									               HAL_Delay(200);  //打开指纹电源
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
													 //静音
										  case VOICE_MUTE_CMD :
												   
											      	Line_1A_WT588S(ether_st.RX_pData[28]);
				                       send_string_to_eth(ether_st.RX_pData,ether_st.RX_Size);        
									  break;
											
																 //时间校准
										  case GET_TIMESTAMP_CMD :
												   
											      	      for(uint16_t i=0;i<4;i++)
										           {
											              com_data.timestamp[3-i]=ether_st.RX_pData[28+i];  //大端存储  转换为小端存储
										           }
													 memcpy(&local_timestamp, com_data.timestamp ,sizeof(com_data.timestamp));   //同步时间
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
//同步指纹状态机
void Updata_ZhiWen_Data()
{
	  uint16_t zhiwenid=0;
   	uint8_t  ensure=0x55;
	  zhiwenid=com_data.id_buf[0]<<8|com_data.id_buf[1];
	  switch(zhiwen_cur)
		{
			
			  //写入数据到指纹
			case WRITE_ONE_ZHIWEN :
				           
           export_in_FR(CharBuffer2,com_data.zhiwen_data_one_size,&zhiwen_len_temp);   		//	PS_DownChar(0,buf,&down_len); //同步指纹到指纹模组
                     PS_StoreChar(CharBuffer2,zhiwenid);//              PS_StoreChar(0,id);	
			      System_Music=FINGER_PRINT_WRITE_SUCCESS;
			//发送应答给服务器
		//	状态机
			        com_data.zhiwen_data_updata[0]=0x01;
	         	
			        Zhiwen_flow=ZHIWEN_DATA_SEND; 
                 zhiwen_cur=ZHIWEN_IDLE;			
			 break;
			
			//导出指纹
				case EXPORT_OUT_ONE_ZHIWEN:
				
           export_out_FR(zhiwenid,com_data.zhiwen_data_one_size,768);   		//	PS_DownChar(0,buf,&down_len); //同步指纹到指纹模组
             	  zhiwen_cur=ZHIWEN_IDLE;
			//发送应答给服务器
		//	状态机
	  //  	 Zhiwen_flow   =	UPDATA_ONE_ZHIWEN;
			
			 break;
								//删除所有指纹
				case  DEL_ALL_ZHIWEN:
		       	ensure=PS_Empty();//清空指纹库
             	if(ensure==0)
	         {	
						 com_data.all_updata_zhiwen_data[0]=0x01;
		    printf("Delete All_fingerprint success!!!");//全部删除指纹成功		
	         }
					 else
					 {
						 com_data.all_updata_zhiwen_data[0]=0x00;
						 
					 }
		//	状态机
					 
	    	    Zhiwen_flow   =	UPDATA_ALL_ZHIWEN;
			        	  zhiwen_cur=ZHIWEN_IDLE;
			 break;
					 						
			 case  DEL_ONE_ZHIWEN :
				 
		       ensure=PS_DeletChar(zhiwenid,1);//删除单个指纹
             	if(ensure==0)
	         {	
		    printf("Delete One_fingerprint success!!!");//删除单个指纹成功		
	         }
		//	状态机
	    	    Zhiwen_flow =	UPDATA_ONE_ZHIWEN;
			         	  zhiwen_cur=ZHIWEN_IDLE;
			 break;
							
		}
}
	

