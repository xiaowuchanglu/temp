#include "cdefs-compat.h"
/* __ieee754_exp(x)
 * Returns the exponential of x.
 *
 * Method
 *   1. Argument reduction:
 *      Reduce x to an r so that |r| <= 0.5*ln2 ~ 0.34658.
 *	Given x, find r and integer k such that
 *
 *               x = k*ln2 + r,  |r| <= 0.5*ln2.  
 *
 *      Here r will be represented as r = hi-lo for better 
 *	accuracy.
 *
 *   2. Approximation of exp(r) by a special rational function on
 *	the interval [0,0.34658]:
 *	Write
 *	    R(r**2) = r*(exp(r)+1)/(exp(r)-1) = 2 + r*r/6 - r**4/360 + ...
 *      We use a special Remes algorithm on [0,0.34658] to generate 
 * 	a polynomial of degree 5 to approximate R. The maximum error 
 *	of this polynomial approximation is bounded by 2**-59. In
 *	other words,
 *	    R(z) ~ 2.0 + P1*z + P2*z**2 + P3*z**3 + P4*z**4 + P5*z**5
 *  	(where z=r*r, and the values of P1 to P5 are listed below)
 *	and
 *	    |                  5          |     -59
 *	    | 2.0+P1*z+...+P5*z   -  R(z) | <= 2 
 *	    |                             |
 *	The computation of exp(r) thus becomes
 *                             2*r
 *		exp(r) = 1 + -------
 *		              R - r
 *                                 r*R1(r)	
 *		       = 1 + r + ----------- (for better accuracy)
 *		                  2 - R1(r)
 *	where
 *			         2       4             10
 *		R1(r) = r - (P1*r  + P2*r  + ... + P5*r   ).
 *	
 *   3. Scale back to obtain exp(x):
 *	From step 1, we have
 *	   exp(x) = 2^k * exp(r)
 *
 * Special cases:
 *	exp(INF) is INF, exp(NaN) is NaN;
 *	exp(-INF) is 0, and
 *	for finite argument, only exp(0)=1 is exact.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Misc. info.
 *	For IEEE double 
 *	    if x >  7.09782712893383973096e+02 then exp(x) overflow
 *	    if x < -7.45133219101941108420e+02 then exp(x) underflow
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double
one	= 1.0,
halF[2]	= {0.5,-0.5,},
huge	= 1.0e+300,
o_threshold=  7.09782712893383973096e+02,  /* 0x40862E42, 0xFEFA39EF */
u_threshold= -7.45133219101941108420e+02,  /* 0xc0874910, 0xD52D3051 */
ln2HI[2]   ={ 6.93147180369123816490e-01,  /* 0x3fe62e42, 0xfee00000 */
	     -6.93147180369123816490e-01,},/* 0xbfe62e42, 0xfee00000 */
ln2LO[2]   ={ 1.90821492927058770002e-10,  /* 0x3dea39ef, 0x35793c76 */
	     -1.90821492927058770002e-10,},/* 0xbdea39ef, 0x35793c76 */
invln2 =  1.44269504088896338700e+00, /* 0x3ff71547, 0x652b82fe */
P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
P5   =  4.13813679705723846039e-08; /* 0x3E663769, 0x72BEA4D0 */

