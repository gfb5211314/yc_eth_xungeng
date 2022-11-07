#include    "u_init.h"
#include     "main.h"
#include     "stdio.h"
#include     "string.h"
 extern void SystemClock_Config(void);
 extern void MX_GPIO_Init(void);
 extern void MX_DMA_Init(void);
 extern void MX_SPI1_Init(void);
 extern void MX_USART1_UART_Init(void);
 extern void MX_USART2_UART_Init(void);
 extern void MX_LPUART1_UART_Init(void);
 extern void MX_NVIC_Init(void);
 extern void  Rc522_Init(void);
 extern void Eth_Init(void); 
 extern void AS608_Init(void);
 extern void Init_Dev_Param(void);
static void Tick_count_Reset()
{
	uwTick=0;
	
}

void Bsp_Driver_Init()
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
 //MX_IWDG_Init();
  MX_LPUART1_UART_Init();
  MX_NVIC_Init();
  printf("init...\r\n");
 
}
void User_Driver_Init()
{
	    Rc522_Init(); //射频读卡初始化
	    Eth_Init();   //网口初始化
	                  //音频初始化
	   AS608_Init();  //指纹初始化
	   Init_Dev_Param();
	   Tick_count_Reset(); //任务时间清零
}
