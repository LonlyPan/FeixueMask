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
  * @file:    timer.c
  * @author:  LonlyPan
  * @version: V0.1
  * @date:    2020-12-10
  * @brief:   定时器
  * @attention:
  *
  * @History:
  *    1.Version: V0.1
  *         date: 2020-12-10
  *        Notes: 初始版本
  *       Author: LonlyPan
  */
#include "timer.h"
#include "lvgl.h"

/* 中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	lv_tick_inc(1);//lvgl的1ms心跳
	fps_cnt++;
}
