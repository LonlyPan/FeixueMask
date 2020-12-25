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
  * @file:    speech_usart.h
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
#ifndef SPEECH_USART_H_
#define SPEECH_USART_H_

#include "usart.h"

extern uint8_t RxBuffer;  // 定义一个接受数组
extern uint8_t current_command;

/* 语音内容索引 */
enum VOICE_CONTENT{
    SPEECH_WAKE_UP = 1,      // 唤醒
	SPEECH_LIGHT_ON,
	SPEECH_LIGHT_OFF,
	SPEECH_FANS_ONE_GEAR = 10,
	SPEECH_FANS_TWO_GEAR,
	SPEECH_FANS_THREE_GEAR,
	SPEECH_FANS_FOUR_GEAR,
	SPEECH_PREVIOUS_PIECE = 33,
	SPEECH_NEXT_PIECE = 34,
	IDENTIFY_COMMAND2,
	IDENTIFY_COMMAND3,
	IDENTIFY_COMMAND4,
	IDENTIFY_COMMAND5,
	IDENTIFY_COMMAND6,
	IDENTIFY_COMMAND7,
	IDENTIFY_COMMAND8,
	IDENTIFY_COMMAND9,
	IDENTIFY_COMMAND10,
};


void speeech_wake_up();


#endif /* SPEECH_USART_H_ */
