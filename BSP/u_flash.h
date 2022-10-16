#ifndef  __U_FLASH_H_
#define  __U_FLASH_H_
#include "main.h"

#define PAGE_SIZE                128
#define PAGE_NUM                 512
#define ADDR_FLASH_PAGE_505      0X08000000 + 128*504   //
#define ADDR_FLASH_PAGE_506      0X08000000 + 128*505   //
#define ADDR_FLASH_PAGE_507      0X08000000 + 128*506   //
#define ADDR_FLASH_PAGE_508      0X08000000 + 128*507   //
#define ADDR_FLASH_PAGE_509      0X08000000 + 128*508   //
#define ADDR_FLASH_PAGE_510      0X08000000 + 128*509   //
#define ADDR_FLASH_PAGE_511      0X08000000 + 128*510   //
#define ADDR_FLASH_PAGE_512      0X08000000 + 128*511   //���һҳ



extern void Flash_Write_Num_Word(uint32_t WriteAddr, uint32_t * pBuffer, uint32_t NumToWrite );

/**
  * ��������: ��ȡָ����ַ�İ���(16λ����)
  * �������: faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
  * �� �� ֵ: ����ֵ:��Ӧ����.
  * ˵    ������
  */
extern uint32_t U_STMFLASH_ReadWord ( uint32_t faddr );

/**
  * ��������: ��ָ����ַ��ʼ����ָ�����ȵ�����
  * �������: ReadAddr:��ʼ��ַ
  *           pBuffer:����ָ��
  *           NumToRead:����(16λ)��
  * �� �� ֵ: ��
  * ˵    ������
  */
extern void Flash_Read_Word( uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead )  ; 	

#endif