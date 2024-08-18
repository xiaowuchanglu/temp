 /* __kernel_sin( x, y, iy)
 * kernel sin function on ~[-pi/4, pi/4] (except on -0), pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input iy indicates whether y is 0. (if iy=0, y assume to be 0). 
 *
 * Algorithm
 *	1. Since sin(-x) = -sin(x), we need only to consider positive x. 
 *	2. Callers must return sin(-0) = -0 without calling here since our
 *	   odd polynomial is not evaluated in a way that preserves -0.
 *	   Callers may do the optimization sin(x) ~ x for tiny x.
 *	3. sin(x) is approximated by a polynomial of degree 13 on
 *	   [0,pi/4]
 *		  	         3            13
 *	   	sin(x) ~ x + S1*x + ... + S6*x
 *	   where
 *	
 * 	|sin(x)         2     4     6     8     10     12  |     -58
 * 	|----- - (1+S1*x +S2*x +S3*x +S4*x +S5*x  +S6*x   )| <= 2
 * 	|  x 					           | 
 * 
 *	4. sin(x+y) = sin(x) + sin'(x')*y
 *		    ~ sin(x) + (1-x*x/2)*y
 *	   For better accuracy, let 
 *		     3      2      2      2      2
 *		r = x *(S2+x *(S3+x *(S4+x *(S5+x *S6))))
 *	   then                   3    2
 *		sin(x) = x + (S1*x + (x *(r-y/2)+y))
 */


#include "math_private.h"
#include "MATH_math.h"
#include "cdefs-compat.h"
/*定义系数：S1到S6是泰勒展开式的系数，用于近似正弦函数。
S1到S6这些系数是提前计算并储存的，这些系数对应于泰勒级数的优化版本。
它们是精确到足够小的误差范围内预先计算好的，这样做既保证了计算的速度，也保证了精度。*/
static const double
half =  5.00000000000000000000e-01, /* 0x3FE00000, 0x00000000 */
S1  = -1.66666666666666324348e-01, /* 0xBFC55555, 0x55555549 */
S2  =  8.33333333332248946124e-03, /* 0x3F811111, 0x1110F8A6 */
S3  = -1.98412698298579493134e-04, /* 0xBF2A01A0, 0x19C161D5 */
S4  =  2.75573137070700676789e-06, /* 0x3EC71DE3, 0x57B1FE7D */
S5  = -2.50507602534068634195e-08, /* 0xBE5AE5E6, 0x8A2B9CEB */
S6  =  1.58969099521155010221e-10; /* 0x3DE5D93A, 0x5ACFD57C */
/*这个算法实现了正弦函数在原点附近的13阶泰勒展开式的具体计算过程，
特别是在[-π/4, π/4] 范围内的值，这个范围大约是 (-0.7854) 到 (0.7854) 之间，这里使用了13阶泰勒展开式，
但是实际实现中通过变换和优化，形成了特定的计算公式来进一步提高精度（不是直接使用阶乘形式的倒数）。*/
OLM_DLLEXPORT double
__kernel_sin(double x, double y, int iy)
{
	/*定义中间变量，来减少计算量，减少了乘法的次数，这对于减少计算机执行指令和提高效率是非常重要的。*/
	double z;
	double r;
	double v;
	double w;
	/*z：存储x^2的值。
	  r：根据泰勒展开式中的高阶项计算的结果。
	  v：用于存储中间计算结果z*x，即x^3。
	  w：存储z^2，即x^4，用于计算更高阶的项。*/
	z	=  x*x;
	w	=  z*z;
	r	=  S2+z*(S3+z*S4) + z*w*(S5+z*S6);
	v	=  z*x;
	/*当不用考虑x的高精度尾数带来的误差时（即参数y有效位iy为0），
	函数__kernel_sin应采用简化后的推导公式：sinx=x+(x^5)*r+S1*(x^3)实现计算输入参数x的近似正弦值功能。*/
	if(iy==0) return x+v*(S1+z*r);
	/*函数采用的优化后的推导公式为sinx=x-((x^2)(y/2-(x^3)*r)-y-S1*(x^3))实现计算输入参数x的近似正弦值功能，
	当需要考虑双精度浮点数x的高精度末尾数y以提供更精确的计算值时（即参数y有效位iy不为0），函数输出为sinx=x-((x^2)(y/2-(x^3)*r)-y-S1*(x^3))。
	此多项式是基于正弦函数的泰勒级数展开，适用于[-π/4, π/4]范围内的弧度值，
	通过优化推导公式提供高精度的正弦值近似。此公式考虑了x的尾数y，提供了更精确的正弦值近似。*/
	else      return x-((z*(half*y-v*r)-y)-v*S1);
}
