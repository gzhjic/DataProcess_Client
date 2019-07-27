#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


//���ڷ��ͻ����� 	
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN];		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ� 
//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];		//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

u16 USART3_RX_STA=0; 
/*******************************************************************************  
* ������      : uart3_init  
* ��������    : IO�˿ڼ�����3,ʱ�ӳ�ʼ������ B10,B11     
* ��  ��      : �� 
* ��  ��      : �� 
*******************************************************************************/    
void USART3_Init(u32 bound)    
{    
	USART_InitTypeDef USART_InitStructure;    
	NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure; 
   
	//ʹ�ܴ��ڵ�RCCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //??UART3??GPIOB???    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    

	//����ʹ�õ�GPIO����  
	// Configure USART3 Rx (PB.11) as input floating      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);    

	// Configure USART3 Tx (PB.10) as alternate function push-pull    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);    

	//���ô���  
	USART_InitStructure.USART_BaudRate = bound;    
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ   
	USART_InitStructure.USART_StopBits = USART_StopBits_1;    
	USART_InitStructure.USART_Parity = USART_Parity_No;    
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������    
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


	// Configure USART3     
	USART_Init(USART3, &USART_InitStructure);

	// Enable USART1 Receive interrupts 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);     

	// Enable the USART3     
	USART_Cmd(USART3, ENABLE);   

	//�����ж����� 
	//Configure the NVIC Preemption Priority Bits       
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);    

	// Enable the USART3 Interrupt     
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure);

	USART3_RX_STA=0;		//����    
}


void USART3_IRQHandler(void)
{
	u8 res;             
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{         
		res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		
		if((USART3_RX_STA&0x8000)==0)
		{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART3_RX_STA=0;	//���մ���, ���¿�ʼ����
				else USART3_RX_STA|=0x8000;	//�������
			}else //��û���յ�0X0D
			{        
				if(res==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=res;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_MAX_RECV_LEN-1))USART3_RX_STA=0;	//���մ���,���¿�ʼ����          
				}                 
			}
		}                                                                                                
	}
}
  
//����3, printf����
//һ�η��͵����ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
        u16 i,j; 
        va_list ap; 
        va_start(ap,fmt);
        vsprintf((char*)USART3_TX_BUF,fmt,ap);
        va_end(ap);
        i=strlen((const char*)USART3_TX_BUF);	//�˴η��͵����ݳ���
        for(j=0;j<i;j++)
        {
                while((USART3->SR&0X40)==0);	//ѭ�����ͣ�ֱ���������	  
                USART3->DR=USART3_TX_BUF[j];  
        } 
}

