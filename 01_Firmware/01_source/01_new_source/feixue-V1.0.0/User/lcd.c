/**
 * FeixueMask Firmware
 * Copyright (c) 2020 LonlyPan. All rights reserved.
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
/**
  * @file:    lcd.c
  * @author:  LonlyPan
  * @version: V0.1
  * @date:    2020-12-10
  * @brief:   lcd底层驱动（初始话、发送命令等）
  *           lcd一些简单画图功能函数
  * @attention:
  *
  * @History:
  *    1.Version: V0.1
  *         date: 2020-12-10
  *        notes: 初始版本
  *       author: LonlyPan
  */

#include "lcd.h"
#include "spi.h"
#include "stm32f4xx_hal_dma.h"

/**
 * @brief   延时函数，仅用于lcd
 * @param   t 延时的时间
 */
void delay(int t)
{
	while(t--);
}

/**
 * @brief   LCD串行数据写入函数
 * @param   dat  要写入的串行数据
 */
void LCD_Writ_Bus(uint8_t data)
{
	LCD_CS_Clr();
	//while(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) == RESET);//检查接收标志位
	//HAL_SPI_Transmit(&hspi1, (u8 *)&dat,1,HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi1, &data, 1, 100);
	delay(1);
	LCD_CS_Set();
}


/**
 * @brief   LCD写入数据
 * @param   dat  要写入的串行数据 8位
 */
void LCD_WR_DATA8(u8 data)
{
	LCD_Writ_Bus(data);
}

/**
 * @brief   LCD写入数据
 * @param   dat  要写入的串行数据 16位
 */
void LCD_WR_DATA(u16 data)
{
	LCD_Writ_Bus(data >> 8);
	LCD_Writ_Bus(data & 0xFF);
}

/**
 * @brief   LCD写入命令
 * @param   dat  写入的命令
 */
void LCD_WR_REG(u8 data)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(data);
	LCD_DC_Set();//写数据
}


/**
 * @brief   设置起始和结束地址
 * @param   x1,x2 设置列的起始和结束地址
 *          y1,y2 设置行的起始和结束地址
 */
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c);//储存器写
}


/**
 * @brief   在指定区域填充颜色，一般模式
 * @param   xsta,ysta   起始坐标
 *          xend,yend   终止坐标
 *          color       要填充的颜色
 */
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
	u16 i,j;
	LCD_Address_Set(xsta,ysta,xend,yend);//设置显示范围
	for(i=ysta;i<yend+1;i++)
	{
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA(color);
		}
	}
}

/**
 * @brief   在指定区域填充颜色，DMA模式
 * @param   xsta,ysta   起始坐标
 *          xend,yend   终止坐标
 *          color       要填充的颜色
 */
void DMA_LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 *color)
{

	u32 num1;
	num1=(xend-xsta+1)*(yend-ysta+1);
	LCD_Address_Set(xsta,ysta,xend,yend);//设置显示范围
	LCD_CS_Clr();

	HAL_SPI_DeInit(&hspi1);
	hspi1.Init.DataSize = SPI_DATASIZE_16BIT; /* 将spi设置为16位数据传输模式 */
	HAL_SPI_Init(&hspi1);

	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)color, num1);
	LCD_CS_Set();
}

/**
 * @brief   在指定位置画点
 * @param   x,y 画点坐标
 *          color       要填充的颜色
 */
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置
	LCD_WR_DATA(color);
}

/**
 * @brief   lcd初始化
 */
void lcd_init()
{
	//SPI1_Init();
	//LCD_GPIO_Init();//初始化GPIO

	LCD_RES_Clr();//复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);

	LCD_BLK_Set();//打开背光
	HAL_Delay(100);

	LCD_WR_REG(0xEF);
	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14);

	LCD_WR_REG(0xFE);
	LCD_WR_REG(0xEF);

	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14);

	LCD_WR_REG(0x84);
	LCD_WR_DATA8(0x40);

	LCD_WR_REG(0x85);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x86);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x87);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x88);
	LCD_WR_DATA8(0x0A);

	LCD_WR_REG(0x89);
	LCD_WR_DATA8(0x21);

	LCD_WR_REG(0x8A);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x8B);
	LCD_WR_DATA8(0x80);

	LCD_WR_REG(0x8C);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0x8D);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0x8E);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x8F);
	LCD_WR_DATA8(0xFF);


	LCD_WR_REG(0xB6);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x68);
	else LCD_WR_DATA8(0xA8);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);


	LCD_WR_REG(0x90);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);

	LCD_WR_REG(0xBD);
	LCD_WR_DATA8(0x06);

	LCD_WR_REG(0xBC);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xFF);
	LCD_WR_DATA8(0x60);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x13);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x13);

	LCD_WR_REG(0xC9);
	LCD_WR_DATA8(0x22);

	LCD_WR_REG(0xBE);
	LCD_WR_DATA8(0x11);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);

	LCD_WR_REG(0xDF);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF0);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2A);

	LCD_WR_REG(0xF1);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x72);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x6F);


	LCD_WR_REG(0xF2);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2A);

	LCD_WR_REG(0xF3);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x72);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x6F);

	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x0B);

	LCD_WR_REG(0xAE);
	LCD_WR_DATA8(0x77);

	LCD_WR_REG(0xCD);
	LCD_WR_DATA8(0x63);


	LCD_WR_REG(0x70);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x03);

	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x34);

	LCD_WR_REG(0x62);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xED);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xEF);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);

	LCD_WR_REG(0x63);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF3);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);

	LCD_WR_REG(0x64);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x07);

	LCD_WR_REG(0x66);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xCD);
	LCD_WR_DATA8(0x67);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x67);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x98);

	LCD_WR_REG(0x74);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x80);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x4E);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x98);
	LCD_WR_DATA8(0x3e);
	LCD_WR_DATA8(0x07);

	LCD_WR_REG(0x35);
	LCD_WR_REG(0x21);

	LCD_WR_REG(0x11);
	HAL_Delay(120);
	LCD_WR_REG(0x29);
	HAL_Delay(20);
}
