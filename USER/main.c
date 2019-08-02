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

//#define  DEBUG_USART1_RXTX
//#define  DEBUG_USART3_RXTX

int main(void)
{

#if (defined DEBUG_USART1_RXTX) || (defined DEBUG_USART3_RXTX)
	u8 t;
	u8 len;	 
#endif
	
	u8 key;
	u16 times=0;
	

	
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);		//串口初始化为115200
	USART2_Init(115200);	//初始化串口2波特率为115200
	USART3_Init(9600);		//初始化串口3波特率为9600
	LED_Init();				//初始化与LED连接的硬件接口
	KEY_Init();				//按键初始化
	mem_init();				//初始化内存池	
	
	printf("\r\n开始配置4G模块、ESP8266模块...\r\n");
	wh_lte_7s4_config();
	atk_8266_config();
	printf("4G模块、ESP8266模块配置完成！\r\n");
	printf("按下KEY0获取当前时间；按下KEY1给Server发送数据；按下WK_UP退出终端系统。\r\n");	
	while(1)
	{
		delay_ms(10); 
		key = KEY_Scan(0);
		if(key == KEY0_PRES)
		{
			wh_lte_7s4_data_process();
			times=0;
		}
		if(key == KEY1_PRES)
		{
			atk_8266_data_process(T2_second, T2_millisecond);
			times=0;
		}
		if(key == WKUP_PRES)
		{
			atk_8266_quit_trans();		//退出透传
			atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20);
			break;
		}
		
		if((times%100)==0)LED0=!LED0;//1000ms闪烁 
		times++;
		
	}
	 
#ifdef DEBUG_USART1_RXTX 
	//串口1收发测试
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//等待发送结束
			}
			printf("\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%30==0)
			{
				printf("请输入数据,以回车键结束\r\n\r\n");
				USART_SendData(USART1,0x1B);
				printf("\r\n\r\n");
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(50);
	}
#endif

#ifdef DEBUG_USART3_RXTX 
	//串口3收发测试
	while(1)
	{
		u3_printf("\r\n串口3测试\r\n");
		if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			u3_printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART3->DR=USART3_RX_BUF[t];
				while((USART3->SR&0X40)==0);//等待发送结束
			}
			u3_printf("\r\n");//插入换行
			USART3_RX_STA=0;
		}
		else
		{
			times++;
			if(times%50==0)
			{
				u3_printf("请输入数据,以回车键结束\r\n");  
				USART_SendData(USART3,0x1B) ;
				u3_printf("\r\n\r\n");
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(1000);
	}
#endif
}


