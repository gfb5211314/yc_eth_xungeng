/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "iwdg.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ether_hal.h"
#include "SX127X_Hal.h"
#include "SX127X_Driver.h"
#include "main_app.h"
#include "stdio.h"
#include "string.h"
#include "stm_flash.h"
#include "u_init.h"
#include "net_com.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ETH_XUNGENG_VERSION        "ETH_XUNGENG_V1.03.1"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern void Eth_Com_Data_Process_hal(void);
extern void Updata_ZhiWen_Data(void);
extern void Time_out_Ack_fun(void);
extern uint8_t	get_time_stramp();
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint32_t system_tick=0;
uint32_t system_temp_tick=0;
uint8_t Eth_business_Cammand_flag;
uint8_t Play_Music_Fun_flag;
uint8_t Eth_Com_Data_Process_hal_flag;
uint8_t	Talk_Process_Fun_flag;
uint8_t Eth_Updata_Finger_Cammand_Task_flag;//指纹指令应答
uint8_t	Updata_ZhiWen_Data_flag ;  //操作指纹模组
uint8_t Time_out_Ack_fun_flag;   //超时
uint8_t	get_time_stramp_flag;


volatile uint32_t  tick_cnt=0;
volatile uint32_t  tick_sys=0; //10ms
volatile uint32_t  systick_count=0;

void Sys_Tick_Count()
{
while(tick_sys==0);
if(tick_sys)
{
tick_sys--;
systick_count++;
}
}
uint32_t Get_Sys_tick()
{
return systick_count;
}
void tick_clr()
{
systick_count=0;
//tick_cnt=0;
//tick_sys=0;
}

void test_task()
{

	
	    if(Get_Sys_tick()%100==0)   Eth_business_Cammand_Task();  //业务指令应答
		  if(Get_Sys_tick()%10==0)     Play_Music_Fun();     //播放音乐
		  if(Get_Sys_tick()%100==1)   Eth_Com_Data_Process_hal(); //业务处理
			if(Get_Sys_tick()%50==1)    Talk_Process_Fun(); //业务处理
	  	if(Get_Sys_tick()%50==2) 	  Eth_Updata_Finger_Cammand_Task(); //指纹指令应答
			if((Get_Sys_tick()%10)==1)  Updata_ZhiWen_Data();   //操作指纹模组
      if((Get_Sys_tick()%50)==3)  Time_out_Ack_fun();   //超时
		  if(Get_Sys_tick()%1000==0)  {get_time_stramp(1);get_time_stramp_flag=0;}
        Sys_Tick_Count();

}


