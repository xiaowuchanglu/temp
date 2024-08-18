#include "cdefs-compat.h"

/* __ieee754_sinh(x)
 * Method : 
 * mathematically sinh(x) if defined to be (exp(x)-exp(-x))/2
 *	1. Replace x by |x| (sinh(-x) = -sinh(x)). 
 *	2. 
 *		                                    E + E/(E+1)
 *	    0        <= x <= 22     :  sinh(x) := --------------, E=expm1(x)
 *			       			        2
 *
 *	    22       <= x <= lnovft :  sinh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  sinh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  sinh(x) := x*shuge (overflow)
 *
 * Special cases:
 *	sinh(x) is |x| if x is +INF, -INF, or NaN.
 *	only sinh(0)=0 is exact for finite x.
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double one = 1.0, shuge = 1.0e307;
/*
	e_sinh.c用于计算一个数x的双曲正弦函数值，函数首先利用双曲正弦的奇函数性质，将输入的负数转换为正数进行计算，最终再根据输入的符号调整结果的正负。双曲正弦函数的定义域和值域是整个实数集。双曲正弦函数的数学定义是：
		sinh(x) = (e^x - e^-x) / 2
	双曲正弦函数是奇函数，即 sinh(-x) = -sinh(x)。在计算前，可以处理输入的符号，将所有计算转换为处理正数值，然后在结果中加上符号。该函数的实现采用了多种策略来处理不同范围的输入值：
	对于输入为无穷大或NaN时，函数应直接返回相应的无穷大或NaN。即：
		sinh(Inf) = Inf
		sinh(-Inf) = -Inf
		sinh(NaN) = NaN
	输入范围为|x|<22时，当|x|非常小时，sinh(x)≈x，因为在这种情况下e^x≈1+x。对于|x|< 2^-28，直接返回x，因为该情况下sinh(x)与x几乎相等。对于稍大但仍然较小的x，直接使用公式 sin(x)= (e^x - e^-x) / 2 计算e^x e^-x 可能带来的精度问题，因此函数通过计算e^x −1来避免该问题，具体方法是令 ，即当|𝑥|<1 时，使用：
		sinh(x) = (2t - (t^2 / (t + 1)) / 2)
	否则,当1<|x|<22时，使用：
		sinh(x) = (t + (t / (t + 1)) / 2)
	输入范围为22≤|x|<log(maxdouble)时，x较大，sinh(x) ≈ e^x / 2，因为e^-x在数值上变得非常小，可以忽略不计。使用公式：
		sinh = e ^ |x| / 2
	输入范围为log(maxdouble)≤|x|≤overflowthresold时，在这个范围内，e^|x|可能非常大，直接计算容易导致溢出。因此，采用更稳定的数值方法计算e^|x|。使用分段计算和调整指数的方法：
		sinh(x) = (e^x - e^-x) / 2 
	输入范围为|x|>overflowthresold时，在这个范围内，x非常大，sinh(x)的结果会非常大，接近于计算机能表示的最大数值。返回一个极大的数值来表示溢出情况,即x*1.0e307。
*/
OLM_DLLEXPORT double
__ieee754_sinh(double x)
{
	double t,h;
	int32_t ix,jx,lx;

    /* High word of |x|. */
	EXTRACT_WORDS(jx, lx, x);
	ix = jx&0x7fffffff;

    /* 
		当 x 为无穷大时，sinh(x) 应该返回一个无穷大的值
	*/
	if(ix == 0x7ff00000 && lx == 0) return x;	
	/* 
		当输入中包含 NaN 时，根据 IEEE 754 标准，任何涉及 NaN 的算术运算结果应为 NaN。正确传播 NaN 输入确保函数符合 IEEE 浮点标准，并提供可预测的行为。
	*/
	if(ix >= 0x7ff00000)
	{
		double result = x + x;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}
	h = 0.5;
	if (jx<0) h = -h;
    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
	if (ix < 0x40360000) {		/* |x|<22 */
		/*
			当|x|非常小（|x|< 2^-28）时，函数sinh(x) 应该使用近似公式 sinh(x)≈x 进行计算，以最小化误差。
		*/
	    if (ix<0x3e300000) 		/* |x|<2**-28 */
			if(shuge+x>one) return x;/* sinh(tiny) = tiny with inexact */
		/*
			使用直接的指数函数计算 sinh(x) = (e^x - e^-x) / 2
		*/
	    t = expm1(fabs(x));
	    if(ix<0x3ff00000) return h*(2.0*t-t*t/(t+one));
	    return h*(t+t/(t+one));
	}

    /* 
		对于较大值的|x|（22≤|x|<log(maxdouble)），函数sinh(x) 应该使用近似公式sinh(x) ≈ e^x / 2进行计算，以避免溢出并保持准确性
	 */
	if (ix < 0x40862E42)  return h*__ieee754_exp(fabs(x));

    /* 
		对于|x|接近机器浮点数表示极限值（log(maxdouble)≤|x|≤overflowthresold）的情况，函数sinh(x) 应该使用调整后的指数计算公式以避免溢出并保持数值稳定性：
		sinh(x) = (e ^ (x / 2) / 2) * e ^ (x / 2)
	*/
	if (ix<=0x408633CE)
	    return h*2.0*__ldexp_exp(fabs(x), -1);

    /* 
		输入范围为|x|>overflowthresold时，在这个范围内，x非常大，sinh(x)的结果会非常大，接近于计算机能表示的最大数值。返回一个极大的数值来表示溢出情况,即返回sinh(x) = x * 1.0e307
	*/
	return x*shuge;
}