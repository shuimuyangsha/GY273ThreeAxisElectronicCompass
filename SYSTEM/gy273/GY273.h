#ifndef GY273_H
#define GY273_H
#include "sys.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//                           ��һ���֣�IICͨ�� ��Э����������   	   		   
//IO��������
#define SDA_IN() {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=8<<16;}
#define SDA_OUT() {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=3<<16;}

//IO��������	 
#define IIC_SCL    PAout(11) //SCL
#define IIC_SDA    PAout(12) //SDA	 
#define READ_SDA   PAin(12)  //����SDA 

//                           �ڶ����֣�GY273 �����ú���

#define  uchar unsigned char
#define  uint  unsigned int	

#define	SlaveAddress   0x3C	  //????5883?IIC???????
typedef unsigned char BYTE;
typedef unsigned short WORD;




//                           ��һ���֣�IICͨ�� ��Э����������
//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  


//                           �ڶ����֣�GY273 �����ú���
void  Multi_read_HMC5883(void);                 //��������HMC5883�ڲ��Ƕ�����,��ַ��Χ0x3~0x5
void  Single_Write_HMC5883(uchar REG_Address,uchar REG_data);   //����д������
//uchar Single_Read_HMC5883(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
void Init_HMC5883(void);            //��ʼ��5883
void conversion(uint temp_data);
void gy273_work(void);


#endif

