#ifndef  __MAIN_APP_H__
#define  __MAIN_APP_H__




typedef enum  {
	   CONFIG=0, //����ģʽ
	   DEV_NET_IN,  //����ģʽ
	   WORKING  //����ģʽ
		
} System_State_Type;

//����
typedef enum  {
   MUTE_VOICE=0, //����
	 CLOCK_IN_SUCESS,  //�򿨳ɹ�
	 CARD_NUMBER_NOT_ENTER,//����û��¼�� 
	 FINGER_SUCCESS_RECORDED,//ָ��¼��ɹ�
   FINGER_PRINT_DELETED_SUCCESS,//ָ��ɾ���ɹ�
	 FINGER_PRINT_PUNCHED_IN_SUCCESS,  //ָ�ƴ򿨳ɹ�
   FINGER_PRINTS_NOT_RECORDED,		//ָ��û��¼��	
	 SYNCHRONIZING_DATA_SUCCESS,   //ͬ�����ݳɹ�
   START_SYNCHRON_DATA,	//��ʼͬ������
	 FINGER_PRINT_WRITE_SUCCESS,//д��ָ�Ƴɹ�
	 DIDI,  //�ε�
	 LOW_BATTERY_POWER,//��ص�������
   ACCESS_GATE_SUCCEE_ADDRESS_ASS_SUCCESS,		//�������سɹ��������ַ�ɹ�
   SET_WORK,		//��ʼ����
   START_RECORD_FINGER_ONCE,  //��ʼ¼��ָ��һ��
	 START_RECORD_FINGER_TWICE,  //   //��ʼ¼��ָ�ƶ���
   CARD_NUM_BEING_VERIFIED,	//��������У��
	 CARD_PUNCHED_OUT_SUCCESS,  //��Ƭ�򿨳ɹ�
   PLEASE_RECORD_FINGER_PRINT,		//��¼��ָ��
   FINGER_PRINT_RECORD_SUCCESS  //¼��ָ�Ƴɹ�	
} System_Music_Type;
//ָ�Ʋ���
//typedef enum  {
//  
//} System_Music_Type;
typedef enum{
	 WRITE_ONE_ZHIWEN=0U,   //д��ָ��
	 EXPORT_OUT_ONE_ZHIWEN=1,  //����ָ��
	 DEL_ONE_ZHIWEN=2,     //ɾ������ָ��
   DEL_ALL_ZHIWEN=3,     //ɾ������ָ��
	 ZHIWEN_IDLE 
}zhiwen_cur_num;
extern zhiwen_cur_num zhiwen_cur;
extern System_Music_Type   System_Music;
extern void Play_Music_Fun(void);
extern void Talk_Process_Fun(void);
#endif
