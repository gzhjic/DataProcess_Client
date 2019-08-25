#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


//���ڷ��ͻ����� 	
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN];		//���ͻ���,���UART4_MAX_SEND_LEN�ֽ� 
//���ڽ��ջ����� 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN];		//���ջ���,���UART4_MAX_RECV_LEN���ֽ�.

u16 UART4_RX_STA=0; 
/*******************************************************************************  
* ������      : UART4_Init  
* ��������    : IO�˿ڼ�����4,ʱ�ӳ�ʼ������ C10,C11     
* ��  ��      : �� 
* ��  ��      : �� 
*******************************************************************************/    
void UART4_Init(u32 bound)    
{    
	USART_InitTypeDef USART_InitStructure;    
	NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure; 
   
	//ʹ�ܴ��ڵ�RCCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);    

	//����ʹ�õ�GPIO����  
	// Configure UART4 Rx (PC.11) as input floating      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	// Configure UART4 Tx (PC.10) as alternate function push-pull    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	//���ô���  
	USART_InitStructure.USART_BaudRate = bound;    
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ   
	USART_InitStructure.USART_StopBits = USART_StopBits_1;    
	USART_InitStructure.USART_Parity = USART_Parity_No;    
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������    
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


	// Configure UART4     
	USART_Init(UART4, &USART_InitStructure);

	// Enable UART4 Receive interrupts 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);     

	// Enable the UART4     
	USART_Cmd(UART4, ENABLE);      

	// Enable the USART4 Interrupt     
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure);

	UART4_RX_STA=0;		//����    
}


void UART4_IRQHandler(void)
{
	u8 res;             
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{         
		res =USART_ReceiveData(UART4);	//��ȡ���յ�������
		
		if((UART4_RX_STA&0x8000)==0)
		{
			if(UART4_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)UART4_RX_STA=0;	//���մ���, ���¿�ʼ����
				else UART4_RX_STA|=0x8000;	//�������
			}else //��û���յ�0X0D
			{        
				if(res==0x0d)UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=res;
					UART4_RX_STA++;
					if(UART4_RX_STA>(UART4_MAX_RECV_LEN-1))UART4_RX_STA=0;	//���մ���,���¿�ʼ����          
				}                 
			}
		}                                                                                                
	}
}
  
//����4, printf����
//һ�η��͵����ݲ�����USART4_MAX_SEND_LEN�ֽ�
void u4_printf(char* fmt,...)  
{  
        u16 i,j; 
        va_list ap; 
        va_start(ap,fmt);
        vsprintf((char*)UART4_TX_BUF,fmt,ap);
        va_end(ap);
        i=strlen((const char*)UART4_TX_BUF);	//�˴η��͵����ݳ���
        for(j=0;j<i;j++)
        {
                while((UART4->SR&0X40)==0);	//ѭ�����ͣ�ֱ���������	  
                UART4->DR=UART4_TX_BUF[j];  
        } 
}

