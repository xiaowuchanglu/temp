
#include "cdefs-compat.h"
/* atan(x)
 * Method
 *   1. Reduce x to positive by atan(x) = -atan(-x).
 *   2. According to the integer k=4t+0.25 chopped, t=x, the argument
 *      is further reduced to one of the following intervals and the
 *      arctangent of t is evaluated by the corresponding formula:
 *
 *      [0,7/16]      atan(x) = t-t^3*(a1+t^2*(a2+...(a10+t^2*a11)...)
 *      [7/16,11/16]  atan(x) = atan(1/2) + atan( (t-0.5)/(1+t/2) )
 *      [11/16.19/16] atan(x) = atan( 1 ) + atan( (t-1)/(1+t) )
 *      [19/16,39/16] atan(x) = atan(3/2) + atan( (t-1.5)/(1+1.5t) )
 *      [39/16,INF]   atan(x) = atan(INF) + atan( -1/t )
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

static const double atanhi[] = {
  4.63647609000806093515e-01, /* atan(0.5)hi 0x3FDDAC67, 0x0561BB4F */
  7.85398163397448278999e-01, /* atan(1.0)hi 0x3FE921FB, 0x54442D18 */
  9.82793723247329054082e-01, /* atan(1.5)hi 0x3FEF730B, 0xD281F69B */
  1.57079632679489655800e+00, /* atan(inf)hi 0x3FF921FB, 0x54442D18 */
};

static const double atanlo[] = {
  2.26987774529616870924e-17, /* atan(0.5)lo 0x3C7A2B7F, 0x222F65E2 */
  3.06161699786838301793e-17, /* atan(1.0)lo 0x3C81A626, 0x33145C07 */
  1.39033110312309984516e-17, /* atan(1.5)lo 0x3C700788, 0x7AF0CBBD */
  6.12323399573676603587e-17, /* atan(inf)lo 0x3C91A626, 0x33145C07 */
};

static const double aT[] = {
  3.33333333333329318027e-01, /* 0x3FD55555, 0x5555550D */
 -1.99999999998764832476e-01, /* 0xBFC99999, 0x9998EBC4 */
  1.42857142725034663711e-01, /* 0x3FC24924, 0x920083FF */
 -1.11111104054623557880e-01, /* 0xBFBC71C6, 0xFE231671 */
  9.09088713343650656196e-02, /* 0x3FB745CD, 0xC54C206E */
 -7.69187620504482999495e-02, /* 0xBFB3B0F2, 0xAF749A6D */
  6.66107313738753120669e-02, /* 0x3FB10D66, 0xA0D03D51 */
 -5.83357013379057348645e-02, /* 0xBFADDE2D, 0x52DEFD9A */
  4.97687799461593236017e-02, /* 0x3FA97B4B, 0x24760DEB */
 -3.65315727442169155270e-02, /* 0xBFA2B444, 0x2C6A6C2F */
  1.62858201153657823623e-02, /* 0x3F90AD3A, 0xE322DA11 */
};

	static const double
