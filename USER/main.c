#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "malloc.h"
#include "usart2.h"
#include "common.h"
#include "usart3.h"


int main(void)
{

	u8 t;
	u8 len;	
	u16 times=0; 

	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(115200);		//���ڳ�ʼ��Ϊ115200
	USART2_Init(115200);	//��ʼ������2������Ϊ115200
	USART3_Init(9600);		//��ʼ������3������Ϊ9600
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//������ʼ��
	mem_init();				//��ʼ���ڴ��	

while(1)
{
//	printf("\r\n����1����\r\n");
//	u2_printf("\r\n����2����\r\n");
	u3_printf("\r\n����3����\r\n");
	if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			u3_printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART3->DR=USART3_RX_BUF[t];
				while((USART3->SR&0X40)==0);//�ȴ����ͽ���
			}
			u3_printf("\r\n\r\n");//���뻻��
			USART3_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				u3_printf("\r\nALIENTEK MiniSTM32������ ����ʵ��\r\n");
				u3_printf("����ԭ��@ALIENTEK\r\n\r\n\r\n");
			}
			if(times%200==0)u3_printf("����������,�Իس�������\r\n");  
			if(times%30==0)LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
    //delay_ms(1500);
}	
//	printf("\r\nESP8266 WiFi���Կ�ʼ\r\n");
//	u2_printf("\r\n����2����\r\n");
	//atk_8266_test();
	 
	 
/*
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nALIENTEK MiniSTM32������ ����ʵ��\r\n");
				printf("����ԭ��@ALIENTEK\r\n\r\n\r\n");
			}
			if(times%200==0)printf("����������,�Իس�������\r\n");  
			if(times%30==0)LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
	}
*/	
}


