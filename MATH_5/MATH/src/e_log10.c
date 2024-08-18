#include "cdefs-compat.h"

/*
 * Return the base 10 logarithm of x.  See e_log.c and k_log.h for most
 * comments.
 *
 *    log10(x) = (f - 0.5*f*f + k_log1p(f)) / ln10 + k * log10(2)
 * in not-quite-routine extra precision.
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"
#include "k_log.h"

static const double
two54      =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
ivln10hi   =  4.34294481878168880939e-01, /* 0x3fdbcb7b, 0x15200000 */
ivln10lo   =  2.50829467116452752298e-11, /* 0x3dbb9438, 0xca9aadd5 */
log10_2hi  =  3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
log10_2lo  =  3.69423907715893078616e-13; /* 0x3D59FEF3, 0x11F12B36 */

static const double zero   =  0.0;
/*
	e_log10.c是是用于计算以10为底的对数。以10为底的对数的定义域是正实数集合 (0, +∞)，值域是实数集合 (-∞, +∞)。Log10函数采用了一种称为“对数恒等式”的方法来进行计算。这种方法可以提高计算的精度，尤其是对于非常大或非常小的数。
	已知对数的性质:log10(xy) = log10(x) + log10(y)。使用这个性质，可以将x分解为x = 2^k * f，其中f在[1,2)之间。因此，以10为底的对数函数值计算公式可近似为：
		log10(x) = k * log10(2) + log10(f)
	下面是这个函数的详细描述：
	对于x < 2^-1022 (非常小的值)的输入值，x被视为次正规数。函数首先检查x是否为0，因为log(0)在数学上是未定义的，log10返回负无穷大。如果x是负数，函数直接返回特殊值NaN。如果x是非常小的正数，函数将x乘以2^54，使其变为一个正规数，同时调整指数k减去54，保持值的等效性。
	对于x >= 2^1024 (非常大的值)的输入值，当x非常大时，超过了浮点数的正常表示范围，函数直接返回x+x，这通常会导致返回正无穷大。
	对于x = 1的输入值，log10(1)等于0，函数直接返回0。
	对于其他正常范围内的x，函数采用以下步骤和公式计算对数：
	首先，将x规范化。任何浮点数都可以表示为介于1和2之间的数m乘以2的整数次幂e的形式，因此规范化公式为：x = m * 2^e(1 <= m <=2)，这里m是尾数，e是指数。接着将尾数部分m转换为接近1的形式，转换公式为f = m - 1，将 m 转换为 1 + f 的形式，使 f 接近 0，这样后续计算中的误差会较小。
	然后使用泰勒展开式来近似计算自然对数。当f非常小时，泰勒展开式可以提供对的高精度近似。
	然后，将自然对数转换为以10为底的对数，转换公式为log10(x) = log(x) / log(10)。根据对数的换底公式，可以通过自然对数和常数之间的比例关系将自然对数转换为常用对数。
	接着，考虑到规范化时的指数部分，根据对数的乘法性质，将乘法转换为加法：log10(x) = log10(m) + e * log10(2)。
*/
OLM_DLLEXPORT double
__ieee754_log10(double x)
{
	double f,hfsq,hi,lo,r,val_hi,val_lo,w,y,y2;
	int32_t i,k,hx;
	u_int32_t lx;

	EXTRACT_WORDS(hx,lx,x);

	k=0;
	if (hx < 0x00100000) {			/* x < 2**-1022  */
		/*
			对于输入值为0时，函数应返回负无穷
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
		}		/* log(-#) = NaN */
		/*
			将非规格化数规格化
		*/
	    k -= 54; x *= two54; /* subnormal number, scale up x */
	    GET_HIGH_WORD(hx,x);
	}
	/*
		当输入值为正无穷时，函数应返回正无穷
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
		对于输入值为1时，函数应返回0，即：
	*/
	if (hx == 0x3ff00000 && lx == 0)
	    return zero;			/* log(1) = +0 */
	/*
		进行参数规约，将x规约为2^k * (1 + f)
	*/
	k += (hx>>20)-1023;
	hx &= 0x000fffff;
	i = (hx+0x95f64)&0x100000;
	SET_HIGH_WORD(x,hx|(i^0x3ff00000));	/* normalize x or x/2 */
	k += (i>>20);
	y = (double)k;
	f = x - 1.0;
	hfsq = 0.5*f*f;
	r = k_log1p(f);
	/*
		计算log10(x) = log(1 + f) / log(10) + k * log10(2) = k_log1p(f) / log(10) + k * log10(2)
	*/
	/* See e_log2.c for most details. */
	hi = f - hfsq;
	SET_LOW_WORD(hi,0);
	lo = (f - hi) - hfsq + r;
	val_hi = hi*ivln10hi;
	y2 = y*log10_2hi;
	val_lo = y*log10_2lo + (lo+hi)*ivln10lo + lo*ivln10hi;

	/*
	 * Extra precision in for adding y*log10_2hi is not strictly needed
	 * since there is no very large cancellation near x = sqrt(2) or
	 * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
	 * with some parallelism and it reduces the error for many args.
	 */
	/*
		为获得更高精度分解为高位和低位误差项进一步优化
	*/
	w = y2 + val_hi;
	val_lo += (y2 - w) + val_hi;
	val_hi = w;

	return val_lo + val_hi;
}