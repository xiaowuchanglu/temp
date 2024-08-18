#include "cdefs-compat.h"
/* __ieee754_asin(x)
 * Method :                  
 *	Since  asin(x) = x + x^3/6 + x^5*3/40 + x^7*15/336 + ...
 *	we approximate asin(x) on [0,0.5] by
 *		asin(x) = x + x*x^2*R(x^2)
 *	where
 *		R(x^2) is a rational approximation of (asin(x)-x)/x^3 
 *	and its remez error is bounded by
 *		|(asin(x)-x)/x^3 - R(x^2)| < 2^(-58.75)
 *
 *	For x in [0.5,1]
 *		asin(x) = pi/2-2*asin(sqrt((1-x)/2))
 *	Let y = (1-x), z = y/2, s := sqrt(z), and pio2_hi+pio2_lo=pi/2;
 *	then for x>0.98
 *		asin(x) = pi/2 - 2*(s+s*z*R(z))
 *			= pio2_hi - (2*(s+s*z*R(z)) - pio2_lo)
 *	For x<=0.98, let pio4_hi = pio2_hi/2, then
 *		f = hi part of s;
 *		c = sqrt(z) - f = (z-f*f)/(s+f) 	...f+c=sqrt(z)
 *	and
 *		asin(x) = pi/2 - 2*(s+s*z*R(z))
 *			= pio4_hi+(pio4-2s)-(2s*z*R(z)-pio2_lo)
 *			= pio4_hi+(pio4-2f)-(2s*z*R(z)-(pio2_lo+2c))
 *
 * Special cases:
 *	if x is NaN, return x itself;
 *	if |x|>1, return NaN with invalid signal.
 *
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
huge =  1.000e+300,
pio2_hi =  1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
pio2_lo =  6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
pio4_hi =  7.85398163397448278999e-01, /* 0x3FE921FB, 0x54442D18 */
/*定义系数：pS0到pS5是有理近似分式分母的系数，qS0到qS4是有理近似分式分子的系数。
这些系数是提前计算并储存的，这些系数对应于有理分式系数的优化版本。
它们是精确到足够小的误差范围内预先计算好的，这样做既保证了计算的速度，也保证了精度。*/
pS0 =  1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
pS2 =  2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
pS4 =  7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
pS5 =  3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
qS2 =  2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */
/*e_asin.c是对反正弦函数在不同输入范围下的计算方法的一种高效实现，适用于所有实数输入。
它的核心思想是将输入的角度值通过范围约简，将其转换到[-π/4, π/4]区间内，再应用泰勒级数和有理近似来得到结果。
它巧妙地利用了数学上的泰勒级数展开和有理近似，以及计算机科学中的浮点数表示和精度控制技术。*/
OLM_DLLEXPORT double
__ieee754_asin(double x)
{
	double t=0.0,w,p,q,c,r,s;
	int32_t hx,ix;
	/*得到浮点数的高位部分*/
	GET_HIGH_WORD(hx,x);
	/*得到高位的绝对值*/	
	ix = hx&0x7fffffff;		
	if(ix>= 0x3ff00000) {
	    u_int32_t lx;
	    GET_LOW_WORD(lx,x);	
		/*|x|==1时，直接返回+-π/2*/
	    if(((ix-0x3ff00000)|lx)==0)	
			return x*pio2_hi+x*pio2_lo;	
		/*|x|>1时，不在定义范围内，构造sNaN作为异常处理返回*/
		if isnan((x-x)/(x-x))	{
			double result = 0.0;
			int32_t high_word = 0x7FF40000;
			int32_t low_word = 0x00000000;
			SET_HIGH_WORD(result, high_word);
			SET_LOW_WORD(result, low_word);
			return result;
		} else {
			return (x-x)/(x-x);
		}
	} else if (ix<0x3fe00000) {	
		/*|x| < 2^-26，即x非常趋近于0时，直接返回x作为反正弦的近似值*/
	    if(ix<0x3e500000) {
			if(huge+x>one) 
				return x;
	    }
		/*0<|x|<0.5时，采用asin(x) = x + x * x^2 * R(x^2)进行近似
		其中R(x^2)是(asin(x)-x)/x^3的有理近似，采用p/q进行近似*/
	    t = x*x;
	    p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	    q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	    w = p/q;
	    return x+x*w;
	}
	/*0.5<=|x|<1时，利用公式asin(x) = π/2 - 2asin(sqrt((1-x)/2))进行映射
	若0.975<=|x|<1，直接利用有理分式s + s * (p/q)来近似asin(sqrt((1-x)/2))
	若0.5<=|x|<0.975，将sqrt((1-x)/2)分为高位和尾数进行误差处理，防止对浮点数直接进行运算引起的舍入等误差*/
	w = one-fabs(x);
	t = w*0.5;
	p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	s = sqrt_my(t);
	if(ix>=0x3FEF3333) {	//|x| >= 0.975
	    w = p/q;
	    t = pio2_hi-(2.0*(s+s*w)-pio2_lo);
	} else {
	    w  = s;
	    SET_LOW_WORD(w,0);
	    c  = (t-w*w)/(s+w);
	    r  = p/q;
	    p  = 2.0*s*r-(pio2_lo-2.0*c);
	    q  = pio4_hi-2.0*w;
	    t  = pio4_hi-(p-q);
	}    
	/*利用asin(-x) = -asin(x)，根据输入的符号决定最后返回值的符号*/
	if(hx>0) return t; else return -t;    
}