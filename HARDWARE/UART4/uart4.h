#ifndef __UART4_H
#define __UART4_H	 
#include "sys.h"  

#define UART4_MAX_RECV_LEN		1024				//最大接收缓存字节数
#define UART4_MAX_SEND_LEN		1024				//最大发送缓存字节数
#define UART4_RX_EN 			1					//0,不接收;1,接收.

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//接收缓冲,最大UART4_MAX_RECV_LEN字节
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//发送缓冲,最大UART4_MAX_SEND_LEN字节
extern u16 UART4_RX_STA;   						//接收数据状态

void UART4_Init(u32 bound);				//串口4初始化 
void u4_printf(char* fmt, ...);
#endif













