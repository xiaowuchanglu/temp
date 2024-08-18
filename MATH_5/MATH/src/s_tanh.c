#include "cdefs-compat.h"
/* Tanh(x)
 * Return the Hyperbolic Tangent of x
 *
 * Method :
 *				       x    -x
 *				      e  - e
 *	0. tanh(x) is defined to be -----------
 *				       x    -x
 *				      e  + e
 *	1. reduce x to non-negative by tanh(-x) = -tanh(x).
 *	2.  0      <= x <  2**-28 : tanh(x) := x with inexact if x != 0
 *					        -t
 *	    2**-28 <= x <  1      : tanh(x) := -----; t = expm1(-2x)
 *					       t + 2
 *						     2
 *	    1      <= x <  22     : tanh(x) := 1 - -----; t = expm1(2x)
 *						   t + 2
 *	    22     <= x <= INF    : tanh(x) := 1.
 *
 * Special cases:
 *	tanh(NaN) is NaN;
 *	only tanh(0)=0 is exact for finite argument.
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double one = 1.0, two = 2.0, tiny = 1.0e-300, huge = 1.0e300;
/*
	s_tanh.c函数用于计算一个数x的双曲正切函数值。双曲正切函数的定义域是整个实数集，值域是[-1,+1）。当x的绝对值很大时，它的图形在第一象限内接近于y=1，而在第三象限内接近于y=-1。双曲正切函数是双曲正弦函数（sinh）与双曲余弦函数（cosh）的比值.
	双曲余弦函数是奇函数，即tanh(-x) =-tanh(x)。如果输入的x是负数，则通过tanh(-x) = -tanh(x)，将x转换为非负数。tanh(x)可以根据x的大小分为几个区间进行不同的处理方法，以提高计算效率和精度：
	对于特殊值的处理，tanh输入为无穷大或NaN时，函数应直接返回相应的无穷大或NaN。输入值为无穷大时，则根据x的符号返回1或-1。只有当x = 0时，tanh(0)精确地等于0。
	当输入范围为0 <=|x|< 2^(-28)时,在这个范围内，x 非常小，因此tanh(x)近似等于x。
	当|x|在[2^(-28), 1]内时，在这个范围内，使用公式(-t)/(t + 2)，其中t = expm1(-2x)。expm1函数计算e^x - 1，这比直接计算 e^x更精确，特别是当x接近 0 .
	输入范围为1 <= x < 22时，在这个范围内，使用公式 1 - 2 / (t + 2)，其中 t = expm1(2x)。这个公式也是基于双曲正切的定义的简化形式，用于提高精度。
	输入范围为22 <= x <= INF时，tanh(x)近似等于1。这是因为当x逐渐变大时，e^(-x)变得非常小，因此双曲正切的定义简化为1：
*/
OLM_DLLEXPORT double
tanh(double x)
{
	double t,z;
	int32_t ix,jx,lx;

    /* High word of |x|. */
	EXTRACT_WORDS(jx, lx, x);
	ix = jx&0x7fffffff;

    /* 
		当 x 为无穷大时，tanh(x) 应该返回一个无穷大的值
	*/
	if(ix == 0x7ff00000 && lx == 0) 
	{
		if (jx>=0) return one/x+one;    /* tanh(+-inf)=+-1 */
		else       return one/x-one;   

	}	
	/* 
		当输入中包含 NaN 时，根据 IEEE 754 标准，任何涉及 NaN 的算术运算结果应为 NaN
	*/
	if(ix >= 0x7ff00000)
	{
		double result = x + x;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}	 /* tanh(NaN) = NaN */

    /* |x| < 22 */
	if (ix < 0x40360000) {		/* |x|<22 */
		/*
			当x是一个非常小的正数时，直接返回x
		*/
	    if (ix<0x3e300000) {	/* |x|<2**-28 */
			if(huge+x>one) return x; /* tanh(tiny) = tiny with inexact */
	    }
		/*
			当输入值 1 <= x < 22 时，函数应使用公式 1 - 2 / (t + 2) 来计算 tanh(x)
		*/
	    if (ix>=0x3ff00000) {	/* |x|>=1  */
			t = expm1(two*fabs(x));
			z = one - two/(t+two);
		/*
			对于输入值 2^(-28) <= x < 1，函数应使用公式 (-t) / (t + 2) 来计算 tanh(x)
		*/
	    } else {
	        t = expm1(-two*fabs(x));
	        z= -t/(t+two);
	    }
    /*
		对于非常大的正数输入 22 <= x < INF，函数应返回1作为 tanh(x) 的结果，对于非常大的负数输入 -INF < x <=-22，函数应返回-1作为 tanh(x) 的结果
	*/
	} else {
	    z = one - tiny;		/* raise inexact flag */
	}
	return (jx>=0)? z: -z;
}