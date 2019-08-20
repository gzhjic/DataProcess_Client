#include "lte7s4_common.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//�û�������

//NTP��������ַ�Լ��˿ں�:123
const u8* NTP_server_addr = "ntp1.aliyun.com";
const u8* NTP_portnum = "123";
u8 NTPDataSend[48] = {	0x1B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0xD0,0xAF,0x5F,0xF5,0x23,0xD7,0x08,0x00};

//Ϊ�˵��Է��㣬��ʱʹ��T2�����Ա�ʾ����ʱ��
uint64_t T_Integer[4] = {0};
uint64_t T_Fraction[4] = {0}; 
uint64_t timestamp = 0;

//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
//void wh_lte_7s4_at_response(u8 mode)
//{
//	if(USART3_RX_STA&0X8000)		//���յ�һ��������
//	{ 
//		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
//		printf("%s",USART3_RX_BUF);	//���͵�����
//		if(mode)USART3_RX_STA=0;
//	} 
//}
//wh_lte_7s4���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* wh_lte_7s4_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��wh_lte_7s4��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 wh_lte_7s4_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(wh_lte_7s4_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//��wh_lte_7s4����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//dataNum:���ݳ���
void wh_lte_7s4_send_data(u8 *data, u8 dataNum)
{
	u8 i; 
	for(i = 0; i<dataNum; i++) 
	{ 
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  
		USART_SendData(USART3,*data++); 
	}
}

//wh_lte_7s4����ָ������״̬
//����ֵ:0, ����ɹ�;
//       1, ����ʧ��
u8 wh_lte_7s4_enter_config(void)
{
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';
	delay_ms(50);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='a';
	delay_ms(500);					//�ȴ�500ms
	return wh_lte_7s4_send_cmd("AT","OK",50);//����ָ������״̬�ж�	
}

//wh_lte_7s4ģ������������
void wh_lte_7s4_config(void)
{
	while(wh_lte_7s4_send_cmd("AT","OK",50))//���4Gģ���Ƿ���ָ������ģʽ
	{
		delay_ms(1000);
		wh_lte_7s4_enter_config();//�˳�͸��������ָ������ģʽ
		printf("δ��⵽4Gģ�飡2�������������ģ��...\r\n\r\n");
		delay_ms(1000);
	} 
	while(wh_lte_7s4_send_cmd("AT+E=OFF","OK",50));//�رջ���
	printf("4Gģ�����ӳɹ�����ʼ���ò�����UDPģʽ...\r\n");
	delay_ms(50); 
	wh_lte_7s4_udp_config();	//����͸��UDP����
}