void System_Tick_Count()
{
	if(uwTick)
		{
			   
		
		  uwTick--;
			system_tick++;
      if(system_tick%100==0)   Eth_business_Cammand_Task();  //业务指令应答
		  if(system_tick%10==0)     Play_Music_Fun();     //播放音乐
		  if(system_tick%100==1)   Eth_Com_Data_Process_hal(); //业务处理
			if(system_tick%50==1)    Talk_Process_Fun(); //业务处理
	  	if(system_tick%50==1) 	 Eth_Updata_Finger_Cammand_Task(); //指纹指令应答
			if((system_tick%10)==1)  Updata_ZhiWen_Data();   //操作指纹模组
      if((system_tick%50)==1)  Time_out_Ack_fun();   //超时
		  if(system_tick%1000==6){get_time_stramp(1);get_time_stramp_flag=0;}
		 // if((system_tick%5000)==1){System_Music=CARD_NUMBER_NOT_ENTER;}
		
		
		
		}
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
uint32_t gate_way_ip[1];
uint32_t gate_waytem_ip[1];
 uint32_t chuchang_flag=0;
uint32_t tep_chuchang_flag=0;
uint8_t  ip_in[20]={0};
uint8_t  ip_test[10]={192,168,0,128};
uint32_t  ip_test1[1]={0x12345678};
extern uint8_t  local_eth_ip[30];
//uint8_t   local_eth_port[30]="at+NRPort0=6962\r\n";
extern uint8_t  Remote_eth_ip[30];
extern uint8_t  Remote_eth_port[30];

 uint32_t  u32_local_eth_ip[1]={0};
 uint32_t  u32_Remote_eth_ip[1];
 uint32_t  u32_Remote_eth_port[1];
 uint32_t  u32_dev_num[1]={0}; 
extern uint8_t product_key[30];
 uint32_t u32_product_key[2]={0};

 
//test
//uint32_t  u32_product_key[2]={0x12345678,0x87654321};
//将4个字节转换成U32位
void u8_ip_to_u32_ip(uint8_t *ipbuf,uint32_t *ipuf)
{
          ipuf [0]=*(uint32_t*)ipbuf; //直接操作内存  1.元素的首地址  
	          printf("%08x",ipuf[0]);
}

//将4个字节转换成U32位
void u8_ip_to_u32_ip_more(uint8_t *ipbuf,uint32_t *ipuf,uint16_t u8_len)
{
	  for(uint8_t i=0;i<u8_len;i++)
	{
//          ipuf [i]=*((uint32_t*)ipbuf); //直接操作内存  1.元素的首地址  
	       ipuf [i]=*((uint32_t*)(ipbuf+i*4)); //直接操作内存  1.元素的首地址  
//	       printf("%08x",ipuf[i]);

	}
}
//将u32转换成U8位
void u32_ip_to_u8_ip(uint8_t *ipbuf,uint32_t *ipuf,uint16_t u32_len)
{
	    for(uint8_t i=0;i<u32_len;i++)
	   {
             ipbuf[i*4]=*(uint8_t *)&ipuf[i];
	           ipbuf[i*4+1]=*(((uint8_t *)&ipuf[i])+1);
			       ipbuf[i*4+2]=*(((uint8_t *)&ipuf[i])+2);
			       ipbuf[i*4+3]=*(((uint8_t *)&ipuf[i])+3);
	   }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  Bsp_Driver_Init();
	User_Driver_Init();

  while (1)
  {
    /* USER CODE END WHILE */
    // Add_FR(1);
    /* USER CODE BEGIN 3 */
//printf("123\r\n");
		HAL_IWDG_Refresh(&hiwdg);
	  System_Tick_Count();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System 
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
 void MX_NVIC_Init(void)
{
  /* LPUART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(LPUART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Channel4_5_6_7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 2, 1);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
}

/* USER CODE BEGIN 4 */
uint32_t  local_timestamp=0;
uint32_t  local_timecount=0;
extern uint8_t net_state;  //网络状态
extern uint32_t net_time_count;
extern uint8_t net_time_flag;
/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	  	system_temp_tick++;
  /* USER CODE BEGIN Callback 0 */
	//网络检测
  if (htim->Instance == TIM2) {
		  if(net_time_flag==1)
			{
				net_time_count++;
				 if(net_time_count>5000)  //5秒
				 {
					 net_time_flag=0;
					 net_time_count=0;
					 net_state=1; //断网
				 }
			}
			else
			{
				
				net_time_count=0;
				
			}
		
	}

  
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
	
	    tick_cnt++;
       if(tick_cnt==10)  //10ms为基准
        {
         tick_cnt=0;
          tick_sys++;
//systick_count++;
       }    


      HAL_IncTick();
		  local_timecount++;
		 if(local_timecount%1000==0)  //1s +1
		 {
			 local_timestamp++;
			 //大端存储
			  com_data.timestamp[0]= local_timestamp>>24;
			  com_data.timestamp[1]= local_timestamp>>16;
			  com_data.timestamp[2]= local_timestamp>>8;
			  com_data.timestamp[3]= local_timestamp;
			 //小端存储
			// memcpy (com_data.timestamp, &local_timestamp ,sizeof(local_timestamp));   //同步时间
		 } 
  }
  /* USER CODE BEGIN Callback 1 */
    	HAL_IWDG_Refresh(&hiwdg);
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
