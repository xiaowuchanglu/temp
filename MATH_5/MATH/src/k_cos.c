 
/*
 * __kernel_cos( x,  y )
 * kernel cos function on [-pi/4, pi/4], pi/4 ~ 0.785398164
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x. 
 *
 * Algorithm
 *	1. Since cos(-x) = cos(x), we need only to consider positive x.
 *	2. if x < 2^-27 (hx<0x3e400000 0), return 1 with inexact if x!=0.
 *	3. cos(x) is approximated by a polynomial of degree 14 on
 *	   [0,pi/4]
 *		  	                 4            14
 *	   	cos(x) ~ 1 - x*x/2 + C1*x + ... + C6*x
 *	   where the remez error is
 *	
 * 	|              2     4     6     8     10    12     14 |     -58
 * 	|cos(x)-(1-.5*x +C1*x +C2*x +C3*x +C4*x +C5*x  +C6*x  )| <= 2
 * 	|    					               | 
 * 
 * 	               4     6     8     10    12     14 
 *	4. let r = C1*x +C2*x +C3*x +C4*x +C5*x  +C6*x  , then
 *	       cos(x) ~ 1 - x*x/2 + r
 *	   since cos(x+y) ~ cos(x) - sin(x)*y 
 *			  ~ cos(x) - x*y,
 *	   a correction term is necessary in cos(x) and hence
 *		cos(x+y) = 1 - (x*x/2 - (r - x*y))
 *	   For better accuracy, rearrange to
 *		cos(x+y) ~ w + (tmp + (r-x*y))
 *	   where w = 1 - x*x/2 and tmp is a tiny correction term
 *	   (1 - x*x/2 == w + tmp exactly in infinite precision).
 *	   The exactness of w + tmp in infinite precision depends on w
 *	   and tmp having the same precision as x.  If they have extra
 *	   precision due to compiler bugs, then the extra precision is
 *	   only good provided it is retained in all terms of the final
 *	   expression for cos().  Retention happens in all cases tested
 *	   under FreeBSD, so don't pessimize things by forcibly clipping
 *	   any extra precision in w.
 */

#include "cdefs-compat.h"
#include "math_private.h"
#include "MATH_math.h"
/*定义系数：C1到C6是泰勒展开式的系数，用于近似余弦函数。
C1到C6这些系数是提前计算并储存的，这些系数对应于泰勒级数的优化版本。
它们是精确到足够小的误差范围内预先计算好的，这样做既保证了计算的速度，也保证了精度。*/
static const double
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
C1  =  4.16666666666666019037e-02, /* 0x3FA55555, 0x5555554C */
C2  = -1.38888888888741095749e-03, /* 0xBF56C16C, 0x16C15177 */
C3  =  2.48015872894767294178e-05, /* 0x3EFA01A0, 0x19CB1590 */
C4  = -2.75573143513906633035e-07, /* 0xBE927E4F, 0x809C52AD */
C5  =  2.08757232129817482790e-09, /* 0x3E21EE9E, 0xBDB4B1C4 */
C6  = -1.13596475577881948265e-11; /* 0xBDA8FAE9, 0xBE8838D4 */

/*这个算法实现了正弦函数在原点附近的13阶泰勒展开式的具体计算过程，
特别是在[-π/4, π/4] 范围内的值，这个范围大约是 (-0.7854) 到 (0.7854) 之间，这里使用了13阶泰勒展开式，
但是实际实现中通过变换和优化，形成了特定的计算公式来进一步提高精度（不是直接使用阶乘形式的倒数）。*/
OLM_DLLEXPORT double
__kernel_cos(double x, double y)
{
	double hz;
	double z;
	double r;
	double w;

	z  = x*x;
	w  = z*z;
	/*z：存储x^2的值。
	  w：存储z^2，即x^4，用于计算更高阶的项。*/
	r  = z*(C1+z*(C2+z*C3)) + w*w*(C4+z*(C5+z*C6));
	hz = 0.5*z;
	w  = one-hz;
	/*z*r为泰勒展开式近似结果
	x*y为近似的误差项*/
	return w + (((one-w)-hz) + (z*r-x*y));
}
