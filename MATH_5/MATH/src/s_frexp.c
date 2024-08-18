#include "cdefs-compat.h"
/*
 * for non-zero x
 *	x = frexp(arg,&exp);
 * return a double fp quantity x such that 0.5 <= |x| <1.0
 * and the corresponding binary exponent "exp". That is
 *	arg = x*2^exp.
 * If arg is inf, 0.0, or NaN, then frexp(arg,&exp) returns arg
 * with *exp=0.
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double
two54 =  1.80143985094819840000e+16; /* 0x43500000, 0x00000000 */
/*
	s_frexp.c的主要作用是将一个双精度浮点数分解为一个尾数和一个指数，使得该浮点数可以表示为尾数乘以二的指数次方的形式。这在需要对数值进行详细分析或精细控制时非常有用。例如，在某些算法中，需要将浮点数分解为尾数和指数，以便进行逐位操作或避免浮点数下溢和上溢。frexp 函数的定义域是所有双精度浮点数，包括：正负零（0.0 和 -0.0）、正负无穷大（+∞ 和 -∞）、非数字（NaN）、正常浮点数（normal numbers）、次正规浮点数（subnormal numbers）。frexp函数的值域包括两个部分：
	1.尾数 (x)：对于非零的正常浮点数，返回的尾数满足0.5≤∣x∣<1.0。对于零、无穷大和 NaN，尾数直接返回输入值。
	2.指数 (exp)：对于正常浮点数，指数是一个整数，表示浮点数的二进制指数。对于次正规数，指数经过调整，使得尾数在规范化范围内。对于零、无穷大和 NaN，指数返回 0。
*/
OLM_DLLEXPORT double
frexp(double x, int *eptr)
{
	int32_t hx, ix, lx;
	EXTRACT_WORDS(hx,lx,x);
	ix = 0x7fffffff&hx;
	*eptr = 0;
	/*
		frexp函数对于输入Inf、0、NaN，应直接返回x，并将指数位置为0
	*/
	if(ix>=0x7ff00000||((ix|lx)==0)) return x;	/* 0,inf,nan */
	/*
		对于尾数部分不符合规格化浮点数的情况，先进行规格话，然后提取尾数和指数
	*/
	if (ix<0x00100000) {		/* subnormal */
	    x *= two54;
	    GET_HIGH_WORD(hx,x);
	    ix = hx&0x7fffffff;
	    *eptr = -54;
	}
	/*
		对于尾数部分符合规格化浮点数的情况，直接提取尾数和指数
	*/
	*eptr += (ix>>20)-1022;
	hx = (hx&0x800fffff)|0x3fe00000;
	SET_HIGH_WORD(x,hx);
	return x;
}