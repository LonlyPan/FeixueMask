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
  * @file:    Feixue.c
  * @author:  LonlyPan
  * @version: V0.1
  * @date:    2020-12-10
  * @brief:   初始化外设
  *           LVGL初始化
  *			  LVGL-screen界面制作
  *    		   任务处理
  * @attention:
  *
  * @History:
  *    1.Version: V0.1
  *         date: 2020-12-10
  *        notes: 初始版本
  *       author: LonlyPan
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#include "feixue.h"
#include "spi.h"
#include "lcd.h"
#include "tim.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_style.h"
#include "timer.h"
#include "stdio.h"
#include "speech_usart.h"

/**
 * @brief  sine 函数，泰勒公式
 * @note   参考：https://stackoverflow.com/questions/23876260/source-code-for-trigonometric-functions-calculations
 * @param  x 当前角度值(0~360)，角度顺时针递增
 * @retval 由x角度计算的正弦值（0~1）
 */
double sine_taylor(double x)
{
	/* 将角度限定在 -90~90 度内，用于后续计算 */
	if(x>90 && x <= 180)
		x = 180 - x;
    else if(x>180 && x <= 270)
		x = 180 - x;
	else if(x>270 && x <= 360)
		x = x - 360;

	/* 将角度值转换为 -PI/2~PI/2 之间 */
    x = x * 2*3.14 / 360.0;
    // useful to pre-calculate
    double x2 = x*x;
    double x4 = x2*x2;

    //计算条件
    //只要abs(x) < sqrt(6)（即2.45），所有项都是正数。
    //为了准确起见，无论如何都应将上述范围之外的值减小为[-pi / 2，pi / 2]。
    //必须注意阶乘。
    //它们可以由编译器预先计算，但是较高的值将超过int的存储容量。
    //因此，强制编译器使用无符号的long long（如果有）或doubles类型。
    double t1 = x * (1.0 - x2 / (2*3));
    double x5 = x * x4;
    double t2 = x5 * (1.0 - x2 / (6*7)) / (1.0* 2*3*4*5);
    double x9 = x5 * x4;
    double t3 = x9 * (1.0 - x2 / (10*11)) / (1.0* 2*3*4*5*6*7*8*9);
    double x13 = x9 * x4;
    double t4 = x13 * (1.0 - x2 / (14*15)) / (1.0* 2*3*4*5*6*7*8*9*10*11*12*13);
    //如果您对准确性由更高要求，请添加更多内容。
    //但是请记住，x小于2，但阶乘增长非常快，所以我怀疑2 ^ 17/17 作用微乎其微（再添加一行计算就会是计算 2^17 阶乘）。
    //甚至与t1相比，t4可能已经太小而无所谓。

    // Sum backwards
    double result = t4;
    result += t3;
    result += t2;
    result += t1;

    return result;
}

/**
 * @brief  cosine 函数，泰勒公式
 * @note   参考：https://stackoverflow.com/questions/23876260/source-code-for-trigonometric-functions-calculations
 * @param  x 当前角度值(0~360)，角度顺时针递增
 * @retval 由x角度计算的余弦值（0~1）
 */
double cose_taylor(double x)
{

    const float Q = 90; // pi/2
    const float BASE_PI =180;
    x += Q;

    if(x > BASE_PI)
        x -= 2 * BASE_PI;

    return( sine_taylor(x));
}

static lv_style_t arc_block_style;  /* 大圆弧块的样式 */
static lv_style_t arc_line_style;   /* 小圆弧块的样式 */


lv_obj_t * big_arc; /* 大的弧块 */
lv_obj_t * tiny_arc;/* 小的弧块 */
lv_obj_t * out_arc; /* 外弧线 */
lv_obj_t * in_arc1; /* 内弧线1 */
lv_obj_t * in_arc2; /* 内弧线1 */

/* 内外弧之间的短线 */
lv_obj_t * line1;
lv_obj_t * line2;
lv_obj_t * line3;
lv_obj_t * line4;

static u8 current_screen = 0;

/* 短线1端点值 */
static lv_point_t line_points1[2] = {{0,0},{0,0}};
/* 短线1与水平夹角，逆时针递增 */
static int line_angle= 87;
/* 短线2 */
static lv_point_t line_points2[2] = {{0,0},{0,0}};
static int line2_angle_offest = 51;
/* 短线3 */
static lv_point_t line_points3[2] = {{0,0},{0,0}};
static int line3_angle_offest = 80;
/* 短线4 */
static lv_point_t line_points4[2] = {{0,0},{0,0}};
static int line4_angle_offest = -33;

static int16_t big_arc_block_start_angle = 330;  /* 大圆弧块一个端边对应角度 */
static int16_t big_arc_block_offer = 300;        /* 大圆弧块的圆心角 */

static int16_t tiny_arc_block_start_angle = 125;
static int16_t tiny_arc_block_end_angle = 25;

static int16_t out_arc_block_start_angle = 285;
static int16_t out_arc_block_off = 265;

static int16_t in_arc1_block_start_angle = 305;
static int16_t in_arc1_block_offer = 110;

