#ifndef  __MAIN_APP_H__
#define  __MAIN_APP_H__




typedef enum  {
	   CONFIG=0, //配置模式
	   DEV_NET_IN,  //入网模式
	   WORKING  //工作模式
		
} System_State_Type;

//声音
typedef enum  {
   MUTE_VOICE=0, //静音
	 CLOCK_IN_SUCESS,  //打卡成功
	 CARD_NUMBER_NOT_ENTER,//卡号没有录入 
	 FINGER_SUCCESS_RECORDED,//指纹录入成功
   FINGER_PRINT_DELETED_SUCCESS,//指纹删除成功
	 FINGER_PRINT_PUNCHED_IN_SUCCESS,  //指纹打卡成功
   FINGER_PRINTS_NOT_RECORDED,		//指纹没有录入	
	 SYNCHRONIZING_DATA_SUCCESS,   //同步数据成功
   START_SYNCHRON_DATA,	//开始同步数据
	 FINGER_PRINT_WRITE_SUCCESS,//写入指纹成功
	 DIDI,  //滴滴
	 LOW_BATTERY_POWER,//电池电量不足
   ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS,		//接入网关成功，分配地址成功
   SET_WORK,		//开始工作
   START_RECORD_FINGER_ONCE,  //开始录入指纹一次
	 START_RECORD_FINGER_TWICE,  //   //开始录入指纹二次
   CARD_NUM_BEING_VERIFIED,	//卡号正在校验
	 CARD_PUNCHED_OUT_SUCCESS,  //卡片打卡成功
   PLEASE_RECORD_FINGER_PRINT,		//请录入指纹
   FINGER_PRINT_RECORD_SUCCESS  //录入指纹成功	
} System_Music_Type;
//指纹操作
//typedef enum  {
//  
//} System_Music_Type;
typedef enum{
	 WRITE_ONE_ZHIWEN=0U,   //写入指纹
	 EXPORT_OUT_ONE_ZHIWEN=1,  //导出指纹
	 DEL_ONE_ZHIWEN=2,     //删除单个指纹
   DEL_ALL_ZHIWEN=3,     //删除所有指纹
	 ZHIWEN_IDLE 
}zhiwen_cur_num;
extern zhiwen_cur_num zhiwen_cur;
extern System_Music_Type   System_Music;
extern void Play_Music_Fun(void);
extern void Talk_Process_Fun(void);
#endif
