#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "malloc.h"
#include "usart2.h"
#include "common.h"
#include "usart3.h"
#include "lte7s4_common.h"
#include "uart4.h"

//#define  DEBUG_USART1_RXTX
//#define  DEBUG_USART3_RXTX
#define  DEBUG_UART4_RXTX

int main(void)
{

#if (defined DEBUG_USART1_RXTX) || (defined DEBUG_USART3_RXTX) || (defined DEBUG_UART4_RXTX)
	u8 t;
	u8 len;	 
#endif
	
	u8 key;
	u16 times=0;
	

	
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(115200);		//���ڳ�ʼ��Ϊ115200
	USART2_Init(115200);	//��ʼ������2������Ϊ115200
	USART3_Init(9600);		//��ʼ������3������Ϊ9600
	UART4_Init(9600);		//��ʼ������4������Ϊ9600
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//������ʼ��
	mem_init();				//��ʼ���ڴ��	
	
#if (!defined DEBUG_USART1_RXTX) && (!defined DEBUG_USART3_RXTX) && (!defined DEBUG_UART4_RXTX)

	printf("\r\n��ʼ����4Gģ�顢ESP8266ģ��...\r\n");
	//wh_lte_7s4_config();
	atk_8266_config();
	printf("4Gģ�顢ESP8266ģ��������ɣ�\r\n");
	printf("����KEY0������ǰ������������ȡʱ�䣻����KEY1��Server�������ݣ�����WK_UP�˳��ն�ϵͳ��\r\n");	
	while(1)
	{
		delay_ms(10); 
		key = KEY_Scan(0);
		if(key == KEY0_PRES)
		{
			//�Ȳ�AQI
			wh_lte_7s4_data_process();
			times=0;
		}
		if(key == KEY1_PRES)
		{			
			atk_8266_data_process(timestamp);
			times=0;
		}
		if(key == WKUP_PRES)
		{
			atk_8266_quit_trans();		//�˳�͸��
			atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20);
			break;
		}
		
		if((times%100)==0)LED0=!LED0;//1000ms��˸ 
		times++;
		
	}
	
#endif
	 
#ifdef DEBUG_USART1_RXTX 
	//����1�շ�����
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n������1�������͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\r\n");//���뻻��
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%30==0)
			{
				printf("\r\n������1������������,�Իس�������\r\n\r\n");
				LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			}
			delay_ms(10);   
		}
		delay_ms(100);
	}
#endif

#ifdef DEBUG_USART3_RXTX 
	//����3�շ�����
	while(1)
	{
		if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			u3_printf("\r\n������3�������͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART3->DR=USART3_RX_BUF[t];
				while((USART3->SR&0X40)==0);//�ȴ����ͽ���
			}
			u3_printf("\r\n");//���뻻��
			USART3_RX_STA=0;
		}
		else
		{
			times++;
			if(times%30==0)
			{
				u3_printf("\r\n������3������������,�Իس�������\r\n");  
				LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			}
			delay_ms(10);   
		}
		delay_ms(100);
	}
#endif
	
#ifdef DEBUG_UART4_RXTX 
	//����4�շ�����
	while(1)
	{
		if(UART4_RX_STA&0x8000)
		{
			len=UART4_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			u4_printf("\r\n������4�������͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				UART4->DR=UART4_RX_BUF[t];
				while((UART4->SR&0X40)==0);//�ȴ����ͽ���
			}
			u4_printf("\r\n");//���뻻��
			UART4_RX_STA=0;
		}
		else
		{
			times++;
			if(times%30==0)
			{
				u4_printf("\r\n������4������������,�Իس�������\r\n");  
				LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			}
			delay_ms(10);   
		}
		delay_ms(100);
	}
#endif	
	
}