static int16_t in_arc2_block_start_angle = 220;
static int16_t in_arc2_block_offer = 55;



#define MAIN_COLOR lv_color_make(255, 213,0)   /* 界面的主色 猩造黄*/
/**
 * An `lv_task` to call periodically to set the angles of the arc
 * @param t
 */
static uint32_t arc_size_reduce = 0;  /* 圆弧的缩小半径 ，动画后期部分圆弧会缩小，当前半径 = 初始值 -arc_size_reduce */


lv_obj_t *show_scr1;  /* 显示界面1：动画*/
lv_obj_t *show_scr2;  /* 显示界面2：圆环*/
lv_obj_t *show_scr3;  /* 显示界面3：圆环+浅色实心圆*/
lv_obj_t *show_scr4;  /* 显示界面4：浅色实心圆*/

lv_obj_t *scr1_label; /* 通用字符标签，主要用于测试 */

lv_task_t * scr1_task;/* 定期调用“ lv_task”以设置圆弧的角度 */
static bool scr1_task_flag = true; /* scr1_task 运行标识符，目前并未在 LVGL 中找到暂停 task 的函数，只有删除函数，但删除后就无法再调用以实现我们想要的重复显示 */

millis_t run_times; /* 系统运行时间 ms */
void screen_change(u8 addr);  /* 切换界面 */

/**
 * @brief  复位界面1的各图案
 * @note   动画期间会改变各图案的角度值、大小等，需再重新运行界面1前初始化
 * @param
 * @retval
 */
static void rest_show_scr1()
{
	/* 透明度 */
    lv_obj_set_style_local_line_opa(big_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(in_arc1,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(in_arc2,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(out_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);

    /* 线宽度 */
    lv_style_set_line_width(&arc_line_style, LV_STATE_DEFAULT,7);
    lv_obj_set_style_local_line_width(in_arc1, LV_ARC_PART_INDIC,LV_STATE_DEFAULT,7);

    /* 角度 */
    lv_arc_set_angles(big_arc,big_arc_block_start_angle,big_arc_block_start_angle-big_arc_block_offer);
    lv_arc_set_angles(tiny_arc, tiny_arc_block_start_angle,150);
    lv_arc_set_angles(in_arc1, in_arc1_block_start_angle,in_arc1_block_start_angle-in_arc1_block_offer);
    lv_arc_set_angles(in_arc2, in_arc2_block_start_angle,in_arc2_block_start_angle+in_arc2_block_offer);
    lv_arc_set_angles(out_arc, out_arc_block_start_angle-out_arc_block_off,out_arc_block_start_angle);

    /* 大小 */
    lv_obj_set_size(in_arc1, 174, 174);
    lv_obj_set_size(in_arc2, 174, 174);
    lv_obj_set_size(tiny_arc, 240, 240);
    //lv_obj_set_size(in_arc2, 240, 240);

    /* 对齐 */
    lv_obj_align(in_arc1, show_scr1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(in_arc2, show_scr1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(tiny_arc, show_scr1, LV_ALIGN_CENTER, 0, 0);

    /* 各短线段 */
    line_points1[0].x= (int)(120+80*cose_taylor(line_angle));
    line_points1[0].y =(int)(120-80*sine_taylor(line_angle));
    line_points1[1].x= (int)(120+116*cose_taylor(line_angle));
    line_points1[1].y = (int)(120-116*sine_taylor(line_angle));


    line_points2[0].x= (int)(120+80*cose_taylor(line_angle+line2_angle_offest));
    line_points2[0].y =(int)(120-80*sine_taylor(line_angle+line2_angle_offest));
    line_points2[1].x= (int)(120+116*cose_taylor(line_angle+line2_angle_offest));
    line_points2[1].y = (int)(120-116*sine_taylor(line_angle+line2_angle_offest));


    line_points3[0].x= (int)(120+80*cose_taylor(line_angle+line3_angle_offest));
    line_points3[0].y =(int)(120-80*sine_taylor(line_angle+line3_angle_offest));
    line_points3[1].x= (int)(120+116*cose_taylor(line_angle+line3_angle_offest));
    line_points3[1].y = (int)(120-116*sine_taylor(line_angle+line3_angle_offest));


    line_points4[0].x= (int)(120+80*cose_taylor(line_angle+line4_angle_offest));
    line_points4[0].y =(int)(120-80*sine_taylor(line_angle+line4_angle_offest));
    line_points4[1].x= (int)(120+116*cose_taylor(line_angle+line4_angle_offest));
    line_points4[1].y = (int)(120-116*sine_taylor(line_angle+line4_angle_offest));

    lv_obj_set_style_local_line_opa(line1,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(line2,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(line3,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);
    lv_obj_set_style_local_line_opa(line4,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);

    lv_line_set_points(line2, line_points2,2);
    lv_line_set_points(line1, line_points1,2);
    lv_line_set_points(line3, line_points3,2);
    lv_line_set_points(line4, line_points4,2);


}
/**
 * @brief   定期调用 “show_scr1_loader” 以设置圆弧的角度
 * @note    动画期间会改变各图案的角度值、大小等，需再重新运行界面1前初始化
 * @param   lv_task_t 自定义任务传参，此处并未用到
 * @retval
 */
static void show_scr1_loader(lv_task_t * t)
{

	/* 以下字符显示语句（原本用于测试使用）不可删除，会导致语音切换屏幕不工作，原因未知*/
	/* 这里改为显示 空字符，不显示任何字符 */
	char buf[30]; /* 储存转换后的字符 字符标签不支持变量直接显示，需要先将变量进行转换 */
    sprintf(buf, "%d", fps_cnt);//将data转换为fmt样式的字符
    lv_label_set_text(scr1_label," ");

	if(scr1_task_flag ){

        big_arc_block_start_angle -= 3;  /* 角度变化量，同时该值也是旋转角度变化的基准值  */
        tiny_arc_block_start_angle -= 3; /* 角度变化量 */
        in_arc1_block_start_angle -= 3;  /* 角度变化量 */
        in_arc2_block_start_angle -= 3;  /* 角度变化量 */
        line_angle += 3;  /* 角度变化量 */

        /* 重新设定各圆弧、线段位置 */
        lv_arc_set_angles(big_arc, big_arc_block_start_angle,(big_arc_block_start_angle < big_arc_block_offer)? big_arc_block_start_angle+60:big_arc_block_start_angle - big_arc_block_offer);
        lv_arc_set_angles(tiny_arc, tiny_arc_block_start_angle < 0 ? (360+tiny_arc_block_start_angle):tiny_arc_block_start_angle ,tiny_arc_block_start_angle+tiny_arc_block_end_angle);
        if(big_arc_block_start_angle > 200) {  /* 当角度变化到该值后，内部圆弧不再旋转 */
                lv_arc_set_angles(in_arc1, in_arc1_block_start_angle,in_arc1_block_start_angle-in_arc1_block_offer);
                lv_arc_set_angles(in_arc2, in_arc2_block_start_angle,in_arc2_block_start_angle+in_arc2_block_offer);
        }

        line_points1[0].x= (int)(120+80*cose_taylor(line_angle));
        line_points1[0].y =(int)(120-80*sine_taylor(line_angle));
        line_points1[1].x= (int)(120+116*cose_taylor(line_angle));
        line_points1[1].y = (int)(120-116*sine_taylor(line_angle));


        line_points2[0].x= (int)(120+80*cose_taylor(line_angle+line2_angle_offest));
        line_points2[0].y =(int)(120-80*sine_taylor(line_angle+line2_angle_offest));
        line_points2[1].x= (int)(120+116*cose_taylor(line_angle+line2_angle_offest));
        line_points2[1].y = (int)(120-116*sine_taylor(line_angle+line2_angle_offest));


        line_points3[0].x= (int)(120+80*cose_taylor(line_angle+line3_angle_offest));
        line_points3[0].y =(int)(120-80*sine_taylor(line_angle+line3_angle_offest));
        line_points3[1].x= (int)(120+116*cose_taylor(line_angle+line3_angle_offest));
        line_points3[1].y = (int)(120-116*sine_taylor(line_angle+line3_angle_offest));


        line_points4[0].x= (int)(120+80*cose_taylor(line_angle+line4_angle_offest));
        line_points4[0].y =(int)(120-80*sine_taylor(line_angle+line4_angle_offest));
        line_points4[1].x= (int)(120+116*cose_taylor(line_angle+line4_angle_offest));
        line_points4[1].y = (int)(120-116*sine_taylor(line_angle+line4_angle_offest));


        lv_line_set_points(line2, line_points2,2);
        lv_line_set_points(line1, line_points1,2);
        lv_line_set_points(line3, line_points3,2);
        lv_line_set_points(line4, line_points4,2);

        /* 手绘动画 一帧对应角度变化量约为3度 */
        /* 程序的顺序与实际显示顺序相反，所以最后一行 else if 是动画最先部分 */
        if(big_arc_block_start_angle <= 145) {  /* 界面1 动画停止，显示结束 */
            lv_arc_set_angles(in_arc1,0,0);
            line_angle= 87;
            big_arc_block_start_angle = 330;
            tiny_arc_block_start_angle = 125;
            out_arc_block_start_angle = 285;
            in_arc1_block_start_angle = 305;
            in_arc2_block_start_angle = 220;
            //lv_arc_set_angles(in_arc1,0,0);
            scr1_task_flag = false;

			current_screen = 2;                /* 切换到界面2 */
			screen_change(current_screen);
        }

        else if(big_arc_block_start_angle <= 155) { /* 内弧线收缩为点，同时改变线宽（否则 点 会很大） */

            arc_size_reduce = 169;
            lv_arc_set_angles(in_arc1,0,359);
            lv_obj_set_style_local_line_width(in_arc1, LV_ARC_PART_INDIC,LV_STATE_DEFAULT,4);
            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);

        }

        else if(big_arc_block_start_angle <= 165) { /* 外弧线消失，内弧线缩短并收缩 、变暗*/

            arc_size_reduce = 150;
            lv_arc_set_start_angle(in_arc1,in_arc1_block_start_angle+110);
            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_set_style_local_line_opa(in_arc1,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,80);
            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);

        }
        else if(big_arc_block_start_angle <= 175) {  /* 外弧线消失，内弧线收缩并缩短 */
            lv_arc_set_angles(out_arc,0,0);

            arc_size_reduce = 130;

            lv_arc_set_start_angle(in_arc1,in_arc1_block_start_angle+80);
            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_set_style_local_line_opa(in_arc1,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,150);

            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);

        }
        else if(big_arc_block_start_angle <= 180) { /* 外弧线缩短，内弧线收缩 */
            lv_arc_set_end_angle(out_arc,40);

            arc_size_reduce = 90;

            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);
        }

        else if(big_arc_block_start_angle <= 190) {/* 小圆弧块消失，外弧线缩短，内弧线收缩 */
            lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,0);
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-195);
            lv_arc_set_angles(in_arc2, 0,0);

            arc_size_reduce = 60;
            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(tiny_arc, NULL, LV_ALIGN_CENTER, 0, 0);
        }

        else if(big_arc_block_start_angle <= 195) {/* 小圆弧块变暗，外弧线、内弧线2缩短，小圆弧块、内弧线收缩 */
            lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,30);
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-135);
            lv_arc_set_start_angle(in_arc2,in_arc2_block_start_angle+in_arc2_block_offer-20);

            arc_size_reduce = 30;
            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_set_size(in_arc2, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_set_size(tiny_arc, 240-arc_size_reduce, 240-arc_size_reduce);
            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(in_arc2, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(tiny_arc, NULL, LV_ALIGN_CENTER, 0, 0);
        }
        else if(big_arc_block_start_angle <= 200) {/* 小圆弧块变暗，外弧线缩短，小圆弧块、内弧线收缩 */
            lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,100);
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-95);
            arc_size_reduce = 10;
            lv_obj_set_size(in_arc1, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_set_size(in_arc2, 174-arc_size_reduce, 174-arc_size_reduce);
            lv_obj_set_size(tiny_arc, 240-arc_size_reduce, 240-arc_size_reduce);
            lv_obj_align(in_arc1, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(in_arc2, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(tiny_arc, NULL, LV_ALIGN_CENTER, 0, 0);
        }
        else if(big_arc_block_start_angle <= 203) {/* 外弧线缩短*/
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-60);

        }
        else if(big_arc_block_start_angle <= 206) {/* 小圆弧块恢复，线段4消失 ，外弧线缩短*/
            lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);
            lv_obj_set_style_local_line_opa(line4,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-35);

        }
        else if(big_arc_block_start_angle <= 209) {/* 小圆弧块变暗，线段4变暗 ，外弧线缩短*/
            lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,50);
            lv_obj_set_style_local_line_opa(line4,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,50);
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-15);
        }
        else if(big_arc_block_start_angle <= 214) {/* 小圆弧块变暗，线段4变暗 ，外弧线缩短*/
            lv_obj_set_style_local_line_opa(tiny_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,240);
            lv_obj_set_style_local_line_opa(line4,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
            lv_arc_set_end_angle(out_arc,out_arc_block_start_angle-5);
        }
        else if(big_arc_block_start_angle <= 360 -138) {/* 大圆弧块消失，线段2消失 */
            lv_obj_set_style_local_line_opa(big_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,0);
            lv_obj_set_style_local_line_opa(line2,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
        }
        else if(big_arc_block_start_angle <= 360 -135) {/* 大圆弧块变暗，线段2变暗、4恢复 */
            lv_obj_set_style_local_line_opa(big_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,200);
            lv_obj_set_style_local_line_opa(line4,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);
            lv_obj_set_style_local_line_opa(line2,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
        }
        else if(big_arc_block_start_angle <= 360 -132) {/* 大圆弧块恢复，线段2恢复、4消失  */
            lv_obj_set_style_local_line_opa(big_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,255);
            lv_obj_set_style_local_line_opa(line4,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
            lv_obj_set_style_local_line_opa(line2,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);
        }
        else if(big_arc_block_start_angle <= 360 -128) {/* 大圆弧块变暗，线段1、2 消失 */
            lv_obj_set_style_local_line_opa(big_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,120);
            lv_obj_set_style_local_line_opa(line1,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
            lv_obj_set_style_local_line_opa(line2,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
        }
        else if(big_arc_block_start_angle <= 360 -125) {/* 大圆弧块变暗，线段1、2 变暗 */
            lv_obj_set_style_local_line_opa(big_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,200);
            lv_obj_set_style_local_line_opa(line1,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
            lv_obj_set_style_local_line_opa(line2,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
        }
        else if(big_arc_block_start_angle <= 360 -123) { /* 线段1 恢复 */
            lv_obj_set_style_local_line_opa(line1,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,255);
        }
        else if(big_arc_block_start_angle <= 360 -118) { /* 线段1 消失 线段3 消失 */
            lv_obj_set_style_local_line_opa(line1,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
             lv_obj_set_style_local_line_opa(line3,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
        }
        else if(big_arc_block_start_angle <= 360 -115) { /* 线段1 变暗 线段3 变亮 */
            lv_obj_set_style_local_line_opa(line1,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
            lv_obj_set_style_local_line_opa(line3,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
        }
        else if(big_arc_block_start_angle <= 360 -110) { /* 线段3 消失 */
            lv_obj_set_style_local_line_opa(line3,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,0);
        }
        else if(big_arc_block_start_angle <= 360 -102) { /* 线段3 变暗 */
            lv_obj_set_style_local_line_opa(line3,LV_LINE_PART_MAIN,LV_STATE_DEFAULT,150);
        }
	}
}

/**
 * @brief   界面定义与初始化
 * @note
 * @param
 * @retval
 */
void test_screen()
{
/* 用于填充背景色,目前没找到填充背景的函数,使用没有角度值的圆弧空间代替 */

    /* scr1 样式表 */
    // 环形弧样式，用于环形弧块
    lv_style_init(&arc_block_style);
    lv_style_set_line_width(&arc_block_style, LV_STATE_DEFAULT,40);         /* 设置环形弧宽度 */
    lv_style_set_line_rounded(&arc_block_style, LV_STATE_DEFAULT,false);    /* 设置两端为直线 */
    lv_style_set_pad_all(&arc_block_style, LV_STATE_DEFAULT,0);             /* 设置内部填充间距 弧形显示部分与外框之间的距离 */

    lv_style_set_line_color(&arc_block_style,LV_STATE_DEFAULT,MAIN_COLOR);  /* 设置 弧形显示部分颜色 */
    lv_style_set_bg_opa(&arc_block_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */
    lv_style_set_border_opa(&arc_block_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */

    // 环形线条样式，补充，用于内外环线
    lv_style_init(&arc_line_style);
    lv_style_set_line_width(&arc_line_style, LV_STATE_DEFAULT,7);           /* 设置环形弧宽度 */

    /* 四条短线段样式 */
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 7);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT,false);    /* 设置两端为直线 */
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, MAIN_COLOR);

    /* 界面1 */
    show_scr1 = lv_scr_act();//lv_obj_create(lv_scr_act(), NULL);    /* 首先创建一个默认界面，必须继承当前默认屏幕 */

    lv_obj_t * scr1_backgroud = lv_obj_create(show_scr1, NULL);     /* 创建背景 */
    lv_obj_set_style_local_bg_color(scr1_backgroud ,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1, 1,1));  /* 设置背景部分颜色，黑色 */
    lv_obj_set_style_local_border_color(scr1_backgroud ,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1, 1,1));
    lv_obj_set_size(scr1_backgroud, 240,240);                   /* 设置背景大小 */
    lv_obj_align(scr1_backgroud, show_scr1, LV_ALIGN_CENTER, 0, 0); /* 设置背景相对scr位置 */

    scr1_label = lv_label_create(scr1_backgroud, NULL);    /* 创建label，仅用于标识，正式应用会删除 */
    lv_label_set_text(scr1_label, " ");
    lv_obj_align(scr1_label, show_scr1, LV_ALIGN_CENTER, 0, 0);


    /*较大的环形弧块*/
    big_arc = lv_arc_create(show_scr1, NULL);
    lv_obj_add_style(big_arc ,LV_ARC_PART_INDIC, &arc_block_style);         /* 设置弧形显示部分样式  */
    lv_obj_add_style(big_arc ,LV_ARC_PART_BG, &arc_block_style);            /* 设置背景部分样式*/
    lv_arc_set_bg_angles(big_arc,0,0);                                      /* 设置背景弧形角度为0，不显示  */
    lv_arc_set_angles(big_arc,big_arc_block_start_angle,big_arc_block_start_angle-big_arc_block_offer);                                     /* 设置前景弧形角度为 */
    lv_obj_set_size(big_arc, 240, 240);                                     /* 设置对象（屏幕）大小 */
    lv_obj_align(big_arc, show_scr1, LV_ALIGN_CENTER, 0, 0);                     /* 设置对象位置，居中，偏移0*/

    /*较小的环形弧块*/
    tiny_arc = lv_arc_create(show_scr1,big_arc);              /* 继承 big_arc 样式 */
    lv_arc_set_bg_angles(tiny_arc,0,0);
    lv_arc_set_angles(tiny_arc, tiny_arc_block_start_angle,150);
    lv_obj_set_size(tiny_arc, 240, 240);
    lv_obj_align(tiny_arc, show_scr1, LV_ALIGN_CENTER, 0, 0);

    /*out block Arc*/
    out_arc = lv_arc_create(show_scr1,big_arc);
    lv_obj_add_style(out_arc,_LV_OBJ_PART_VIRTUAL_LAST, &arc_line_style);   /* 仅修改部分样式 前景弧宽度 */
    lv_arc_set_bg_angles(out_arc,0,0);
    lv_arc_set_angles(out_arc, out_arc_block_start_angle-out_arc_block_off,out_arc_block_start_angle);
    lv_obj_set_size(out_arc, 240, 240);
    lv_obj_align(out_arc, show_scr1, LV_ALIGN_CENTER, 0, 0);

    /*in block Arc1*/
    in_arc1 = lv_arc_create(show_scr1,out_arc);                /* 继承 out_arc 样式 */
    lv_obj_set_style_local_border_opa(in_arc1,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,0);  /* 边框透明 */

    lv_arc_set_bg_angles(in_arc1,0,0);
    lv_arc_set_angles(in_arc1, in_arc1_block_start_angle,in_arc1_block_start_angle-in_arc1_block_offer);
    lv_obj_set_size(in_arc1, 174, 174);                                      /* 小的弧线在内部，对象大小要变，且去除边框 */
    lv_obj_align(in_arc1, show_scr1, LV_ALIGN_CENTER, 0, 0);

    /*in block Arc2*/
    in_arc2 = lv_arc_create(show_scr1,in_arc1);                /* 继承 out_arc 样式 */
    lv_arc_set_angles(in_arc2, in_arc2_block_start_angle,in_arc2_block_start_angle+in_arc2_block_offer);
    lv_obj_set_size(in_arc2, 174, 174);                                      /* 小的弧线在内部，对象大小要变，且去除边框 */
    lv_obj_align(in_arc2, show_scr1, LV_ALIGN_CENTER, 0, 0);

    /*Create a line meter */
    line_points1[0].x= (int)(120+80*cose_taylor(line_angle));
    line_points1[0].y =(int)(120-80*sine_taylor(line_angle));
    line_points1[1].x= (int)(120+116*cose_taylor(line_angle));
    line_points1[1].y = (int)(120-116*sine_taylor(line_angle));

    /*Create a line and apply the new style*/
    line1 = lv_line_create(show_scr1, NULL);
    lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_line);     /*Set the points*/
    lv_line_set_points(line1, line_points1,2);     /*Set the points*/


    line_points2[0].x= (int)(120+80*cose_taylor(line_angle+line2_angle_offest));
    line_points2[0].y =(int)(120-80*sine_taylor(line_angle+line2_angle_offest));
    line_points2[1].x= (int)(120+116*cose_taylor(line_angle+line2_angle_offest));
    line_points2[1].y = (int)(120-116*sine_taylor(line_angle+line2_angle_offest));

    line2 = lv_line_create(show_scr1, line1);
    lv_line_set_points(line2, line_points2,2);     /*Set the points*/

    line_points3[0].x= (int)(120+80*cose_taylor(line_angle+line3_angle_offest));
    line_points3[0].y =(int)(120-80*sine_taylor(line_angle+line3_angle_offest));
    line_points3[1].x= (int)(120+116*cose_taylor(line_angle+line3_angle_offest));
    line_points3[1].y = (int)(120-116*sine_taylor(line_angle+line3_angle_offest));

    line3 = lv_line_create(show_scr1, line1);
    lv_line_set_points(line3, line_points3,2);     /*Set the points*/


    line_points4[0].x= (int)(120+80*cose_taylor(line_angle+line4_angle_offest));
    line_points4[0].y =(int)(120-80*sine_taylor(line_angle+line4_angle_offest));
    line_points4[1].x= (int)(120+116*cose_taylor(line_angle+line4_angle_offest));
    line_points4[1].y = (int)(120-116*sine_taylor(line_angle+line4_angle_offest));

    line4 = lv_line_create(show_scr1, line1);
    lv_line_set_points(line4, line_points4,2);     /*Set the points*/

    scr1_task = lv_task_create(show_scr1_loader, 30,LV_TASK_PRIO_LOW, NULL);  /* 创建任务，每30ms调用一次 */

    /* 界面2 */
    // 环形弧样式，用于环形弧块
    static lv_style_t scr2_arc2_style;
    lv_style_init(&scr2_arc2_style);
    lv_style_set_line_width(&scr2_arc2_style, LV_STATE_DEFAULT,15);         /* 设置环形弧宽度 */
    lv_style_set_line_rounded(&scr2_arc2_style, LV_STATE_DEFAULT,false);    /* 设置两端为直线 */
    lv_style_set_pad_all(&scr2_arc2_style, LV_STATE_DEFAULT,0);             /* 设置内部填充间距 弧形显示部分与外框之间的距离 */

    lv_style_set_line_color(&scr2_arc2_style,LV_STATE_DEFAULT,MAIN_COLOR);  /* 设置 弧形显示部分颜色 */
    lv_style_set_bg_opa(&scr2_arc2_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */
    lv_style_set_border_opa(&scr2_arc2_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */


    show_scr2 = lv_obj_create(NULL, NULL);            /* 从第二幅界面开始必须重新创建屏幕，不可从当前屏幕继承 */

    lv_obj_t * scr2_backgroud = lv_obj_create(show_scr2, NULL);     /* 创建背景 同一控件是可以共用的,也可以重新创建,单会消耗内存*/
    lv_obj_set_style_local_bg_color(scr2_backgroud,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1, 1,1));  /* 设置背景部分颜色，黑色 */
    lv_obj_set_style_local_border_color(scr2_backgroud,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1,1,1));
    lv_obj_set_size(scr2_backgroud, 240,240);
    lv_obj_align(scr2_backgroud, show_scr2, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *scr2_arc = lv_arc_create(show_scr2,NULL);
    lv_obj_add_style(scr2_arc  ,LV_ARC_PART_INDIC, &scr2_arc2_style);         /* 设置弧形显示部分样式  */
    lv_obj_add_style(scr2_arc  ,LV_ARC_PART_BG, &scr2_arc2_style);            /* 设置背景部分样式*/

    lv_arc_set_bg_angles(scr2_arc ,0,0);
    lv_arc_set_angles(scr2_arc , 0,360);
    lv_obj_set_size(scr2_arc , 120, 120);
    lv_obj_align(scr2_arc , show_scr2, LV_ALIGN_CENTER, 0, 0);

    /* 界面3 */
    // 环形弧样式，用于环形弧块
    static lv_style_t scr3_arc_style;
    lv_style_init(&scr3_arc_style);
    lv_style_set_line_width(&scr3_arc_style, LV_STATE_DEFAULT,60);         /* 设置环形弧宽度 */
    lv_style_set_line_rounded(&scr3_arc_style, LV_STATE_DEFAULT,false);    /* 设置两端为直线 */
    lv_style_set_pad_all(&scr3_arc_style, LV_STATE_DEFAULT,0);             /* 设置内部填充间距 弧形显示部分与外框之间的距离 */

    lv_style_set_line_color(&scr3_arc_style,LV_STATE_DEFAULT,MAIN_COLOR);  /* 设置 弧形显示部分颜色 */
    lv_style_set_bg_opa(&scr3_arc_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */
    lv_style_set_border_opa(&scr3_arc_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */


    show_scr3 = lv_obj_create(NULL, NULL);            /* 从第二幅界面开始必须重新创建屏幕，不可从当前屏幕继承 */

    lv_obj_t * scr3_backgroud = lv_obj_create(show_scr3, NULL);     /* 创建背景 同一控件是可以共用的,也可以重新创建,单会消耗内存*/
    lv_obj_set_style_local_bg_color(scr3_backgroud,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1, 1,1));  /* 设置背景部分颜色，黑色 */
    lv_obj_set_style_local_border_color(scr3_backgroud,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1,1,1));
    lv_obj_set_size(scr3_backgroud, 240,240);
    lv_obj_align(scr3_backgroud, show_scr3, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *scr3_arc = lv_arc_create(show_scr3,NULL);
    lv_obj_add_style(scr3_arc  ,LV_ARC_PART_INDIC, &scr3_arc_style);         /* 设置弧形显示部分样式  */
    lv_obj_add_style(scr3_arc  ,LV_ARC_PART_BG, &scr3_arc_style);            /* 设置背景部分样式*/
    lv_obj_set_style_local_line_opa(scr3_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,20);
    lv_obj_set_style_local_line_color(scr3_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,lv_color_make(255, 0,250));
    lv_obj_set_style_local_line_width(scr3_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,45);         /* 设置环形弧宽度 */

    lv_arc_set_bg_angles(scr3_arc ,0,0);
    lv_arc_set_angles(scr3_arc , 0,360);
    lv_obj_set_size(scr3_arc , 210, 210);
    lv_obj_align(scr3_arc , show_scr3, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *scr3_arc3 = lv_arc_create(show_scr3,scr3_arc);
    lv_obj_set_style_local_line_opa(scr3_arc3,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,180);
    lv_obj_set_style_local_line_width(scr3_arc3,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,15);         /* 设置环形弧宽度 */
    lv_obj_set_style_local_line_color(scr3_arc3,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,MAIN_COLOR);

    lv_obj_set_size(scr3_arc3 , 240, 240);
    lv_obj_align(scr3_arc3 , show_scr3, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *scr3_arc2 = lv_arc_create(show_scr3,scr3_arc);
    lv_obj_set_style_local_line_opa(scr3_arc2,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,60);
    lv_obj_set_style_local_line_width(scr3_arc2,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,60);         /* 设置环形弧宽度 */
    lv_obj_set_style_local_line_color(scr3_arc2,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,lv_color_make(128, 128,128));

    lv_obj_set_size(scr3_arc2 , 120, 120);
    lv_obj_align(scr3_arc2 , show_scr3, LV_ALIGN_CENTER, 0, 0);

    /* 界面4 */
    // 环形弧样式，用于环形弧块
    static lv_style_t scr4_arc_style;
    lv_style_init(&scr4_arc_style);
    lv_style_set_line_width(&scr4_arc_style, LV_STATE_DEFAULT,50);         /* 设置环形弧宽度 */
    lv_style_set_line_rounded(&scr4_arc_style, LV_STATE_DEFAULT,false);    /* 设置两端为直线 */
    lv_style_set_pad_all(&scr4_arc_style, LV_STATE_DEFAULT,0);             /* 设置内部填充间距 弧形显示部分与外框之间的距离 */

    lv_style_set_line_color(&scr4_arc_style,LV_STATE_DEFAULT,MAIN_COLOR);  /* 设置 弧形显示部分颜色 */
    lv_style_set_bg_opa(&scr4_arc_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */
    lv_style_set_border_opa(&scr4_arc_style,LV_STATE_DEFAULT,0);               /* 设置显示背景透明 */


    show_scr4 = lv_obj_create(NULL, NULL);            /* 从第二幅界面开始必须重新创建屏幕，不可从当前屏幕继承 */

    lv_obj_t * scr4_backgroud = lv_obj_create(show_scr4, NULL);     /* 创建背景 同一控件是可以共用的,也可以重新创建,单会消耗内存*/
    lv_obj_set_style_local_bg_color(scr4_backgroud,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1, 1,1));  /* 设置背景部分颜色，黑色 */
    lv_obj_set_style_local_border_color(scr4_backgroud,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_make(1,1,1));
    lv_obj_set_size(scr4_backgroud, 240,240);
    lv_obj_align(scr3_backgroud, show_scr4, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *scr4_arc = lv_arc_create(show_scr4,NULL);
    lv_obj_add_style(scr4_arc  ,LV_ARC_PART_INDIC, &scr4_arc_style);         /* 设置弧形显示部分样式  */
    lv_obj_add_style(scr4_arc  ,LV_ARC_PART_BG, &scr4_arc_style);            /* 设置背景部分样式*/
    lv_obj_set_style_local_line_opa(scr4_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,100);
    lv_obj_set_style_local_line_color(scr4_arc,LV_ARC_PART_INDIC,LV_STATE_DEFAULT,lv_color_make(128, 128,128));

    lv_arc_set_bg_angles(scr4_arc ,0,0);
    lv_arc_set_angles(scr4_arc , 0,360);
    lv_obj_set_size(scr4_arc , 100, 100);
    lv_obj_align(scr4_arc , show_scr4, LV_ALIGN_CENTER, 0, 0);
}


/**
 * @brief  初始化
 */
void setup()
{
	__HAL_SPI_ENABLE(&hspi1); /* 使能spi传输  */
	HAL_TIM_Base_Start_IT(&htim10); /* 使能定时器  */
	HAL_UART_Receive_IT(&huart1,&RxBuffer,1); /* 使能串口接受中断  */

	lcd_init();//LCD初始化

	lv_init();				//lvgl系统初始化
	lv_port_disp_init();	//lvgl显示接口初始化,放在lv_init()的后面

	test_screen(); /* 初始化界面后立即会显示 */
	HAL_GPIO_WritePin(GPIOB, LED_G_Pin,SET); /* 同时打开灯光 */

	run_times = millis()+10000;
}

/**
 * @brief  切换显示界面
 * @param  addr 当前界面索引
 */
void screen_change(u8 addr)
{
    switch(addr){
		case 0:
			scr1_task_flag = true;
			rest_show_scr1();
			lv_scr_load(show_scr1);
			break;
    	case 1:
    		scr1_task_flag = false;
			lv_scr_load(show_scr2);
			break;
    	case 2:
    		scr1_task_flag = false;
			lv_scr_load(show_scr3);
			break;
    	case 3:
    		scr1_task_flag = false;
			lv_scr_load(show_scr4);
			break;
   }
}

/**
 * @brief  语音识别
 * @note   current_command 为全局变量，记录当前已经识别到的命令索引
 */
void speech_process()
{
	switch(current_command){
		case SPEECH_WAKE_UP: /* 语音芯片被唤醒 */
			//LCD_Fill(0,0,LCD_W,LCD_H,RED);
			break;
		case SPEECH_LIGHT_ON: /* 打开灯光 */
			HAL_GPIO_WritePin(GPIOB, LED_G_Pin,SET);
			break;
		case SPEECH_LIGHT_OFF:
			HAL_GPIO_WritePin(GPIOB, LED_G_Pin,RESET);
			break;
		case SPEECH_FANS_ONE_GEAR:  /* 风速一档对应 界面1 */
			current_screen = 0;
			screen_change(current_screen);
			break;
		case SPEECH_FANS_TWO_GEAR: /* 界面2 */
			current_screen = 1;
			screen_change(current_screen);
			break;
		case SPEECH_FANS_THREE_GEAR: /* 界面3 */
			current_screen = 2;
			screen_change(current_screen);
			break;
		case SPEECH_FANS_FOUR_GEAR: /* 界面4 */
			current_screen = 3;
			screen_change(current_screen);
			break;
		case SPEECH_PREVIOUS_PIECE: /* 下一个界面，语音芯片bug、上一曲和下一曲颠倒了 */
			if(current_screen >= 3){
				current_screen = 0;
			}
			else{
				current_screen += 1;
			}
			screen_change(current_screen);
			break;
		case SPEECH_NEXT_PIECE:  /* 上一个界面  */
			if(current_screen <= 0){
				current_screen = 3;
			}
			else{
				current_screen -= 1;
			}
			screen_change(current_screen);

			break;
	}
	current_command = 0;
}

void loop()
{
	speech_process();

	if(ELAPSED(millis(), run_times)){ /* 每12秒唤醒一次语音芯片。芯片原生每15秒会休眠，之后需要先说唤醒词唤醒，才能说指令，这里为了简单，保持唤醒状态 */
		run_times = millis()+13000;
		speeech_wake_up();
	}
	lv_task_handler();

}

