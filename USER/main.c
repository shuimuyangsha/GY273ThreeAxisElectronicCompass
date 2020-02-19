#include "stm32f10x.h"
#include "usart.h"	
#include "delay.h"
#include "sys.h"
#include "GY273.h"

/************************************************
 ALIENTEK ս��STM32F103������ʵ��0
 ����ģ��
 ע�⣬�����ֲ��е��½������½�ʹ�õ�main�ļ� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

extern double gy273_data;
extern void gy273_work(void);

 int main(void)
 {	
  delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600); 
	Init_HMC5883();      //GY273 ��ʼ��
  while(1)
	{
	  gy273_work();
		printf("gy273_data is %f \r\n",gy273_data); 
		
		
	}
 }
