#include "as608_app.h"
#include "as608.h"  

typedef unsigned          char u8;
typedef unsigned short     int u16;

//
#define usart2_baund  57600//

SysPara AS608Para;//
u16 ValidN;
u8** kbd_tbl;

uint8_t Add_FR(uint16_t shouid);	//
void Del_FR(void);	//
uint8_t  press_FR(uint16_t *pageid);
void ShowErrMessage(u8 ensure);//
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl);//
u8  AS608_get_keynum(u16 x,u16 y);//
//u16 GET_NUM(void);//
//�Ӵ�������ȡָ�Ʋ���������Ȼ���ϴ�����λ��

//��ָ�Ƶ�Դ
void open_zhiwen_vol()
{
	
	 HAL_GPIO_WritePin(Touch_Pwr_GPIO_Port, Touch_Pwr_Pin, GPIO_PIN_RESET);
	
}
void close_zhiwen_vol()
{
	
	 HAL_GPIO_WritePin(Touch_Pwr_GPIO_Port, Touch_Pwr_Pin, GPIO_PIN_SET);
	
}
//�������ܣ��������¼��ָ�Ʋ���������ģ�����Buffer1��Buffer2
void Create_FR_Char(void)
{
	uint8_t i,ensure ,processnum=0;
	uint16_t ID_NUM=0;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);
					if(ensure==0x00)
					{
						printf("succrss1");
						i=0;
						processnum=1;//�����ڶ���
					}else {};
				}else {};
			break;
					
			case 1:
				i++;
			
  				HAL_Delay(100);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//????
					if(ensure==0x00)
					{
//						Delay_Ms(1);
					  printf("succrss2");
    				HAL_Delay(10);
						i=0;
						processnum=2;//����������
					}else {};	
					}else {};		
				break;
						
				case 2:
