
#include "cdefs-compat.h"
/* __ieee754_cosh(x)
 * Method : 
 * mathematically cosh(x) if defined to be (exp(x)+exp(-x))/2
 *	1. Replace x by |x| (cosh(x) = cosh(-x)). 
 *	2. 
 *		                                        [ exp(x) - 1 ]^2 
 *	    0        <= x <= ln2/2  :  cosh(x) := 1 + -------------------
 *			       			           2*exp(x)
 *
 *		                                  exp(x) +  1/exp(x)
 *	    ln2/2    <= x <= 22     :  cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  cosh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  cosh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only cosh(0)=1 is exact for finite x.
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double one = 1.0, half=0.5, huge = 1.0e300;
/*
	e_cosh.c函数用于计算一个数x的双曲余弦函数值。双曲余弦函数的定义域是整个实数集，值域是[1,+Inf）。cosh(x)的最小值是1，当x为0时达到此值，且cosh(x)随着|x|的增大而增大。
	双曲余弦函数是偶函数，即 。cosh(x)可以根据x的大小分为几个区间进行不同的处理方法，以提高计算效率和精度：
	对于输入为无穷大或NaN时，函数应直接返回相应的无穷大或NaN。
	当输入范围为[0, 0.5*ln2]时,x的值较小，直接使用公式计算 和可能带来的精度损失，因此函数通过计算 −1来避免该问题，具体公式如下：
		cosh(x) = 1 + (e^x - 1)^2 / 2*e^x
	当|x|在[0.5*ln2, 22]内时，该范围适用cosh函数定义公式进行计算，即：
		cosh(x) = (e^x + e^-x) / 2
	输入范围为22≤|x|<log(maxdouble)时，x的值较大，e^-x在数值上变得非常小，可以忽略不计。因此使用公式：
		cosh(x) = e ^x / 2
	输入范围为log(maxdouble)≤|x|≤overflowthresold时，在这个范围内，可能非常大，直接计算容易导致溢出。因此，采用更稳定的数值方法计算。使用分段计算和调整指数的方法：
		cosh(x) = e^(x/2) / 2 × e^(x/2)
	输入范围为|x|>overflowthresold时，在这个范围内，x非常大，cosh(x)的结果会非常大，接近于计算机能表示的最大数值。返回一个极大的数值来表示溢出情况,即1.0e307*1.0e307。
*/
OLM_DLLEXPORT double
__ieee754_cosh(double x)
{
	double t,w;
	int32_t ix,jx,lx;

    /* High word of |x|. */
	EXTRACT_WORDS(jx, lx, x);
	ix = jx&0x7fffffff;

    /* 
		当 x 为无穷大时，cosh(x) 应该返回一个无穷大的值
	*/
	if(ix == 0x7ff00000 && lx == 0) return x;	
	/* 
		当输入值x为 NaN 时，函数应返回 NaN
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

    /* 
		当输入范围为[0, 0.5*ln2]时，cosh应采用如下的近似公式计算：
		cosh(x) = 1  + (e^x - 1)^2 / (2*e^x)
	 */
	if(ix<0x3fd62e43) {
	    t = expm1(fabs(x));
	    w = one+t;
		/*
			当输入值趋近于0时，cosh的值趋近于1
		*/
	    if (ix<0x3c800000) return w;	/* cosh(tiny) = 1 */
	    return one+(t*t)/(w+w);
	}

    /* 
		当|x|在[0.5*ln2, 22]内时，该范围适用cosh函数定义公式进行计算，即
			cosh(x) = (e^-x + e^x) / 2
	 */
	if (ix < 0x40360000) {
		t = __ieee754_exp(fabs(x));
		return half*t+half/t;
	}

    /* 
		对于较大值的|x|（22≤|x|<log(maxdouble)），函数cosh(x) 应该使用近似公式 cosh(x) ≈ e^x / 2 进行计算
	*/
	if (ix < 0x40862E42)  return half*__ieee754_exp(fabs(x));

    /* 
		对于|x|接近机器浮点数表示极限值（log(maxdouble)≤|x|≤overflowthresold）的情况，函数cosh(x) 应该使用调整后的指数计算公式以避免溢出并保持数值稳定性：
		cosh(x) = e^(x/2) / 2 * e^(x/2)
	*/
	if (ix<=0x408633CE)
	    return __ldexp_exp(fabs(x), -1);

    /* 
		输入范围为|x|>overflowthresold时，在这个范围内，x非常大，cosh(x)的结果会非常大，接近于计算机能表示的最大数值。返回一个极大的数值来表示溢出情况,即返回1.0e307*1.0e307
	*/
	return huge*huge;
}