static volatile double
twom1000= 9.33263618503218878990e-302;     /* 2**-1000=0x01700000,0*/
/*
	e_exp.c用于计算计算 x 的指数函数，即()，其中 e 是自然对数的底数。自然指数函数的定义域是实数集合(-∞, +∞)，值域是正实数集合(0, +∞)。Exp函数采用了一些数学技巧和近似来计算任意正实数 x 的自然指数值。下面是这个函数的详细描述：
	1.参数缩减：找到r和整数k，将输入的数字x 缩减为r，使得，并且满足0.3456的条件。
	2.近似计算exp(r)：函数在区间 [0, 0.34658] 上使用多项式近似来计算exp(r)。这种近似是通过一个特殊的有理函数R(r^2)导出的，通过特殊的Remes算法生成一个五阶多项式来近似R(r^2)。R(r^2)的近似形式为：

	其中z=r*r。多项式系数P1到P5通过Remes算法确定，式子可满足精度，此时这个多项式近似的最大误差被限制在2^-59以内，满足64位浮点运算要求。然后，exp(r)的计算被细化为：

	为了提高精度，使用修正的公式：

	其中R1(r)是从多项式近似导出的矫正项，即：

	3.缩放回原值以获得exp(x)：计算出exp(r)后，通过关系式缩放回原值exp(x)： 

	Exp函数对于特殊情况的处理包括：当x为正无穷大时，exp(+INF) 返回正无穷大。当x为负无穷大时，exp(-INF) 返回0。当x为0时，exp(0)返回1。当 x 为 NaN 时，exp(NaN) 返回同样的 NaN。
*/
OLM_DLLEXPORT double
__ieee754_exp(double x)	/* default IEEE double exp */
{
	double y,hi=0.0,lo=0.0,c,t,twopk;
	int32_t k=0,xsb;
	u_int32_t hx;

	GET_HIGH_WORD(hx,x);
	xsb = (hx>>31)&1;		/* sign bit of x */
	hx &= 0x7fffffff;		/* high word of |x| */

    /* filter out non-finite argument */
	if(hx >= 0x40862E42) {			/* if |x|>=709.78... */
		if(hx>=0x7ff00000) {
	        u_int32_t lx;
			GET_LOW_WORD(lx,x);
			/*
				当输入中包含 NaN 时，根据 IEEE 754 标准，任何涉及 NaN 的算术运算结果应为 NaN。
			*/
			if(((hx&0xfffff)|lx)!=0)
			{
				double result = x+x;
				int32_t high_word = 0x7FF40000;
				int32_t low_word = 0x00000000;
				SET_HIGH_WORD(result, high_word);
				SET_LOW_WORD(result, low_word);
				return result;
			}		/* NaN */
			/*
				对于x是+Inf的情况，exp函数的值与x近似相等，这一特性基于exp函数在Inf处趋近与Inf;
				对于x是-Inf的情况，exp函数的值与0.0近似相等
			*/
			else return (xsb==0)? x:0.0;	/* exp(+-inf)={inf,0} */
	    }
		/*
			对于x > 7.09782712893383973096e+02，计算exp(x)会超出double的范围，向上溢出。
		*/
	    if(x > o_threshold) return huge*huge; /* overflow */
		/*
			对于x < -7.45133219101941108420e+02，计算exp(x)会超出double的范围，向下溢出
		*/
	    if(x < u_threshold) return twom1000*twom1000; /* underflow */
	}

	/* this implementation gives 2.7182818284590455 for exp(1.0),
		which is well within the allowable error. however,
		2.718281828459045 is closer to the true value so we prefer that
		answer, given that 1.0 is such an important argument value. */
	/*
		对于x == 1.0 这种常见情况，给定更精准的计算结果以减少误差
	*/
	if (x == 1.0)
		return 2.718281828459045235360;

    /* argument reduction */
	if(hx > 0x3fd62e42) {		/* if  |x| > 0.5 ln2 */ 
		/*
			对于绝对值在[0.5 * ln2,1.5 * ln2]范围内的x，可以找到r和整数k，将输入的数字x 缩减为r，使得，并且满足0.3456的条件
		*/
	    if(hx < 0x3FF0A2B2) {	/* and |x| < 1.5 ln2 */
			hi = x-ln2HI[xsb]; 
			lo=ln2LO[xsb]; 
			k = 1-xsb-xsb;
	    } else {
		/*
			对于绝对值在[1.5 * 7.09782712893383973096e+02)范围内的x，可以找到r和整数k，将输入的数字x 缩减为r，使得，并且满足0.3456的条件
		*/
		k  = (int)(invln2*x+halF[xsb]);
		t  = k;
		hi = x - t*ln2HI[0];	/* t*ln2HI is exact here */
		lo = t*ln2LO[0];
	    }
	    x = hi - lo;
	} 
	/*
		对于非常小的输入值，exp函数的值与1.0 + x近似相等，这一特性基于exp函数在0附近的泰勒级数展开
	*/
	else if(hx < 0x3e300000)  {	/* when |x|<2**-28 */
	    if(huge+x>one) return one+x;/* trigger inexact */
	}
	/*
		对于绝对值在[2^-54,0.5*ln2]范围内的x，满足0.3456的条件，不需要进行规约，直接使用多项式近似来计算 exp(r)
	*/
	else k = 0;

	/*
		在规约范围内进行多项式近似
	*/
    /* x is now in primary range */
	t  = x*x;
	if(k >= -1021)
	    INSERT_WORDS(twopk,0x3ff00000+(k<<20), 0);
	else
	    INSERT_WORDS(twopk,0x3ff00000+((k+1000)<<20), 0);
	c  = x - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	if(k==0) 	return one-((x*c)/(c-2.0)-x); 
	else 		y = one-((lo-(x*c)/(2.0-c))-hi);
	if(k >= -1021) {
	    if (k==1024) return y*2.0*0x1p1023;
	    return y*twopk;
	} else {
	    return y*twopk*twom1000;
	}
}