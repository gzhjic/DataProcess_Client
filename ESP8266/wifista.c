#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
u8 netpro=0;	//����ģʽ
void atk_8266_wifista_config(void)
{
	u8 *p;

	p=mymalloc(32);							//����32�ֽ��ڴ�
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ4S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
    
	delay_ms(200);		//ֱ������netproʱ����Ҫ��һ����ʱ
	netpro = 1;			//0:TCP server; 1:TCP client; 2:UDP
	
	if(netpro&0x00)		//TCP server(������)
	{

	}	
	else if(netpro&0x01)	//TCP client  ͸��ģʽ
	{
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0:������,1:������		
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP Server
		while(atk_8266_send_cmd(p,"OK",300))
		{
			printf("ESP8266����TCPʧ��, IP:%s, Port:%s\r\n", ipbuf, (u8*)portnum);
		}		
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��
	
	}	
	else if(netpro&0x02)	//UDP(������)
	{

	}
	printf("ESP8266ģ�����óɹ���\r\n");
	myfree(p);		//�ͷ��ڴ� 	
} 

//ATK-ESP8266 Data Process
//������Server��������
void atk_8266_data_process(u32 T2_second, u32 T2_millisecond)
{
	u16 rlen=0;
	USART2_RX_STA = 0;
	
	if(netpro&0x00)		//TCP server(������)
	{

	}
	else if(netpro&0x01)		//TCP client  ͸��ģʽ
	{
		atk_8266_quit_trans();
		atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��

		u2_printf("POST /dataInfo/store?clientId=%s&clientTime=2208988801&airPara=60 HTTP/1.1\r\n", clientId);
		delay_ms(10);//��ʱһ��ʱ��ȴ��������
		u2_printf("Content-Type: application/json;charset=utf-8\r\n");
		delay_ms(10);
		u2_printf("Host: %s:%s\r\n", ipbuf, (u8*)portnum);
		delay_ms(10);
		u2_printf("Connection: Keep Alive\r\n");
		delay_ms(10);
		u2_printf("\r\n");

		//u2_printf("%08X",T2_second);
		delay_ms(200);
		//u2_printf("%08X",T2_millisecond);
	}
	else if((netpro==3)||(netpro==2))   //UDP(������)
	{

	}
	
	while(1)
	{
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//���յ�һ��������
		{
			rlen=USART2_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
			USART2_RX_BUF[rlen]=0;		//��ӽ����� 
			printf("%s",USART2_RX_BUF);	//���͵�����   
			USART2_RX_STA=0;
			break;
		}
	}
}

