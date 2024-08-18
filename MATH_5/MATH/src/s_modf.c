/*
 * modf(double x, double *iptr)
 * return fraction part of x, and return x's integral part in *iptr.
 * Method:
 *	Bit twiddling.
 *
 * Exception:
 *	No exception.
 */

#include <MATH_math.h>

#include "math_private.h"

static const double one = 1.0;
/*
	s_modf.c的主要作用是将一个浮点数x分解为其整数部分和小数部分。这个过程不涉及浮点数的四则运算，而是通过位操作（即直接操作数的二进制表示）来实现。modf函数的定义域和值域都是所有双精度浮点数。下面详细解释这个函数的原理和步骤：
	提取指数部分：函数首先将浮点数x的位模式提取出来，并从中提取指数部分。指数部分用于判断`x`的大小范围。
	判断指数范围：根据指数的大小，函数将分为三种主要情况来处理：指数小于20、指数大于51以及介于20到51之间。
	1. 指数小于20的情况
	·指数小于0（|x|<1）：如果指数小于0，说明x的绝对值小于1。此时，整数部分为0，小数部分为x本身。函数将整数部分设置为0（但保留符号位，以处理负数情况），然后返回x作为小数部分。
	·指数在0到19之间：函数会计算一个掩码，掩码用于清除浮点数的小数部分，仅保留整数部分。这个掩码是通过位操作计算出来的。如果x已经是整数，函数直接返回x，并将整数部分赋给iptr。如果x不是整数，函数将掩码应用于x，保留整数部分，去除小数部分。整数部分存储在iptr中，小数部分作为返回值返回。
	2. 指数大于51的情况
	·没有小数部分：如果指数大于51，说明x是一个非常大的数，没有小数部分。整数部分就是x本身。函数将x赋值给iptr，然后返回0作为小数部分。
	·特殊值：无穷大和NaN：如果指数等于特定值（表示无穷大或NaN），函数将直接将x赋值给iptr，并返回x除以0的结果，这会在大多数系统中返回NaN。
	3. 指数在20到51之间的情况
		·小数部分在低位：函数会计算另一个掩码，用于清除整数部分的位，仅保留小数部分。如果x是整数，函数直接返回x，并将整数部分赋给iptr。如果x不是整数，函数将掩码应用于x，保留整数部分，去除小数部分。整数部分存储在iptr中，小数部分作为返回值返回。
	符号处理：在所有情况下，函数都会特别处理符号位，确保返回值和整数部分都保留x的原始符号。这是通过在处理整数和小数部分时保留符号位来实现的。
	对于无穷大和NaN等特殊浮点值的处理，modf函数会特殊处理：对于无穷大，函数应准确返回无穷大作为整数部分，并返回0作为小数部分。对于NaN，函数应正确返回NaN作为整数和小数部分。
*/
OLM_DLLEXPORT double
modf(double x, double *iptr)
{
	int32_t i0,i1,j0;
	u_int32_t i;
	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;	//	提取x的指数
	if(j0<20) {			/* integer part in high x */
	    if(j0<0) {			
			/*
				|x| < 1时，整数部分为0,小数部分为当前输入
			*/
	        INSERT_WORDS(*iptr,i0&0x80000000,0);	/* *iptr = +-0 */
			return x;
	    } else {
			/*
				指数在0～20范围内
			*/
			i = (0x000fffff)>>j0;
			if(((i0&i)|i1)==0) {		
				/*
					针对输入为整数的情况，整数部分为当前输入，小数部分为0
				*/
				u_int32_t high;
				*iptr = x;
				GET_HIGH_WORD(high,x);
				INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
				return x;
			} else {
				/*
					分离整数和小数
				*/
				INSERT_WORDS(*iptr,i0&(~i),0);
				return x - *iptr;
			}
	    }
	} else if (j0>51) {		/* no fraction part */
	    u_int32_t high;
	    if (j0 == 0x400) {		/* inf/NaN */
			if (i0 == 0x7ff00000 && i1 == 0 || i0 == 0xfff00000 && i1 == 0) /*Inf*/
			{
				/*
					无穷大被视为一个完整的数，没有小数部分
				*/
				*iptr = x;
				return 0.0 / x;
			} else 	//NaN
			{	
				/*
					输入NaN时，构造SNaN返回
				*/
				double result = 0.0 / x;
				int32_t high_word = 0x7FF40000;
				int32_t low_word = 0x00000000;
				SET_HIGH_WORD(result, high_word);
				SET_LOW_WORD(result, low_word);
				double result_x =  result;
				*iptr = result ;
				return result_x;
			}
	    }
		/*
			当x极大时，它实际上是一个整数，没有有意义的小数部分
		*/
	    *iptr = x*one;
	    GET_HIGH_WORD(high,x);
	    INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
	    return x;
	} else {	
		/*
			指数在20～51范围内
		*/
	    i = ((u_int32_t)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) { 		
			/*
				针对输入是整数的情况
			*/
	        u_int32_t high;
			*iptr = x;
			GET_HIGH_WORD(high,x);
			INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
			return x;
	    } else {
			/*
				分离整数和小数
			*/
	        INSERT_WORDS(*iptr,i0,i1&(~i));
			return x - *iptr;
	    }
	}
}