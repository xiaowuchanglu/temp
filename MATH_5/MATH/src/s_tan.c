#include "cdefs-compat.h"
/* tan(x)
 * Return tangent function of x.
 *
 * kernel function:
 *	__kernel_tan		... tangent function on [-pi/4,pi/4]
 *	__ieee754_rem_pio2	... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *	[-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *	in [-pi/4 , +pi/4], and let n = k mod 4.
 *	We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *	    0	       S	   C		 T
 *	    1	       C	  -S		-1/T
 *	    2	      -S	  -C		 T
 *	    3	      -C	   S		-1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *	TRIG(x) returns trig(x) nearly rounded
 */

#include <float.h>
#include <MATH_math.h>

//#define INLINE_REM_PIO2
#include "math_private.h"
//#include "e_rem_pio2.c"
/*s_tan.c是对正切函数在不同输入范围下的计算方法的一种高效实现，适用于所有实数输入。
它的核心思想是将输入的角度值通过范围约简，将其转换到[-π/4, π/4]区间内，再应用泰勒级数或直接计算来得到结果。
它巧妙地利用了数学上的泰勒级数展开和三角函数的性质，以及计算机科学中的浮点数表示和精度控制技术。*/
OLM_DLLEXPORT double
tan(double x)
{
	double y[2],z=0.0;
	int32_t n, ix;

    /* High word of x. */
	/*参数检查和初步处理：通过GET_HIGH_WORD宏，获取x的高位字，用于后续的判断。*/
	GET_HIGH_WORD(ix,x);

    /* |x| ~< pi/4 */
	/*如果x的绝对值小于等于π/4（通过与0x3fe921fb 比较判断），
	如果x很小（小于2^{-27}，即0x3e400000），
	如果x==0直接返回x（为了处理很小的值时的精度问题）。
	对于|x| <= π/4的情况，直接使用_kernel_tan函数计算正弦值。*/
	ix &= 0x7fffffff;
	if(ix <= 0x3fe921fb) {
	    if(ix<0x3e400000)			/* x < 2**-27 */
		if((int)x==0) return x;		/* generate inexact */
	    return __kernel_tan(x,z,1);
	}

    /* tan(Inf or NaN) is NaN */
	/*特殊值处理：如果x是无穷大或NaN（通过与0x7ff00000比较判断），
	函数返回 NaN。这是因为正弦函数对于无穷大和NaN没有定义。*/
	else if (ix>=0x7ff00000) {
		if isnan(x - x)	{
			double result = 0.0;
			int32_t high_word = 0x7FF40000;
			int32_t low_word = 0x00000000;
			SET_HIGH_WORD(result, high_word);
			SET_LOW_WORD(result, low_word);
			return result;
		} else {
			return x - x;
		}
	}	/* NaN */

    /* argument reduction needed */
	/*参数约简：对于绝对值大于π/4的x，使用__ieee754_rem_pio2函数进行参数约简。
	这个函数将x减去适当倍数的π/2，使结果y落在[-π/4, π/4]的区间内，并返回一个整数n，表示减去了多少个π/2。
	使用__kernel_tan利用泰勒展开计算tan函数的近似值*/
	else {
	    n = __ieee754_rem_pio2(x,y);
	    return __kernel_tan(y[0],y[1],1-((n&1)<<1)); /*   1 -- n even
							-1 -- n odd */
	}
}
