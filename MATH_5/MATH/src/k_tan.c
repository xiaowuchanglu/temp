#include "cdefs-compat.h"
/* __kernel_tan( x, y, k )
 * kernel tan function on ~[-pi/4, pi/4] (except on -0), pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input k indicates whether tan (if k = 1) or -1/tan (if k = -1) is returned.
 *
 * Algorithm
 *	1. Since tan(-x) = -tan(x), we need only to consider positive x.
 *	2. Callers must return tan(-0) = -0 without calling here since our
 *	   odd polynomial is not evaluated in a way that preserves -0.
 *	   Callers may do the optimization tan(x) ~ x for tiny x.
 *	3. tan(x) is approximated by a odd polynomial of degree 27 on
 *	   [0,0.67434]
 *		  	         3             27
 *	   	tan(x) ~ x + T1*x + ... + T13*x
 *	   where
 *
 * 	        |tan(x)         2     4            26   |     -59.2
 * 	        |----- - (1+T1*x +T2*x +.... +T13*x    )| <= 2
 * 	        |  x 					|
 *
 *	   Note: tan(x+y) = tan(x) + tan'(x)*y
 *		          ~ tan(x) + (1+x*x)*y
 *	   Therefore, for better accuracy in computing tan(x+y), let
 *		     3      2      2       2       2
 *		r = x *(T2+x *(T3+x *(...+x *(T12+x *T13))))
 *	   then
 *		 		    3    2
 *		tan(x+y) = x + (T1*x + (x *(r+y)+y))
 *
 *      4. For x in [0.67434,pi/4],  let y = pi/4 - x, then
 *		tan(x) = tan(pi/4-y) = (1-tan(y))/(1+tan(y))
 *		       = 1 - 2*(tan(y) - (tan(y)^2)/(1+tan(y)))
 */

#include <MATH_math.h>
#include "math_private.h"
/*定义系数：T1到T12是泰勒展开式的系数，用于近似正切函数。
T1到T12这些系数是提前计算并储存的，这些系数对应于泰勒级数的优化版本。
它们是精确到足够小的误差范围内预先计算好的，这样做既保证了计算的速度，也保证了精度。*/
static const double xxx[] = {
		 3.33333333333334091986e-01,	/* 3FD55555, 55555563 */
		 1.33333333333201242699e-01,	/* 3FC11111, 1110FE7A */
		 5.39682539762260521377e-02,	/* 3FABA1BA, 1BB341FE */
		 2.18694882948595424599e-02,	/* 3F9664F4, 8406D637 */
		 8.86323982359930005737e-03,	/* 3F8226E3, E96E8493 */
		 3.59207910759131235356e-03,	/* 3F6D6D22, C9560328 */
		 1.45620945432529025516e-03,	/* 3F57DBC8, FEE08315 */
		 5.88041240820264096874e-04,	/* 3F4344D8, F2F26501 */
		 2.46463134818469906812e-04,	/* 3F3026F7, 1A8D1068 */
		 7.81794442939557092300e-05,	/* 3F147E88, A03792A6 */
		 7.14072491382608190305e-05,	/* 3F12B80F, 32F0A7E9 */
		-1.85586374855275456654e-05,	/* BEF375CB, DB605373 */
		 2.59073051863633712884e-05,	/* 3EFB2A70, 74BF7AD4 */
/* one */	 1.00000000000000000000e+00,	/* 3FF00000, 00000000 */
/* pio4 */	 7.85398163397448278999e-01,	/* 3FE921FB, 54442D18 */
/* pio4lo */	 3.06161699786838301793e-17	/* 3C81A626, 33145C07 */
};
#define	pio4	xxx[14]
#define	pio4lo	xxx[15]
#define	T	xxx
/*这个算法实现了正切函数在原点附近的27阶泰勒展开式的具体计算过程，特别是在[-π/4, π/4] 范围内的值，这个范围大约是 (-0.7854) 到 (0.7854) 之间。
通过优化计算方法，__kernel_tan 函数能够在计算机上高效且精确地计算出tan(x)的值，尤其是对于原点附近的x值。
这种实现利用了计算机算术的特性，如浮点数的表示和计算，以及通过算法优化来提高执行效率和减少误差。*/
double
__kernel_tan(double x, double y, int iy) {
	double z, r, v, w, s;
	int32_t ix, hx;
	/*参数检查和初步处理：通过GET_HIGH_WORD宏，获取x的高位字，用于后续的判断。
	ix为高位的绝对值*/
	GET_HIGH_WORD(hx,x);
	ix = hx & 0x7fffffff;
	/*|x|在[0.6744, π/4]范围内时，计算一个中间变量z = π/4 - x，则tan(x) = tan(π/4 - z) = (1 - tan(z)) / (1 + tan(z)) = 1 - 2(tan(z) - (tan(z))2 / (1 + tan(z)))。
	如果hx<0(x<0)，根据tan(-x)=-tan(x)，将其输入值转变为相反数
	计算替换变量x = π/4 - x*/
	if (ix >= 0x3FE59428) {	/* |x| >= 0.6744 */
		if (hx < 0) {
			x = -x;
			y = -y;
		}
		z = pio4 - x;
		w = pio4lo - y;
		x = z + w;
		y = 0.0;
	}
	/*z：存储x^2的值。
	  w：存储z^2，即x^4，用于计算更高阶的项。*/
	z = x * x;
	w = z * z;
	/*利用泰勒展开式计算
	w = x + T0 * x^3 + T1 * x^5 + ... + T12 * x^27 + (1 + x^2) * y
	其中 x + T0 * x^3 + T1 * x^5 + ... + T12 * x^27是泰勒展开式部分，(1 + x^2) * y是尾数计算的高精度误差项*/
	r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] +
		w * T[11]))));
	v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] +
		w * T[12])))));
	s = z * x;
	r = y + z * (s * (r + v) + y);
	r += T[0] * s;
	w = x + r;
	if (ix >= 0x3FE59428) {
		v = (double) iy;
		return (double) (1 - ((hx >> 30) & 2)) *
			(v - 2.0 * (x - (w * w / (w + v) - r)));
	}
	/*如果iy==1，返回w作为正切的近似值，即直接利用泰勒展开近似*/
	if (iy == 1)
		return w;
	else {
		/*如果iy!=1，返回-1.0 / w作为正切的近似值
		这是利用了tan(x + π/4) = - 1/tan(x)的性质*/
		double a, t;
		z = w;
		SET_LOW_WORD(z,0);
		v = r - (z - x);	/* z+v = r+x */
		t = a = -1.0 / w;	/* a = -1.0/w */
		SET_LOW_WORD(t,0);
		s = 1.0 + t * z;
		return t + a * (s + t * v);
	}
}
