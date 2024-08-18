#include "cdefs-compat.h"

/* __ieee754_log(x)
 * Return the logrithm of x
 *
 * Method :                  
 *   1. Argument Reduction: find k and f such that 
 *			x = 2^k * (1+f), 
 *	   where  sqrt(2)/2 < 1+f < sqrt(2) .
 *
 *   2. Approximation of log(1+f).
 *	Let s = f/(2+f) ; based on log(1+f) = log(1+s) - log(1-s)
 *		 = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *	     	 = 2s + s*R
 *      We use a special Reme algorithm on [0,0.1716] to generate 
 * 	a polynomial of degree 14 to approximate R The maximum error 
 *	of this polynomial approximation is bounded by 2**-58.45. In
 *	other words,
 *		        2      4      6      8      10      12      14
 *	    R(z) ~ Lg1*s +Lg2*s +Lg3*s +Lg4*s +Lg5*s  +Lg6*s  +Lg7*s
 *  	(the values of Lg1 to Lg7 are listed in the program)
 *	and
 *	    |      2          14          |     -58.45
 *	    | Lg1*s +...+Lg7*s    -  R(z) | <= 2 
 *	    |                             |
 *	Note that 2s = f - s*f = f - hfsq + s*hfsq, where hfsq = f*f/2.
 *	In order to guarantee error in log below 1ulp, we compute log
 *	by
 *		log(1+f) = f - s*(f - R)	(if f is not too large)
 *		log(1+f) = f - (hfsq - s*(hfsq+R)).	(better accuracy)
 *	
 *	3. Finally,  log(x) = k*ln2 + log(1+f).  
 *			    = k*ln2_hi+(f-(hfsq-(s*(hfsq+R)+k*ln2_lo)))
 *	   Here ln2 is split into two floating point number: 
 *			ln2_hi + ln2_lo,
 *	   where n*ln2_hi is always exact for |n| < 2000.
 *
 * Special cases:
 *	log(x) is NaN with signal if x < 0 (including -INF) ; 
 *	log(+INF) is +INF; log(0) is -INF with signal;
 *	log(NaN) is that NaN with no signal.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
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
ln2_hi  =  6.93147180369123816490e-01,	/* 3fe62e42 fee00000 */
ln2_lo  =  1.90821492927058770002e-10,	/* 3dea39ef 35793c76 */
two54   =  1.80143985094819840000e+16,  /* 43500000 00000000 */
Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

static const double zero   =  0.0;
/*
	Log函数是用于计算自然对数（以e为底数）的函数。自然对数的定义域是正实数集合 (0, +∞)，值域是实数集合 (-∞, +∞)。Log函数采用了一些数学技巧和近似来计算任意正实数 x 的自然对数。下面是这个函数的详细描述：
	1.参数缩减：将输入的一个数字x重整化为 x = 2^k * (1 + f) 的形式，并且满足sqrt(2)/2 < 1+f < sqrt(2)的条件。以确保1+f的大小足够合适，保证后续换元操作的泰勒展开精度不会被影响。然后很容易发现规律：ln(x) = k*ln2 + ln(1 + f)，其中可直接计算得到，接下来则需进一步计算的值。
	2.对数近似：计算ln(1 + f)的值，首先（1+f）因范围控制得比较小，所以很容易近似到一个很精确的值。计算方法为：因为泰勒对ln(x)函数的展开在x接近1的时候最好，因此取中间代换变量s = f / (2 + f)。基于对数恒等式 log(1+f) 则有泰勒展开为：
		ln(1 + f) = ln(1 + s) - ln(1 - s) = 2s + 2 / 3s^2 + ... = 2s + s * R(z)
	因此函数可以简化为。这里使用 Remez算法在[0, 0.1716]区间生成一个14阶多项式来近似 R。此时这个多项式近似的最大误差被限制在2^-58.45以内，满足64位浮点运算要求。
	为确保对数计算的误差小于1ulp（单位最后一位），计算 log(1+f) 有两种方式：
	当 f 不是很大时：可根据s = f / (2 + f)，进一步得到代换关系：2s = f - sf，于是有：ln(1 + f) = 2s + s*R(z) = f - ff/2 + sff/2 + s*R(z),ln(x) = kln2 + ln(1+f)
*/
OLM_DLLEXPORT double
__ieee754_log(double x)
{
	double hfsq,f,s,z,R,w,t1,t2,dk;
	int32_t k,hx,i,j;
	u_int32_t lx;

	EXTRACT_WORDS(hx,lx,x);

	k=0;
	if (hx < 0x00100000) {			/* x < 2**-1022  */
		/*
			对于输入值为0时，函数应返回负无穷。
		*/
	    if (((hx&0x7fffffff)|lx)==0) 
			return -two54/zero;		/* log(+-0)=-inf */
		/*
			当输入范围为x < 0（包括负无穷大）时，函数应返回NaN
		*/
	    if (hx<0) 
		{
			double result = 0.0 / 0.0;
			int32_t high_word = 0x7FF40000;
			int32_t low_word = 0x00000000;
			SET_HIGH_WORD(result, high_word);
			SET_LOW_WORD(result, low_word);
			return result;
		}	/* log(-#) = NaN */
		/*
			对非规格化数，先进行规格化
		*/
	    k -= 54; x *= two54; /* subnormal number, scale up x */
	    GET_HIGH_WORD(hx,x);
	} 
	/* 
		由于log(x)是增函数，且随x的增大而增大，因此log(+inf) = +inf。
	*/
	if (hx == 0x7ff00000 && lx == 0)
	{
		return x;
	} else if (hx >= 0x7ff00000)	//NaN
	{
		/*
			当输入值x为 NaN 时，函数应返回 NaN
		*/
		double result = x + x;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}
	/*
		进行参数约减，将x约减为2^k * (1 + f)
	*/
	k += (hx>>20)-1023;
	hx &= 0x000fffff;
	i = (hx+0x95f64)&0x100000;
	SET_HIGH_WORD(x,hx|(i^0x3ff00000));	/* normalize x or x/2 */
	k += (i>>20);
	f = x-1.0;
	/*
		当f是0或f很小时，近似为k * ln2 + 误差项
	*/
	if((0x000fffff&(2+hx))<3) {	/* -2**-20 <= f < 2**-20 */
	    if(f==zero) {
			if(k==0) {
				return zero;
			} else {
				dk=(double)k;
				return dk*ln2_hi+dk*ln2_lo;
			}
	    }
	    R = f*f*(0.5-0.33333333333333333*f);
	    if(k==0) return f-R; else {dk=(double)k;
	    	     return dk*ln2_hi-((R-dk*ln2_lo)-f);}
	}
	/*
		根据 s = f / (2 + f)，得到代换关系 2s = f - sf
		ln(1 + f) = 2s + s * R(z)
		ln(x) = kln2 + ln(1 + f)
	*/
 	s = f/(2.0+f); 
	dk = (double)k;
	z = s*s;
	i = hx-0x6147a;
	w = z*z;
	j = 0x6b851-hx;
	t1= w*(Lg2+w*(Lg4+w*Lg6)); 
	t2= z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7))); 
	i |= j;
	R = t2+t1;
	if(i>0) {
	    hfsq=0.5*f*f;
	    if(k==0) return f-(hfsq-s*(hfsq+R)); else
		     return dk*ln2_hi-((hfsq-(s*(hfsq+R)+dk*ln2_lo))-f);
	} else {
	    if(k==0) return f-s*(f-R); else
		     return dk*ln2_hi-((s*(f-R)-dk*ln2_lo)-f);
	}
}