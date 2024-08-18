 
/* sin(x)
 * Return sine function of x.
 *
 * kernel function:
 *	__kernel_sin		... sine function on [-pi/4,pi/4]
 *	__kernel_cos		... cose function on [-pi/4,pi/4]
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


//#define INLINE_REM_PIO2
#include "cdefs-compat.h"
#include "MATH_math.h"
#include "math_private.h"
//#include "e_rem_pio2.c"
/*s_sin.c是对正弦函数在不同输入范围下的计算方法的一种高效实现，适用于所有实数输入。
它的核心思想是将输入的角度值通过范围约简，将其转换到[-π/4, π/4]区间内，再应用泰勒级数或直接计算来得到结果。
这种方法的优点是可以提高大角度输入时的计算精度，因为直接在大角度上使用泰勒展开会导致精度问题。
通过参数约简，可以将问题转换为在一个较小的区间内计算，这个区间内泰勒展开的收敛速度更快、精度更高。
利用了数学上的泰勒级数展开和三角函数的性质，以及计算机中的浮点数表示和精度控制技术。*/
OLM_DLLEXPORT double
sin(double x)
{
	int32_t ix;

    /* High word of x. */
	/*参数检查和初步处理：通过GET_HIGH_WORD宏，获取x的高位字，用于后续的判断。*/
	GET_HIGH_WORD(ix,x);
    /* |x| ~< pi/4 */
	/*如果x的绝对值小于等于π/4（通过与0x3fe921fb 比较判断），
	如果x很小（小于2^{-26}，即0x3e500000），
	直接返回x（为了处理很小热的值时的精度问题）。
	对于|x| <= π/4的情况，直接使用_kernel_sin函数计算正弦值。*/
	ix &= 0x7fffffff;
	double z=0.0;
	if(ix <= 0x3fe921fb) {
	    if(ix<0x3e500000)			/* |x| < 2**-26 */
	       {if((int)x==0) return x;}	/* generate inexact */
	    return __kernel_sin(x,z,0);
	}

    /* sin(Inf or NaN) is NaN */
	/*特殊值处理：如果x是无穷大或NaN（通过与0x7ff00000比较判断），
	函数返回 NaN。这是因为正弦函数对于无穷大和NaN没有定义。*/
	else if (ix>=0x7ff00000){
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
	}
 
    /* argument reduction needed */
	/*参数约简：对于绝对值大于π/4的x，使用__ieee754_rem_pio2函数进行参数约简。
	这个函数将x减去适当倍数的π/2，使结果y落在[-π/4, π/4]的区间内，并返回一个整数n，表示减去了多少个π/2。
	根据n mod 4的结果，决定使用正弦还是余弦函数来计算最终值，
	以及是否需要改变符号。这依据的是三角函数的周期性和对称性。
	具体映射如下：
     当n = 0：使用__kernel_sin直接计算正弦值。
     当n = 1：使用__kernel_cos计算余弦值，作为正弦值（因为在第二象限，正弦值等于余弦值）。
     当n = 2：使用__kernel_sin计算正弦值并取负（因为在第三象限，正弦值为负）。
 	 当n != 0\1\2时：使用__kernel_cos计算余弦值并取负，作为正弦值（因为在第四象限，正弦值等于负的余弦值）。*/
	else {
		double y[2];
		int32_t n;
	    n = __ieee754_rem_pio2(x,y);
	    switch(n&3) {
		case 0: return  __kernel_sin(y[0],y[1],1);
		case 1: return  __kernel_cos(y[0],y[1]);
		case 2: return -__kernel_sin(y[0],y[1],1);
		default:
			return -__kernel_cos(y[0],y[1]);
	    }
	}
}

