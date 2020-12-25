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
  * @file:    speech_usart.c
  * @author:  LonlyPan
  * @version: V0.1
  * @date:    2020-12-10
  * @brief:   语音芯片识别与控制
  * @attention:
  *
  * @History:
  *    1.Version: V0.1
  *         date: 2020-12-10
  *        Notes: 初始版本
  *       Author: LonlyPan
  */
#ifndef SPEECH_USART_C_
#define SPEECH_USART_C_
#include "speech_usart.h"

uint8_t RxBuffer; /* 串口数据缓存 */
uint8_t RxCommand[4] = {0,0,0,0}; /* 当前指令缓存 */
uint8_t RxBufferLength = 0;
uint8_t current_command = 0; /* 当前的命令索引 */

unsigned char wake_up_array[4] = {0xF4,0x0A,0x01,0xFF}; // 唤醒指令

/**
 * @brief   串口回调函数，用来接受语音芯片发送过来的命令
 * @note    语音芯片指令格式：0xF4 0xF6 指令编号 0xFF
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART1){   //判断时那种中断
		RxCommand[RxBufferLength++] = RxBuffer;
		if(RxCommand[0] == 0xF4){
			if(RxBufferLength == 4){
				if(RxCommand[1] == 0x06 && RxCommand[3] == 0xFF){
					current_command = RxCommand[2];
				}
				else{
					current_command = 0;
				}

				RxBufferLength = 0;
				for(int i = 0;i<4;i++){
					RxCommand[i] = 0x00;
				}
			}
		}
		else
			RxBufferLength = 0;
	}
	HAL_UART_Receive_IT(&huart1,&RxBuffer,1); // 再次开启中断
}

/**
 * @brief   唤醒语音芯片
 */
void speeech_wake_up(){
	HAL_UART_Transmit(&huart1, wake_up_array, sizeof(wake_up_array), 500);
}

#endif /* SPEECH_USART_C_ */
