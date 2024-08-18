#include "cdefs-compat.h"
#include <MATH_math.h>

#include "math_private.h"

static const u_int32_t k = 1799;		/* constant for reduction */
static const double kln2 =  1246.97177782734161156;	/* k * ln2 */

/*
 * Compute exp(x), scaled to avoid spurious overflow.  An exponent is
 * returned separately in 'expt'.
 *
 * Input:  ln(DBL_MAX) <= x < ln(2 * DBL_MAX / DBL_MIN_DENORM) ~= 1454.91
 * Output: 2**1023 <= y < 2**1024
 */
/*
	__frexp_exp是对大浮点数自然指数计算的一种精确实现。它的核心思想是将输入的浮点数进行按照规定常数进行缩放，计算exp值后进行还原，这样可以避免输入过大造成的精度损失和上溢
*/
static double
__frexp_exp(double x, int *expt)
{
	double exp_x;
	u_int32_t hx;

	/*
	 * We use exp(x) = exp(x - kln2) * 2**k, carefully chosen to
	 * minimize |exp(kln2) - 2**k|.  We also scale the exponent of
	 * exp_x to MAX_EXP so that the result can be multiplied by
	 * a tiny number without losing accuracy due to denormalization.
	 */
	exp_x = exp(x - kln2);
	GET_HIGH_WORD(hx, exp_x);
	*expt = (hx >> 20) - (0x3ff + 1023) + k;
	SET_HIGH_WORD(exp_x, (hx & 0xfffff) | ((0x3ff + 1023) << 20));
	return (exp_x);
}

/*
 * __ldexp_exp(x, expt) and __ldexp_cexp(x, expt) compute exp(x) * 2**expt.
 * They are intended for large arguments (real part >= ln(DBL_MAX))
 * where care is needed to avoid overflow.
 *
 * The present implementation is narrowly tailored for our hyperbolic and
 * exponential functions.  We assume expt is small (0 or -1), and the caller
 * has filtered out very large x, for which overflow would be inevitable.
 */

/*
	__frexp_exp是对大浮点数x的exp(x) * 2^ex_expt计算的一种精确实现。它的核心思想是将输入的浮点数进行尾数和指数分离，先计算指数部分的计算结果后与尾数部分相乘，这样可以避免输入过大造成的精度损失和上溢
*/
OLM_DLLEXPORT double
__ldexp_exp(double x, int expt)
{
	double exp_x, scale;
	int ex_expt;

	exp_x = __frexp_exp(x, &ex_expt);
	expt += ex_expt;
	INSERT_WORDS(scale, (0x3ff + expt) << 20, 0);
	return (exp_x * scale);
}
