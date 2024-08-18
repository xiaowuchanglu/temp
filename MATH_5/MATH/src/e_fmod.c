

#include "cdefs-compat.h"
/* 
 * __ieee754_fmod(x,y)
 * Return x mod y in exact arithmetic
 * Method: shift and subtract
 */

#include <MATH_math.h>

#include "math_private.h"

static const double one = 1.0, Zero[] = {0.0, -0.0,};
/*
	e_fmod.c的主要作用是用来计算两个双精度浮点数x和y的模（余数）的，即计算x除以y的余数。fmod函数的定义域和值域都是所有双精度浮点数。函数的实现方法主要基于位移和减法操作。下面详细解释这个函数的原理和步骤：
	1.提取符号位和数值位：首先，函数提取出x和y的高32位和低32位，以及x的符号位，然后将x和y的绝对值分别存储起来。
	2.处理异常值：如果 y 为0，或者 x、y 中任意一个不是有限数（比如无穷大或NaN），函数将返回一个特定的结果NaN，这样的操作可以确保当 x 或 y 非正常值时，函数的返回值遵循IEEE754标准。
	3.比较 x 和 y 的大小：如果 |x| < |y|，直接返回 x。如果 |x| == |y|，则返回0，符号根据 x 的符号确定。
	4.计算 x 和 y 的指数：通过位操作计算出 x 和 y 的指数。如果 x 或 y 是非规范化数，还需要进行额外的处理来计算出正确的指数。
	5.对齐 y 到 x：如果 x 和 y 是规范化数，则将它们的尾数部分前置一个1，否则，如果是非规范化数，则需要通过位移操作来调整它们，使得 x 和 y 对齐，将它们转换为规格化数。
	6.固定点模运算：通过一系列的位移和减法操作，不断地将 x 减去 y，直到 x 小于 y。这个过程模拟了模运算的实际过程。
	7.将结果转换回浮点数：完成模运算后，将结果从固定点格式转换回浮点数格式。这包括调整指数使其回到合适的范围，并恢复符号位。
	8.输出结果：最后，根据调整后的指数和尾数以及符号位，构造最终的浮点数结果，并返回。
*/
OLM_DLLEXPORT double
__ieee754_fmod(double x, double y)
{
	int32_t n,hx,hy,hz,ix,iy,sx,i;
	u_int32_t lx,ly,lz;

	EXTRACT_WORDS(hx,lx,x);
	EXTRACT_WORDS(hy,ly,y);
	sx = hx&0x80000000;		// 获取x的符号
	hx ^=sx;	// 储存x的绝对值
	hy &= 0x7fffffff;	//储存y的绝对值

    /* 
		当输入的y为0 或x为无穷大（正或负）或NaN，或y为NaN时，函数必须返回NaN
	*/
	if((hy|ly)==0||(hx>=0x7ff00000)||	/* y=0,or x not finite */
	  ((hy|((ly|-ly)>>31))>0x7ff00000))	/* or y is NaN */
	{
		double result = (x*y)/(x*y);
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}

	if(hx<=hy) {
		/*
			当输入的|x|小于|y|时，函数必须返回x作为余数
		*/
	    if((hx<hy)||(lx<ly)) return x;	/* |x|<|y| return x */
		/*
			当输入的|x|等于|y|时，函数必须返回+0.0或-0.0，符号根据x的符号决定
		*/
	    if(lx==ly) 
		return Zero[(u_int32_t)sx>>31];	/* |x|=|y| return x*0*/
	}

    /*
		进行非规格化数规格化 
	 */
	if(hx<0x00100000) {	/* subnormal x */
	    if(hx==0) {
			for (ix = -1043, i=lx; i>0; i<<=1) ix -=1;	// 高位等于0的情况下，规格化后对应的指数
	    } else {
			for (ix = -1022,i=(hx<<11); i>0; i<<=1) ix -=1;	//	高位不等于0的情况下，规格化后对应的指数
	    }
	} else ix = (hx>>20)-1023;	//	对规格化数直接提取指数

    /*
		进行非规格化数规格化 
	 */
	if(hy<0x00100000) {	/* subnormal y */
	    if(hy==0) {
			for (iy = -1043, i=ly; i>0; i<<=1) iy -=1;	// 高位等于0的情况下，规格化后对应的指数
	    } else {
			for (iy = -1022,i=(hy<<11); i>0; i<<=1) iy -=1;	//	高位不等于0的情况下，规格化后对应的指数
	    }
	} else iy = (hy>>20)-1023;	//对规格化数直接提取指数

    /* 
		提取规格化后的尾数部分 
	*/
	if(ix >= -1022) 
	    hx = 0x00100000|(0x000fffff&hx);
	else {		/* subnormal x, shift x to normal */
	    n = -1022-ix;	//	左移位数
	    if(n<=31) {
	        hx = (hx<<n)|(lx>>(32-n));	//	规格化后对应的尾数高位
	        lx <<= n;	//	规格化后对应的尾数低位
	    } else {
			hx = lx<<(n-32);	//	规格化后对应的尾数高位
			lx = 0;	//	规格化后对应的尾数低位
	    }
	}
	if(iy >= -1022) 
	    hy = 0x00100000|(0x000fffff&hy);
	else {		/* subnormal y, shift y to normal */
	    n = -1022-iy;	//	左移位数
	    if(n<=31) {
	        hy = (hy<<n)|(ly>>(32-n));	//	规格化后对应的尾数高位
	        ly <<= n;	//	规格化后对应的尾数低位
	    } else {
			hy = ly<<(n-32);	//	规格化后对应的尾数高位
			ly = 0;	//	规格化后对应的尾数低位
	    }
	}

    /* 
		采用循环减法求余数
	*/
	n = ix - iy;	//	x和y的指数差
	while(n--) {
	    hz=hx-hy;lz=lx-ly; 
		if(lx<ly) hz -= 1;
	    if(hz<0){hx = hx+hx+(lx>>31); lx = lx+lx;}
	    else {
	    	if((hz|lz)==0) 		/* 表示除尽了 */
		    return Zero[(u_int32_t)sx>>31];
	    	hx = hz+hz+(lz>>31); lx = lz+lz;
	    }
	}
	hz=hx-hy;lz=lx-ly; 
	if(lx<ly) hz -= 1;
	if(hz>=0) {hx=hz;lx=lz;}

    /*
		可以除尽没有余数时返回0 
	*/
	if((hx|lx)==0) 			/* return sign(x)*0 */
	    return Zero[(u_int32_t)sx>>31];
	
	while(hx<0x00100000) {		/* normalize x */
	    hx = hx+hx+(lx>>31); lx = lx+lx;
	    iy -= 1;
	}
	if(iy>= -1022) {	
		/*
			构造规格化输出
		*/
	    hx = ((hx-0x00100000)|((iy+1023)<<20));
	    INSERT_WORDS(x,hx|sx,lx);
	} else {
		/*
			此时只能用非规格化数表示最后结果
		*/
	    n = -1022 - iy;
	    if(n<=20) {
			lx = (lx>>n)|((u_int32_t)hx<<(32-n));
			hx >>= n;
	    } else if (n<=31) {
			lx = (hx<<(32-n))|(lx>>n); hx = sx;
	    } else {
			lx = hx>>(n-32); hx = sx;
	    }
	    INSERT_WORDS(x,hx|sx,lx);
	    x *= one;		/* create necessary signal */
	}
	return x;		/* exact output */
}
