#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "GY273.h"
#include "delay.h"
#include "sys.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//版权所有，盗版必究。
//All rights reserved

//这个程序分成两部分
////////////////////////////////////////////////////////////////////////////////// 	  


//                           第一部分：IIC通信 的协议驱动函数
//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;

}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来 //返回值：1，接收应答失败 // 0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 

//产生ACK应答
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
//不产生ACK应答		    
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

//IIC发送一个字节 //返回从机有无应答 //1，有应答 //0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
    
    IIC_Wait_Ack();
} 	    



//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
//        IIC_NAck();//发送nACK
//    else
//        IIC_Ack(); //发送ACK   
	return receive;
}



//                           第二部分：GY273 的设置函数


uint BUF[8];     //接收数据缓存区
//连续读出HMC5883内部角度数据,地址范围0x3~0x5
void Multi_read_HMC5883(void){   
    uchar i;
    IIC_Start();                           //起始信号
    IIC_Send_Byte(SlaveAddress);           //发送设备地址 写信号
    IIC_Send_Byte(0x03);                   //发送存储单元地址，从0x3 开始
	
    IIC_Start();                           //起始信号
    IIC_Send_Byte(SlaveAddress+1);         //发送设备地址 读信号
    for(i = 0; i<6; i++){                   //连续读取6个地址数据,存储到BUF    
        BUF[i] = IIC_Read_Byte(0);         //BUF[0]存储0x32地址中的数据
        if (i == 5){
            IIC_NAck();                //不产生应答信号，最后一个数据需要返回NOACK
        }
        else{
            IIC_Ack();                 //产生应答信号，返回ACK
        }
   }
    IIC_Stop();                        //停止信号
    delay_ms(5);
}



void Single_Write_HMC5883(uchar REG_Address,uchar REG_data){
    IIC_Start();                  //起始信号
    IIC_Send_Byte(SlaveAddress);   //发送设备地址 写信号
    IIC_Send_Byte(REG_Address);    //内部寄存器地址 参考中文PDF
    IIC_Send_Byte(REG_data);       //内部寄存器数据 参考中文PDF
    IIC_Stop();                   //发送停止信号    
    delay_ms(10);
}
//初始化HMC5883,根据需要参考pdf进行修改****
void Init_HMC5883(void){
    IIC_Init();
    Single_Write_HMC5883(0x02,0x00);  
	 // Single_Write_HMC5883(0x01,0xE0);
	  delay_ms(300);	 
}
                 


//使用 GY273 采集的数据  
// 真正准确的 用于计算啊、控制啊数据是gy273_data
// void conversion(uint temp_data) 这个函数是方便在其它显示屏显示
signed short x,y;
//	signed short z;
double gy273_data;         //用于存放 最后计算的角度值
char ge,shi,bai,qian,wan;  //用来在 LCD 上显示的变量

void conversion(uint temp_data){     
    wan=temp_data/10000;
    temp_data=temp_data%10000;   //取余运算
		qian=temp_data/1000+0x30;
    temp_data=temp_data%1000;    //取余运算
    bai=temp_data/100+0x30;
    temp_data=temp_data%100;     //取余运算
    shi=temp_data/10+0x30;
    temp_data=temp_data%10;      //取余运算
    ge=temp_data+0x30; 
	 
//    LCD_write_string(2,4,(&bai),1);
//		LCD_write_string(8,4,(&shi),1);
//		LCD_write_string(14,4,(&ge),1);
    
}

void gy273_work(void){
	Multi_read_HMC5883();      //连续读出数据，存储在BUF中
	x = (BUF[0] << 8) | BUF[1]; 
	y = (BUF[4] << 8) | BUF[5]; 
//	z = (BUF[2] << 8) | BUF[3]; 
	gy273_data = atan2((double)(x-10),(double)(y+10)) * (180 / 3.14159265) +180; // angle in degrees
	if(gy273_data<180)
		gy273_data=gy273_data+180;
	else
		gy273_data=gy273_data-180;	
	conversion((uint)gy273_data);       //计算数据和显示 
}









