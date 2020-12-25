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
  * @file:    macros.h
  * @author:  LonlyPan
  * @version: V0.1
  * @date:    2020-12-10
  * @brief:   公共声明定义
  * @attention:
  *
  * @History:
  *    1.Version: V0.1
  *         date: 2020-12-10
  *        Notes: 初始版本
  *       Author: LonlyPan
  */

#ifndef MACROS_H_
#define MACROS_H_

#include "stm32f4xx_hal.h"

#define millis() HAL_GetTick()  /* 系统时间，程序运行时间基准 */
typedef unsigned long millis_t; /* 专门用于 millis() 型变量声明 */


#define PI (3.14159265358979323846)


#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define PENDING(NOW,SOON) ((long)(NOW-(SOON))<0)
#define ELAPSED(NOW,SOON) (!PENDING(NOW,SOON))  /* NOW > SOON return true */

#define FABS(x)     fabs(x)
#endif /* MACROS_H_ */
