#include "lte7s4_common.h"
#include "stdlib.h"

//whlte7s4 UDP���Ժ���
//���ڲ�������͸��UDPģʽ
void wh_lte_7s4_udp_test(void)
{
	u8 *p;	
	u8 key;
	u16 timex=0;
	u8 i, j;
	u8 NTPDataSend[48] = {	0x1B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0xD0,0xAF,0x5F,0xF5,0x23,0xD7,0x08,0x00};

	p=mymalloc(32);							//����32�ֽ��ڴ�
	wh_lte_7s4_send_cmd("AT+WKMOD=NET","OK",50);	//����͸��ģʽ
	delay_ms(20);
	wh_lte_7s4_send_cmd("AT+SOCKAEN=ON","OK",50);	//socketA ʹ��
	delay_ms(20);
	sprintf((char*)p,"AT+SOCKA=UDP,%s,%s",NTP_server_addr,NTP_portnum);//���� SocketA ��Զ�̵�ַ�Լ��˿�
	while(wh_lte_7s4_send_cmd(p,"OK",200))
	{
		printf("4Gģ������ NTP Server ʧ�ܣ�1S���������ã�Addr:%s, Port:%s\r\n", NTP_server_addr, NTP_portnum);
		delay_ms(1000);
	}	
	wh_lte_7s4_send_cmd("AT+Z","OK",50);		//����ģ�飬������Ч
	printf("4Gģ�����óɹ����ȴ��������...\r\n");
	for(i=0; i<20; i++)		//��ʱ20S�ȴ������ɹ�
	{
		delay_ms(1000);
	}
	myfree(p);		//�ͷ��ڴ� 
	USART3_RX_STA = 0;	//��ս��ձ�־��׼����������
	printf("4Gģ��������ɣ����� KEY1 ��ʼ���Ͳ�������\r\n");
	while(1)
	{
		delay_ms(10); 
		key = KEY_Scan(0);
		if(key == KEY1_PRES)
		{
			wh_lte_7s4_send_data(NTPDataSend, 48);
			timex=0;
		}
		if(key == WKUP_PRES)
		{
			break;
		}
		if(USART3_RX_STA == 48)		//���յ�һ��NTP��������
		{
			for(j=0; j<48; j++)
				printf("%02X",USART3_RX_BUF[j]);	//���͵�����   
			USART3_RX_STA=0;		//��ս��ձ�־
		}
		
		if((timex%100)==0)LED0=!LED0;//200ms��˸ 
		timex++;
		
	}	
} 



