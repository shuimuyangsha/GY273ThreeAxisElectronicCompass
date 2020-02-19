#ifndef GY273_H
#define GY273_H
#include "sys.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/10 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//                           第一部分：IIC通信 的协议驱动函数   	   		   
//IO方向设置
#define SDA_IN() {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=8<<16;}
#define SDA_OUT() {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=3<<16;}

//IO操作函数	 
#define IIC_SCL    PAout(11) //SCL
#define IIC_SDA    PAout(12) //SDA	 
#define READ_SDA   PAin(12)  //输入SDA 

//                           第二部分：GY273 的设置函数

#define  uchar unsigned char
#define  uint  unsigned int	

#define	SlaveAddress   0x3C	  //????5883?IIC???????
typedef unsigned char BYTE;
typedef unsigned short WORD;




//                           第一部分：IIC通信 的协议驱动函数
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  


//                           第二部分：GY273 的设置函数
void  Multi_read_HMC5883(void);                 //连续读出HMC5883内部角度数据,地址范围0x3~0x5
void  Single_Write_HMC5883(uchar REG_Address,uchar REG_data);   //单个写入数据
//uchar Single_Read_HMC5883(uchar REG_Address);                   //单个读取内部寄存器数据
void Init_HMC5883(void);            //初始化5883
void conversion(uint temp_data);
void gy273_work(void);


#endif

