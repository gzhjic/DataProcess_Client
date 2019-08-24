#include "pms5003.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ȫ�ֱ���������
//����ʽָ�Return
u8 passiveCode[] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
u8 PCReturn[] = {0x42, 0x4D, 0x00, 0x04, 0xE1, 0x00, 0x01, 0x74};
//����ʽָ��
u8 activeCode[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
//����ʽ����ָ��
u8 passiveReadCode[] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};

u16 PM25 = 0;

 
//��pms5003����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//dataNum:���ݳ���
void pms5003_send_data(u8 *data, u8 dataNum)
{
	u8 i; 
	for(i = 0; i<dataNum; i++) 
	{ 
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  
		USART_SendData(UART4,*data++); 
	}
}

//pms5003��ʼ������
//����ģʽתΪ����ʽ(passive mode)
u8 pms5003_config()
{
	u8 j;
	UART4_RX_STA = 0;	//��ս��ձ�־��׼����������
	pms5003_send_data(passiveCode, 7);
	delay_ms(20);
	//���ý������
	while(1)
	{
		delay_ms(10); 
		if(UART4_RX_STA == 8)	//���յ�һ��PMS5003��������
		{
			UART4_RX_STA=0;		//��ս��ձ�־
			for(j=0; j<8; j++)
			{
				if(UART4_RX_BUF[j] != PCReturn[j])
				{
					printf("PMS5003ģ������ʧ�ܣ�\r\n");
					return 0;
				}
			}
			printf("PMS5003ģ�����óɹ���\r\n");
			return 1;
		}
	}
}

u8 pms5003_data_process()
{
	u8 times;
	UART4_RX_STA = 0;	//��ս��ձ�־��׼����������
	pms5003_send_data(passiveReadCode, 7);;	//����PM2.5��������
	while(1)
	{
		delay_ms(20); 
		if(UART4_RX_STA == 32)	//���յ�һ��PMS5003��������
		{
			times = 0;			//�������
			UART4_RX_STA=0;		//��ս��ձ�־
			if((UART4_RX_BUF[0]==0x42)&&(UART4_RX_BUF[1]==0x4d))
			{
				//������
				PM25 = 0;
				PM25 = ((u16)UART4_RX_BUF[12] << 8) + UART4_RX_BUF[13];
				printf("PM2.5: %d\r\n", PM25);	//���͵�����
				return 0;
				
			}
			else 
			{
				printf("PMS5003������������1������·�������...\r\n");
				delay_ms(1000);
				return 1;
			}
		}
		else
		{
			times++;
			if(times%20==0)
			{
				printf("PMS5003��������ʧ�ܣ�1������·�������...\r\n");
				delay_ms(1000);
				return 1;
			}
		}
	}
}