one   = 1.0,
huge   = 1.0e300;
/*
	s_atan.c计算的是一个数x的反正切值，x的范围在 [-Inf, +Inf] 之间，值域在 [- pi / 2, pi / 2] 。函数接受一个double类型的参数x，返回x的反正切值。计算过程分为以下几个步骤：
	·归一化到正值：由于正切函数在第一和第四象限是增加的，并且满足奇函数的性质（即tan(-θ) = -tan(θ)），我们可以通过计算x的正值的反正切，并在必要时调整最终结果的符号，来简化问题。这就是为什么函数一开始会检查x的符号，并在最后根据原始符号返回正值或负值的原因。
	·范围缩减：直接计算atan(x)对于很大或很小的x值来说可能会导致数值稳定性问题。为了解决这个问题，利用反正切函数的一些性质将x的值缩减到一个更小的范围内，从而提高计算的精度。函数中的缩减步骤是通过将x映射到几个特定的区间内，并使用相应的公式进行处理。主要分为以下几个区间：
	对于小的x值（[0,7/16]）,直接使用x来计算反正切值，因为在这个区间内，反正切函数可以很好地被其泰勒级数近似，由于x的值较小，泰勒级数收敛得很快，因此截断误差通常也很小。
	对于[7/16, 11/16]这个区间内的输入，x值相对较大，直接使用泰勒级数可能会导致收敛较慢，因此采用了角度加法公式来简化问题：
		arctan(x)=arctan(1/2) + arctan((x - 0.5) / (1 + pi / 2))
	对于区间 [11/16, 19/16]的输入，采用的公式是：
		arctan(x) = arctan(1) + arctan((x - 1) / (x + 1))
	对于区间 [19/16, 39/16]的输入，采用的公式是：
		arctan(x) = arctan(1.5) + arctan((x - 1.5) / (1 + 1.5x))
	对于非常大的x值([39/16, ∞])，直接计算atan(x)会变得复杂且低效。在这个区间，使用了：
		π/2−atan(1/x)
	由于反正切函数的性质，当x趋于无穷大时，趋向于，所以可以用减去—个较小角度的反正切值来求解。
*/
OLM_DLLEXPORT double
atan_my(double x)
{
	double w,s1,s2,z;
	int32_t ix,hx,id;

	GET_HIGH_WORD(hx,x);	// 获取参数 x 的浮点表示的高位
	ix = hx&0x7fffffff;		//去除符号位
	if(ix>=0x44100000) {	// if |x| >= 2^66
	    u_int32_t low;
	    GET_LOW_WORD(low,x);
	    if(ix>0x7ff00000||
		(ix==0x7ff00000&&(low!=0)))
		{
			/*
				反正切函数对于输入NaN，根据IEEE 754标准，应返回SNaN。
			*/
			double result = x+x;
			int32_t high_word = 0x7FF40000;
			int32_t low_word = 0x00000000;
			SET_HIGH_WORD(result, high_word);
			SET_LOW_WORD(result, low_word);
			return result;
		}	/* NaN */
		/*
			当x的绝对值非常大（|x| >= 2^66）时，如果x不是NaN且x是正数，则返回atan(inf)= π/ 2 。
		*/
	    if(hx>0) return  atanhi[3]+*(volatile double *)&atanlo[3];
		/*
			当x的绝对值非常大（|x| >= 2^66）时，如果x不是NaN且x是负数，则返回atan(-inf)= -π/ 2 。
		*/
	    else     return -atanhi[3]-*(volatile double *)&atanlo[3];
	} 
	if (ix < 0x3fdc0000) {	/* |x| < 0.4375 */
	    if (ix < 0x3e400000) {	/* |x| < 2^-27 */
			/*
				当x非常小的时候，atan(x)函数在x=0处的泰勒级数的展开首项是x，而更高阶的项因为x的幂次增加而变得无足轻重。
				因此，可以近似地认为atan(x)≈x。这种近似大幅减少了计算时间并保持了高精度。
			*/
			if(huge+x>one) return x;	/* raise inexact */
	    }
		/*
			当输入值的绝对值位于区间[0, 7/16]内时，atan(x)函数使用泰勒级数近似法进行计算
		*/
	    id = -1;
	} else {
		x = fabs(x);
		if (ix < 0x3ff30000) {		/* |x| < 1.1875 */
			if (ix < 0x3fe60000) {	/* 7/16 <=|x|<11/16 */
				/*
					对于区间 [7/16, 11/16] 内的输入值，函数应使用角度加法公式进行计算，并以 arctan(1/2) 作为基准值，具体公式如下：
					arctan(x)=arctan(1/2) + arctan((x - 0.5) / (1 + pi / 2))
				*/
				id = 0; x = (2.0*x-one)/(2.0+x);
			} else {			/* 11/16<=|x|< 19/16 */
				/*
					输入值在 [11/16, 19/16] 区间内时，应使用arctan(1)作为角度加法公式的基准，具体公式如下：
					arctan(x) = arctan(1) + arctan((x - 1) / (x + 1))
				*/
				id = 1; x  = (x-one)/(x+one);
			}
		} else {
			if (ix < 0x40038000) {	/* |x| < 2.4375 */
				/*
					对于区间 [19/16, 39/16] 的输入值，函数应使用 atan(1.5) 作为角度加法公式的基准，具体公式如下：
					arctan(x) = arctan(1.5) + arctan((x - 1.5) / (1 + 1.5x))
				*/
				id = 2; 
				x  = (x-1.5)/(one+1.5*x);
			} else {			/* 2.4375 <= |x| < 2^66 */
				/*
					对于非常大的 x 值，反正切函数趋近于π/2。通过计算π/2−atan(1/x)，可以利用反正切函数的渐近性质，有效地计算出接近π/2的结果，同时保持高精度。
				*/
				id = 3; x  = -1.0/x;
			}
		}
	}
	/*
		利用预先计算的高精度系数进行泰勒近似。
	*/
    /* end of argument reduction */
	z = x*x;
	w = z*z;
    /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
	s1 = z*(aT[0]+w*(aT[2]+w*(aT[4]+w*(aT[6]+w*(aT[8]+w*aT[10])))));
	s2 = w*(aT[1]+w*(aT[3]+w*(aT[5]+w*(aT[7]+w*aT[9]))));
	if (id<0) return x - x*(s1+s2);
	else {
	    z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
	    return (hx<0)? -z:z;
	}
}
