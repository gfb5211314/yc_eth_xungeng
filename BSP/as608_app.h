#ifndef       __AS608_APP_H__
#define       __AS608_APP_H__  

#include "stm32L0xx_hal.h"
#include "main.h"


#define      Touch_Pwr_GPIO_Port        TP_POW_EN_GPIO_Port
#define      Touch_Pwr_Pin              TP_POW_EN_Pin

uint8_t Add_FR(uint16_t shouid);	//
void Del_FR(void);	//
uint8_t  press_FR(uint16_t *pageid);
void ShowErrMessage(uint8_t ensure);//




#endif