//				LCD_show(0x00,"Creating Press");
				HAL_Delay(500);
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
		         printf("succrss3");
					return ;
				}
				else
					{
						
						processnum=0;//���ص�һ��
					}
		HAL_Delay(1000);
				break;
					}
		if(i==10)//����5��δ����ָ�˳�
			break;
	}		
}
void  zhiwen_data_to_computer()
{
	         static uint8_t  text_zhiwen=0;
	  
	   switch(text_zhiwen)
		 {
			 
			 	case 0 :  
				 Create_FR_Char();
				       text_zhiwen=5;   
				break;
//				 if(PS_GetImage()==0) 
//			 {
//				     printf("��ȡ��ָ�ɹ�\r\n");
//				      text_zhiwen=1;
//				 
//			 }
//			 break;
//       	case 1 :  
//			
//				 if(PS_GenChar(CharBuffer1)==0) 
//			 {
//				    printf("ָ����ȷ\r\n");//ָ����ȷ
//				 text_zhiwen=2;
//				 
//			 }
//			 break;          
//			 case 2 : 

//				 if(PS_GetImage()==0) 
//			 {
//				     printf("��ȡ��ָ�ɹ�\r\n");
//				      text_zhiwen=3;
//			 }  break;
//   	case 3 :  
//			
//				 if(PS_GenChar(CharBuffer2)==0) 
//			 {
//				    printf("ָ����ȷ\r\n");//ָ����ȷ
//				 text_zhiwen=4;
//				 
//			 }
//			 break; 
//			   	case 4 : 
//						
//					
//		
//				if(PS_RegModel()==0x00) 
//				{
//							 text_zhiwen=5;
//				}
//				break;
					case 5 : 
				   if(PS_UpChar(CharBuffer1)==0) 
			    {
				     printf("��ʼ��������\r\n");
				   text_zhiwen=6;
				 
			    }
			 break;
			 case 6 :  
//      printf("��������\r\n");
			 if(AS608_USART_ST.RX_flag==1)//���յ�һ������
				        {
									printf("<ether_st.RX_Size=%d\r\n",AS608_USART_ST.RX_Size);
									   for(uint16_t i=0;i<AS608_USART_ST.RX_Size;i++)
									{
										
										  printf("ether_st.RX_pData[%d]=%02x\r\n",i,AS608_USART_ST.RX_pData[i]);
										AS608_USART_ST.RX_flag=0;
									}
									
								}
			 
			 
			 
		 }
	
	
	
	
}
void open_pwr_zhiwen()
{
	
	  HAL_GPIO_WritePin(GPIOC, Touch_Pwr_Pin, GPIO_PIN_SET);
	
}
uint8_t speak_voe=0;
//¼ָ��
uint8_t Add_FR(uint16_t shouid)
{
	uint8_t sate=0;
	u8 i=0,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
			      if(speak_voe==0)
						{  
							  Line_1A_WT588S(19);//
		              HAL_Delay(2000);
			          Line_1A_WT588S(14);//��ʼ¼��ָ�Ƶ�һ��
		            HAL_Delay(2000);
						  	speak_voe=1;
						}
				//printf("�밴��ָ\r\n");//�밴��ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//��������
					if(ensure==0x00)
					{
					   speak_voe=0;
					//	printf("ָ����ȷ/r/n");//ָ����ȷ
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
			   if(speak_voe==0)
						{
			       Line_1A_WT588S(15);//��ʼ¼��ָ�Ƶڶ���
		            HAL_Delay(2000);
						  	speak_voe=1;
						}
		//	printf("���ٰ�һ����ָ\r\n")	;//�ٰ�һ����ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������			
					if(ensure==0x00)
					{
					       speak_voe=0;
						//	printf("ָ����ȷ/r/n");//ָ����ȷ
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				
				//	printf("�Ա�2��ָ��/r/n");//�Ա�����ָ��
				ensure=PS_Match();
				if(ensure==0x00) 
				{
			
				//	printf("2��ָ��һ����/r/n");//����ָ����һ����
					processnum=3;//�������Ĳ�
				}
				else 
				{
					 speak_voe=0;
					  Line_1A_WT588S(6);//
		        HAL_Delay(1000);
			//	printf("�Ա�ʧ��,�����°���ָ/r/n");//�Ա�ʧ�ܣ������°���ָ
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				break;

			case 3:
			
			//	printf("����һ��ָ��ģ��/r/n");;//����һ��ָ��ģ��
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					
			//		printf("����ָ��ģ��ɹ�/r/n");;//����ָ��ģ��ɹ�
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
//				HAL_Delay(1000);
				break;
				
			case 4:	
			  
				printf("������ID\r\n");	;//����ID������Enter������
				printf("0=< ID <=299\r\n");	
				do
//        			ID=GET_NUM();
				ID=shouid;
				while(!(ID<300));//����DI����С��300
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
					     sate=1;
							  Line_1A_WT588S(20);//¼��ָ�Ƴɹ�
		            HAL_Delay(1500);		
					//	printf("���ָ�Ƴɹ�/r/n");//���ָ�Ƴɹ�
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
			//		printf("ʣ��ָ�Ƹ���:%d\r\n"	,AS608Para.PS_max-ValidN);//��ʾʣ��ָ�Ƹ���
//					HAL_Delay(1500);//��ʱ�������ʾ	
				
				return sate;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		HAL_Delay(50);
		if(i==20)//����5��û�а���ָ���˳�
		{
			 sate=0;
			break;	
		}				
	}
	
}

//ˢָ��
uint8_t  press_FR(uint16_t *pageid)
{
	SearchResult seach;
	u8 state=0;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	

		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
//			printf("���������ɹ� ");
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
//			       Line_1A_WT588S(5);//ָ�ƴ򿨳ɹ�
//		           HAL_Delay(2000);
			//	printf("����ָ�Ƴɹ�\r\n");//����ָ�Ƴɹ�				
				pageid[0]=seach.pageID;
			//	printf("Match ID:%d  Match score:%d\r\n",seach.pageID,seach.mathscore);//��ʾƥ��ָ�Ƶ�ID�ͷ���
          state=1;
			}
			else 
				 if(ensure==0x09)
				 {
					 		 state=0;
//			      Line_1A_WT588S(6);//û���ҵ�ָ��
//		            HAL_Delay(2000);
			//		printf("û��������ָ��\r\n");//����ָ�Ƴɹ�	
				ShowErrMessage(ensure);		
				 }					 
	  }
		else
			ShowErrMessage(ensure);
//	    HAL_Delay(500);//��ʱ�������ʾ
	}
	  return  	state;
}
void check_zhiwen_nomal()
{  
		u8 ensure;
	 u8 str[20];
		while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		printf("ָ��ģ�����\r\n");
	}
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //��AS608ģ����� 
	if(ensure==0x00)
	{
	//	mymemset(str,0,50);
		//sprintf(str,"RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//��ʾʣ��ָ�������Ͱ�ȫ�ȼ�
	//	printf("%s\r\n",str);
	}
	else
		ShowErrMessage(ensure);
	
}
//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 num;
 
	printf("��ʾɾ��ָ��");//��ʾɾ��ָ��
	printf("Input ID and touch Enter!");//��ʾ����ID�����¡�Enter��
	printf("0=< ID <=299");
	HAL_Delay(50);
//	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
//	num=GET_NUM();//��ȡ���ص���ֵ
 if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
	
		printf("Delete fingerprint success!!!");//ɾ��ָ�Ƴɹ�		
	}
  else
		ShowErrMessage(ensure);	
	HAL_Delay(1500);//��ʱ�������ʾ
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	printf("��ʾʣ��ָ�Ƹ���=%d\r\n",AS608Para.PS_max-ValidN);//��ʾʣ��ָ�Ƹ���
}


//

//ָ�������ϴ�

