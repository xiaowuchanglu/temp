/*
 * fabs(x) returns the absolute value of x.
 */

#include <MATH_math.h>

#include "math_private.h"
/*s_fabs.c是对绝对值函数在不同输入范围下的计算方法的一种高效实现，适用于所有实数输入。
它的核心思想是将输入参数的符号位置0来实现绝对值计算。*/
OLM_DLLEXPORT double
fabs(double x)
{
	/*如果是nan，构造sNaN返回*/
	if(isnan(x)){
		double result = 0.0;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}
	u_int32_t high;
	/*获取浮点数高位*/
	GET_HIGH_WORD(high,x);
	/*将符号位置0*/
	SET_HIGH_WORD(x,high&0x7fffffff);
        return x;
}
