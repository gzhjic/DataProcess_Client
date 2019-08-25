#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "malloc.h"
#include "usart2.h"
#include "esp8266_common.h"
#include "usart3.h"
#include "lte7s4_common.h"
#include "uart4.h"
#include "pms5003.h"
#include "timer.h"

//#define  DEBUG_USART1_RXTX
//#define  DEBUG_USART3_RXTX
//#define  DEBUG_UART4_RXTX

int main(void)
{

#if (defined DEBUG_USART1_RXTX) || (defined DEBUG_USART3_RXTX) || (defined DEBUG_UART4_RXTX)
	u8 t;
	u8 len;	 
#endif
	
	u8 keyManual, keyAuto, mode;
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
	TIM3_Int_Init(49999,7199);//10Khz�ļ���Ƶ�ʣ�������50000Ϊ5000ms  
	
#if (!defined DEBUG_USART1_RXTX) && (!defined DEBUG_USART3_RXTX) && (!defined DEBUG_UART4_RXTX)

	printf("\r\n��ʼ����PMS5003ģ�顢4Gģ�顢ESP8266ģ��...\r\n");
	while(pms5003_config());
	wh_lte_7s4_config();
	atk_8266_config();
	printf("PMS5003ģ�顢4Gģ�顢ESP8266ģ��������ɣ�\r\n");
	printf("���ֶ�ģʽ��\r\n��ǰΪ�ֶ�ģʽ������KEY0��ȡ���ݣ�����KEY1�����Զ�ģʽ������WK_UP�˳��ն�ϵͳ��\r\n");	
	while(1)
	{
		delay_ms(10); 
		keyManual = KEY_Scan(0);
		if(keyManual == KEY0_PRES)
		{
			mode = 0;
			//�Ȳ�AQI
			while(pms5003_data_process(mode));
			while(wh_lte_7s4_data_process());
			atk_8266_data_process(timestamp, PM25);
		}
		if(keyManual == KEY1_PRES)
		{
			printf("���Զ�ģʽ��\r\nʱ������5�룻����WK_UP�˳��Զ�ģʽ��\r\n");
			//���ж�
			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
			TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
			while(1)
			{
				delay_ms(10); 
				keyAuto = KEY_Scan(0);
				if(keyAuto == WKUP_PRES)
				{
					//�ر��ж�
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx����
					printf("�Զ�ģʽ�˳��ɹ���\r\n���ֶ�ģʽ��\r\n��ǰΪ�ֶ�ģʽ������KEY0��ȡ���ݣ�����KEY1�����Զ�ģʽ������WK_UP�˳��ն�ϵͳ��\r\n");
					break;
				}
				if((times%100)==0)
				{
					times = 0;
					LED0=!LED0;//1s��˸ 
				}
				times++;	
			}
		}
		if(keyManual == WKUP_PRES)
		{
			atk_8266_quit_trans();		//�˳�͸��
			atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20);
			break;
		}
		if((times%200)==0)
		{
			times = 0;
			LED0=!LED0;//2s��˸ 
		}
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

