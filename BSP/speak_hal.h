#ifndef __SPEAK_APP__H_
#define __SPEAK_APP__H_
#include "main.h"
#include "GPIO.h"

#define  speak_busy_GPIO_Port  WT_BUSY_GPIO_Port

#define   SPEAK_BUSY_PIN   WT_BUSY_Pin
#define   SPEAK_BUSY_PORT   WT_LCK_GPIO_Port


#define   BELL_EN_PORT     WT_LCK_GPIO_Port
#define   BELL_EN_PIN       WT_LCK_Pin

#define   SPEAK_DATA_PIN                 WT_DAT_Pin
#define   SPEAK_DATA_PORT                WT_DAT_GPIO_Port
extern void Line_1A_WT588S(uint8_t DDATA);
extern void Play_Music_Fun(void);
#endif

