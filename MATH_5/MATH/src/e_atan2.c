#include "cdefs-compat.h"
/* __ieee754_atan2(y,x)
 * Method :
 *	1. Reduce y to positive by atan2(y,x)=-atan2(-y,x).
 *	2. Reduce x to positive by (if x and y are unexceptional): 
 *		ARG (x+iy) = arctan(y/x)   	   ... if x > 0,
 *		ARG (x+iy) = pi - arctan[y/(-x)]   ... if x < 0,
 *
 * Special cases:
 *
 *	ATAN2((anything), NaN ) is NaN;
 *	ATAN2(NAN , (anything) ) is NaN;
 *	ATAN2(+-0, +(anything but NaN)) is +-0  ;
 *	ATAN2(+-0, -(anything but NaN)) is +-pi ;
 *	ATAN2(+-(anything but 0 and NaN), 0) is +-pi/2;
 *	ATAN2(+-(anything but INF and NaN), +INF) is +-0 ;
 *	ATAN2(+-(anything but INF and NaN), -INF) is +-pi;
 *	ATAN2(+-INF,+INF ) is +-pi/4 ;
 *	ATAN2(+-INF,-INF ) is +-3pi/4;
 *	ATAN2(+-INF, (anything but,0,NaN, and INF)) is +-pi/2;
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

static volatile double
tiny  = 1.0e-300;
static const double
zero  = 0.0,
pi_o_4  = 7.8539816339744827900E-01, /* 0x3FE921FB, 0x54442D18 */
pi_o_2  = 1.5707963267948965580E+00, /* 0x3FF921FB, 0x54442D18 */
pi      = 3.1415926535897931160E+00; /* 0x400921FB, 0x54442D18 */
static volatile double
pi_lo   = 1.2246467991473531772E-16; /* 0x3CA1A626, 0x33145C07 */

OLM_DLLEXPORT double
__ieee754_atan2(double y, double x)
{
	double z;
	int32_t k,m,hx,hy,ix,iy;
	u_int32_t lx,ly;

	EXTRACT_WORDS(hx,lx,x);
	ix = hx&0x7fffffff;
	EXTRACT_WORDS(hy,ly,y);
	iy = hy&0x7fffffff;
	/*
		函数在 x 或 y 为 NaN 时，应返回 NaN
	*/
	if(((ix|((lx|-lx)>>31))>0x7ff00000)||
	   ((iy|((ly|-ly)>>31))>0x7ff00000))	/* x or y is NaN */
	{
		double result = x+y;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}
	/*
		函数在 x == 1.0时，应返回 atan(y)
	*/
	if(hx==0x3ff00000&&lx==0) return atan_my(y);   /* x=1.0 */

	m = ((hy>>31)&1)|((hx>>30)&2);	/* 2*sign(x)+sign(y) */

    /*
		处理 y == 0的特殊情况
	*/
	if((iy|ly)==0) {
	    switch(m) {
			case 0: 
			/*
				当 y = +-0 且 x > 0 时，角度在 x 轴的正方向，因此 atan2(0, x) 应返回 +-0
			*/
			case 1: return y; 	/* atan(+-0,+anything)=+-0 */
			/*
				当 y = +0 且 x < 0 时，角度在 x 轴的负方向，因此 atan2(0, x) 应返回 pi
			*/
			case 2: return  pi+tiny;/* atan(+0,-anything) = pi */
			/*
				当 y = -0 且 x < 0 时，角度在 x 轴的负方向，因此 atan2(-0, x) 应返回 -pi
			*/
			case 3: return -pi-tiny;/* atan(-0,-anything) =-pi */
	    }
	}
    /*
		当x = 0且y > 0时，角度在y轴的正方向，因此 atan2(y, 0)应返回π/2。
		当x = 0且y < 0时，角度在y轴的负方向，因此 atan2(y, 0)应返回-π/2。 
	*/
	if((ix|lx)==0) return (hy<0)?  -pi_o_2-tiny: pi_o_2+tiny;
	    
    /* when x is INF */
	if(ix==0x7ff00000) {
	    if(iy==0x7ff00000) {
			switch(m) {
				/*
					函数在 x 和 y 都为无穷大时，根据 x 和 y 的符号。atan2(+INF, +INF) = π/4
				*/
				case 0: return  pi_o_4+tiny;/* atan(+INF,+INF) */
				/*
					函数在 x 和 y 都为无穷大时，根据 x 和 y 的符号。atan2(-INF, +INF) = -π/4
				*/
				case 1: return -pi_o_4-tiny;/* atan(-INF,+INF) */
				/*
					函数在 x 和 y 都为无穷大时，根据 x 和 y 的符号。atan2(+INF, -INF) = 3π/4
				*/
				case 2: return  3.0*pi_o_4+tiny;/*atan(+INF,-INF)*/
				/*
					函数在 x 和 y 都为无穷大时，根据 x 和 y 的符号。atan2(-INF, -INF) = -3π/4
				*/
				case 3: return -3.0*pi_o_4-tiny;/*atan(-INF,-INF)*/
			}
	    } else {
			switch(m) {
				/*
					当x为正无穷大且y为正数时，atan2(y, +INF) = 0
				*/
				case 0: return  zero  ;	/* atan(+...,+INF) */
				/*
					当x为正无穷大且y为负数时，atan2(y, +INF) = 0
				*/
				case 1: return -zero  ;	/* atan(-...,+INF) */
				/*
					当x为负无穷大且y为正数时，atan2(y, -INF) = pi
				*/
				case 2: return  pi+tiny  ;	/* atan(+...,-INF) */
				/*
					当x为负无穷大且y为正数时，atan2(y, -INF) = pi
				*/
				case 3: return -pi-tiny  ;	/* atan(-...,-INF) */
			}
	    }
	}
    /* 
		当y为无穷大且x有限时，如果y为正，atan2(∞, x) 返回π/2；如果y为负，atan2(-∞, x)返回-π/2
	*/
	if(iy==0x7ff00000) return (hy<0)? -pi_o_2-tiny: pi_o_2+tiny;

    /* compute y/x */
	k = (iy-ix)>>20;
	/* 
		|y/x| >  2**60 时，近似计算无穷
	*/
	if(k > 60) {		 	
	    z=pi_o_2+0.5*pi_lo;
	    m&=1;
	}
	/* 
		0 > |y|/x > -2**-60 时，近似计算0.0
	*/
	else if(hx<0&&k<-60) z=0.0;
	/*
		正常值，计算atan(|y/x|)
	*/
	else z=atan_my(fabs(y/x));	
	switch (m) {
		/*
			对于y和x都为正数（第一象限），角度计算为 arctan(y/x)，即：
			arctan2(y, x) = arctan(y / x)
		*/
	    case 0: return       z  ;	/* atan(+,+) */
		/*
			对于y为正数，x为负数（第二象限），角度为π减去 arctan(|y/x|)，即：
			arctan2(y, x) = pi - arctan(|y / x|)
		*/
	    case 1: return      -z  ;	/* atan(-,+) */
		/*
			对于y和x都为负数（第三象限），角度为-π加上 arctan(y/x)，即：
			arctan2(y, x) = -pi + arctan(|y / x|)
		*/
	    case 2: return  pi-(z-pi_lo);/* atan(+,-) */
		/*
			对于y为负数，x为负数（第四象限），角度计算为 -arctan(|y/x|)，即：
			arctan2(y, x) = -arctan(|y / x|)
		*/
	    default: /* case 3 */
	    	    return  (z-pi_lo)-pi;/* atan(-,-) */
	}
}