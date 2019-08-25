#include "lte7s4_common.h"
#include "stdlib.h"

//whlte7s4 UDP���ú���
//������������͸��UDPģʽ
void wh_lte_7s4_udp_config()
{
	u8 *p;	
	u8 i;

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
	delay_ms(20);
	printf("4Gģ��������ɣ�\r\n");
		
} 

//whlte7s4 UDP Data Process
//����ͬ��ʱ��
u8 wh_lte_7s4_data_process(void)
{
	u8 j, times = 0;
	USART3_RX_STA = 0;	//��ս��ձ�־��׼����������
	wh_lte_7s4_send_data(NTPDataSend, 48);	//����NTP��������
	while(1)
	{
		delay_ms(50); 
		if(USART3_RX_STA == 48)	//���յ�һ��NTP��������
		{
			//�����㣬�������
			T_Integer[1] = 0;
			T_Fraction[1] = 0;
			timestamp = 0;
			for(j=0; j<4; j++)
			{
				T_Integer[1] |= (uint64_t)USART3_RX_BUF[32+j]<<(8*(3-j));
				T_Fraction[1] |= (uint64_t)USART3_RX_BUF[36+j]<<(8*(3-j));

			}
			timestamp = (T_Integer[1]-0x83AA7E80)*1000 + T_Fraction[1]*1000/4294967296;
			
			printf("Timestamp: %"PRIu64"\r\n",timestamp);	//���͵�����
			USART3_RX_STA=0;		//��ս��ձ�־
			return 0;
		}
		else
		{
			times++;
			if(times%20==0)
			{
				printf("4Gģ�鷵����������5������·�������...\r\n");
				times = 0;
				return 1;
			}
		}
	}
}


