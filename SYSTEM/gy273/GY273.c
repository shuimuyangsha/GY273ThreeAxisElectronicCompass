#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "GY273.h"
#include "delay.h"
#include "sys.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//��Ȩ���У�����ؾ���
//All rights reserved

//�������ֳ�������
////////////////////////////////////////////////////////////////////////////////// 	  


//                           ��һ���֣�IICͨ�� ��Э����������
//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;

}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}

//�ȴ�Ӧ���źŵ��� //����ֵ��1������Ӧ��ʧ�� // 0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 

//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     

//IIC����һ���ֽ� //���شӻ�����Ӧ�� //1����Ӧ�� //0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
    
    IIC_Wait_Ack();
} 	    



//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ ){
			IIC_SCL=0; 
			delay_us(2);
			IIC_SCL=1;
			receive<<=1;
			if(READ_SDA){
				receive++; 
			}					
			delay_us(1); 
	}					 
//    if (!ack)
//        IIC_NAck();//����nACK
//    else
//        IIC_Ack(); //����ACK   
	return receive;
}



//                           �ڶ����֣�GY273 �����ú���


uint BUF[8];     //�������ݻ�����
//��������HMC5883�ڲ��Ƕ�����,��ַ��Χ0x3~0x5
void Multi_read_HMC5883(void){   
    uchar i;
    IIC_Start();                           //��ʼ�ź�
    IIC_Send_Byte(SlaveAddress);           //�����豸��ַ д�ź�
    IIC_Send_Byte(0x03);                   //���ʹ洢��Ԫ��ַ����0x3 ��ʼ
	
    IIC_Start();                           //��ʼ�ź�
    IIC_Send_Byte(SlaveAddress+1);         //�����豸��ַ ���ź�
    for(i = 0; i<6; i++){                   //������ȡ6����ַ����,�洢��BUF    
        BUF[i] = IIC_Read_Byte(0);         //BUF[0]�洢0x32��ַ�е�����
        if (i == 5){
            IIC_NAck();                //������Ӧ���źţ����һ��������Ҫ����NOACK
        }
        else{
            IIC_Ack();                 //����Ӧ���źţ�����ACK
        }
   }
    IIC_Stop();                        //ֹͣ�ź�
    delay_ms(5);
}



void Single_Write_HMC5883(uchar REG_Address,uchar REG_data){
    IIC_Start();                  //��ʼ�ź�
    IIC_Send_Byte(SlaveAddress);   //�����豸��ַ д�ź�
    IIC_Send_Byte(REG_Address);    //�ڲ��Ĵ�����ַ �ο�����PDF
    IIC_Send_Byte(REG_data);       //�ڲ��Ĵ������� �ο�����PDF
    IIC_Stop();                   //����ֹͣ�ź�    
    delay_ms(10);
}
//��ʼ��HMC5883,������Ҫ�ο�pdf�����޸�****
void Init_HMC5883(void){
    IIC_Init();
    Single_Write_HMC5883(0x02,0x00);  
	 // Single_Write_HMC5883(0x01,0xE0);
	  delay_ms(300);	 
}
                 


//ʹ�� GY273 �ɼ�������  
// ����׼ȷ�� ���ڼ��㰡�����ư�������gy273_data
// void conversion(uint temp_data) ��������Ƿ�����������ʾ����ʾ
signed short x,y;
//	signed short z;
double gy273_data;         //���ڴ�� ������ĽǶ�ֵ
char ge,shi,bai,qian,wan;  //������ LCD ����ʾ�ı���

void conversion(uint temp_data){     
    wan=temp_data/10000;
    temp_data=temp_data%10000;   //ȡ������
		qian=temp_data/1000+0x30;
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100+0x30;
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10+0x30;
    temp_data=temp_data%10;      //ȡ������
    ge=temp_data+0x30; 
	 
//    LCD_write_string(2,4,(&bai),1);
//		LCD_write_string(8,4,(&shi),1);
//		LCD_write_string(14,4,(&ge),1);
    
}

void gy273_work(void){
	Multi_read_HMC5883();      //�����������ݣ��洢��BUF��
	x = (BUF[0] << 8) | BUF[1]; 
	y = (BUF[4] << 8) | BUF[5]; 
//	z = (BUF[2] << 8) | BUF[3]; 
	gy273_data = atan2((double)(x-10),(double)(y+10)) * (180 / 3.14159265) +180; // angle in degrees
	if(gy273_data<180)
		gy273_data=gy273_data+180;
	else
		gy273_data=gy273_data-180;	
	conversion((uint)gy273_data);       //�������ݺ���ʾ 
}









