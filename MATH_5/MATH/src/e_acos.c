#include "cdefs-compat.h"
//__FBSDID("$FreeBSD: src/lib/msun/src/e_acos.c,v 1.13 2008/07/31 22:41:26 das Exp $");

/* __ieee754_acos(x)
 * Method :                  
 *	acos(x)  = pi/2 - asin(x)
 *	acos(-x) = pi/2 + asin(x)
 * For |x|<=0.5
 *	acos(x) = pi/2 - (x + x*x^2*R(x^2))	(see asin.c)
 * For x>0.5
 * 	acos(x) = pi/2 - (pi/2 - 2asin(sqrt((1-x)/2)))
 *		= 2asin(sqrt((1-x)/2))  
 *		= 2s + 2s*z*R(z) 	...z=(1-x)/2, s=sqrt(z)
 *		= 2f + (2c + 2s*z*R(z))
 *     where f=hi part of s, and c = (z-f*f)/(s+f) is the correction term
 *     for f so that f+c ~ sqrt(z).
 * For x<-0.5
 *	acos(x) = pi - 2asin(sqrt((1-|x|)/2))
 *		= pi - 0.5*(s+s*z*R(z)), where z=(1-|x|)/2,s=sqrt(z)
 *
 * Special cases:
 *	if x is NaN, return x itself;
 *	if |x|>1, return NaN with invalid signal.
 *
 * Function needed: sqrt
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double
one=  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
pi =  3.14159265358979311600e+00, /* 0x400921FB, 0x54442D18 */
pio2_hi =  1.57079632679489655800e+00; /* 0x3FF921FB, 0x54442D18 */
static volatile double
pio2_lo =  6.12323399573676603587e-17; /* 0x3C91A626, 0x33145C07 */
static const double
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

/*
	e_cos.c计算的是一个数x的反余弦值，x的范围在 [-1, 1] 之间。反余弦函数acos(x)定义为余弦函数cos(θ)的反函数，其作用是找到一个角度θ，使得cos(θ) = x。这个角度θ的值就是x的反余弦值。反余弦函数的主值域是从0到π。它针对x的不同范围使用了不同的近似方法：
	当|x| >= 1时，如果x == 1，那么acos(1) = 0，因为cos(0) = 1。如果x == -1，那么acos(-1) = π，因为cos(π) = -1。如果|x| > 1，由于cos(θ)的值域是[-1, 1]，acos(x)在数学上是未定义的，因此函数返回NaN表示这是一个无效输入。
	对于|x| < 0.5这个范围内，acos(x)可以通过近似为π/2 - (x + x*x^2*R(x^2))，其中R(x^2)是一个关于x^2的多项式。这个近似基于asin(x)的泰勒级数展开，并利用了acos(x) = π/2 - asin(x)的关系。
	对于x > 0.5的情况,在这个范围内，acos(x)可以利用半角公式sin^2(θ) = (1 - cos(2θ))/2来表示，从而得到acos(x) = 2asin(sqrt((1-x)/2))。这个公式利用了asin和acos之间的关系，以及三角恒等式来简化计算。通过求出sqrt((1-x)/2)的反正弦值，然后将其乘以2，我们得到了acos(x)的值。
	对于x < -0.5的情况,利用三角函数的性质和对称性，acos(x)可以表示为π - 2asin(sqrt((1-|x|)/2))。这个公式同样基于半角公式和acos与asin之间的关系，但是由于x是负数，我们需要求出|x|的值来确保平方根内的表达式是正数。
*/
OLM_DLLEXPORT double
__ieee754_acos(double x)
{
	double z,p,q,r,w,s,c,df;
	int32_t hx,ix;
	GET_HIGH_WORD(hx,x);
	ix = hx&0x7fffffff;
	if(ix>=0x3ff00000) {	/* |x| >= 1 */
	    u_int32_t lx;
	    GET_LOW_WORD(lx,x);
	    if(((ix-0x3ff00000)|lx)==0) {	/* |x|==1 */
			/*
				当输入值x等于1时，函数应返回0。
			*/
			if(hx>0) return 0.0;		/* acos(1) = 0  */
			/*
				当输入值x等于-1时，函数应返回π。
			*/
			else return pi+2.0*pio2_lo;	/* acos(-1)= pi */
	    }
		/*
			对于|x| > 1的输入，不存在有效的反余弦值。	
		*/
		if isnan(x)	{ 		/* acos(|x|>1) is NaN */
			x = 0.0;
		} 
		double result = (x-x)/(x-x);
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}
	if(ix<0x3fe00000) {	/* |x| < 0.5 */
		/*
			对于|x| < 2^-57的输入，反余弦函数的值与π/2近似相等
		*/
	    if(ix<=0x3c600000) return pio2_hi+pio2_lo;/*if|x|<2**-57*/
		/*
			对于接近0的x值，acos(x)的泰勒级数展开能够提供一个有效的近似。
			在|x| < 0.5的范围内，这种近似特别有效，因为泰勒级数收敛速度较快，可以用较少的项数达到较高的精度。
		*/
	    z = x*x;
	    p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	    q = one+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	    r = p/q;
	    return pio2_hi - (x - (pio2_lo-x*r));
	} else  if (hx<0) {		/* x < -0.5 */
		/*
			对于在(-1.0, -0.5)范围内的x，函数应利用acos(x) = 2asin(sqrt((1-|x|)/2))的公式来计算反余弦值。
		*/
	    z = (one+x)*0.5;
	    p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	    q = one+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	    s = sqrt_my(z);
	    r = p/q;
	    w = r*s-pio2_lo;
	    return pi - 2.0*(s+w);
	} else {			/* x > 0.5 */
		/*
			对于在(0.5, 1.0)范围内的x，函数应利用acos(x) = 2asin(sqrt((1-|x|)/2))的公式来计算反余弦值。
		*/
	    z = (one-x)*0.5;
	    s = sqrt_my(z);
	    df = s;
	    SET_LOW_WORD(df,0);
	    c  = (z-df*df)/(s+df);
	    p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	    q = one+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	    r = p/q;
	    w = r*s+c;
	    return 2.0*(df+w);
	}
}
