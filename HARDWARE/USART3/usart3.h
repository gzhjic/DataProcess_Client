#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  

#define USART3_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		1024				//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA;   						//��������״̬

void USART3_Init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt, ...);
#endif













