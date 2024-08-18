#include "cdefs-compat.h"
/* __ieee754_sqrt(x)
 * Return correctly rounded sqrt.
 *           ------------------------------------------
 *	     |  Use the hardware sqrt if you have one |
 *           ------------------------------------------
 * Method: 
 *   Bit by bit method using integer arithmetic. (Slow, but portable) 
 *   1. Normalization
 *	Scale x to y in [1,4) with even powers of 2: 
 *	find an integer k such that  1 <= (y=x*2^(2k)) < 4, then
 *		sqrt(x) = 2^k * sqrt(y)
 *   2. Bit by bit computation
 *	Let q  = sqrt(y) truncated to i bit after binary point (q = 1),
 *	     i							 0
 *                                     i+1         2
 *	    s  = 2*q , and	y  =  2   * ( y - q  ).		(1)
 *	     i      i            i                 i
 *                                                        
 *	To compute q    from q , one checks whether 
 *		    i+1       i                       
 *
 *			      -(i+1) 2
 *			(q + 2      ) <= y.			(2)
 *     			  i
 *							      -(i+1)
 *	If (2) is false, then q   = q ; otherwise q   = q  + 2      .
 *		 	       i+1   i             i+1   i
 *
 *	With some algebric manipulation, it is not difficult to see
 *	that (2) is equivalent to 
 *                             -(i+1)
 *			s  +  2       <= y			(3)
 *			 i                i
 *
 *	The advantage of (3) is that s  and y  can be computed by 
 *				      i      i
 *	the following recurrence formula:
 *	    if (3) is false
 *
 *	    s     =  s  ,	y    = y   ;			(4)
 *	     i+1      i		 i+1    i
 *
 *	    otherwise,
 *                         -i                     -(i+1)
 *	    s	  =  s  + 2  ,  y    = y  -  s  - 2  		(5)
 *           i+1      i          i+1    i     i
 *				
 *	One may easily use induction to prove (4) and (5). 
 *	Note. Since the left hand side of (3) contain only i+2 bits,
 *	      it does not necessary to do a full (53-bit) comparison 
 *	      in (3).
 *   3. Final rounding
 *	After generating the 53 bits result, we compute one more bit.
 *	Together with the remainder, we can decide whether the
 *	result is exact, bigger than 1/2ulp, or less than 1/2ulp
 *	(it will never equal to 1/2ulp).
 *	The rounding mode can be detected by checking whether
 *	huge + tiny is equal to huge, and whether huge - tiny is
 *	equal to huge for some floating point number "huge" and "tiny".
 *		
 * Special cases:
 *	sqrt(+-0) = +-0 	... exact
 *	sqrt(inf) = inf
 *	sqrt(-ve) = NaN		... with invalid signal
 *	sqrt(NaN) = NaN		... with invalid signal for signaling NaN
 */

#include <float.h>
#include <MATH_math.h>
#include "math_private.h"

static	const double	one	= 1.0, tiny=1.0e-300;
/*e_sqrt.c是对平方根函数在不同输入范围下的计算方法的一种高效实现，适用于所有实数输入。
它的核心思想是将输入的实数进行规范化，将x映射到[-1, 4)区间内，再用迭代的方式逐比特计算平方根来得到结果。*/
OLM_DLLEXPORT double
__ieee754_sqrt(double x)
{
	double z;
	int32_t sign = (int)0x80000000;
	int32_t ix0,s0,q,m,t,i;
	u_int32_t r,t1,s1,ix1,q1;
	/*参数检查和初步处理：通过EXTRACT_WORDS宏，在ix0中存储输入x的高32位，在ix1中存储输入x的低32位。*/
	EXTRACT_WORDS(ix0,ix1,x);

    /*特殊值处理：如果x是无穷大或NaN（通过与0x7ff00000比较判断），
	函||| isnan(x/home/xiaobai/libm_m_my/MATH/src/e_pow.c| isnan(x数返回 NaN。这是因为平方根函数对于无穷大和NaN没有定义。*/
	if((ix0&0x7ff00000)==0x7ff00000 || isnan(x)) {
		double result = 0.0;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}		/* NaN */
	if(ix0<=0) {
		/*平方根函数对于+-0，直接返回+-0*/
	    if(((ix0&(~sign))|ix1)==0) return x;
		/*平方根函数对于负数的输入没有定义的结果，构造sNaN返回异常值*/
	    else if(ix0<0){
			if isnan((x-x)/(x-x)){
				double result = 0.0;
				int32_t high_word = 0x7FF40000;
				int32_t low_word = 0x00000000;
				SET_HIGH_WORD(result, high_word);
				SET_LOW_WORD(result, low_word);
				return result;
			} else {
				return (x-x)/(x-x);
			}
		}		/* sqrt(-ve) = sNaN */
	}
    /* 通过将尾数左移对浮点数进行规格化 */
	m = (ix0>>20);
	if(m==0) {
	    while(ix0==0) {
		/*将低位尾数的高21位移动到了高位尾数位的低21位，相当于把尾数整体左移了21位（高位32位中去除1位符号位和11位指数位，还有21位尾数)
		m为指数
		*/
		m -= 21;
		ix0 |= (ix1>>11); ix1 <<= 21;
	    }
		/*将高32位不断左移，直到第12位（第一位尾数位前的一位）为非0，并在指数m中记录移动了多少位。
		这步操作是恢复了浮点数中隐含的整数1*/
	    for(i=0;(ix0&0x00100000)==0;i++) ix0<<=1;
	    m -= i-1;
	    ix0 |= (ix1>>(32-i));
	    ix1 <<= i;
	}
	/*移除指数偏移量*/
	m -= 1023;
	/*规范化指数，将符号位和指数位移除*/
	ix0 = (ix0&0x000fffff)|0x00100000;
	/*如果m是奇数，将x翻倍，以此处理开方时浮点数指数位除以2时产生的误差。。*/
	if(m&1){
	    ix0 += ix0 + ((ix1&sign)>>31);
	    ix1 += ix1;
	}
	/*开方时指数除以2*/
	m >>= 1;
    /* 逐比特进行开方运算 */
	ix0 += ix0 + ((ix1&sign)>>31);
	ix1 += ix1;
	/*
	q，q1储存了最终低位和高位平方根结果
	判断si+2^(-(i+1))<=yi是否成立
	如果成立则si+1=si+2^-i，yi+1=yi-si-2^(-(i+1))
	否则si+1=si，yi+1=yi
	*/
	q = q1 = s0 = s1 = 0;
	r = 0x00200000;		/* r = moving bit from right to left */

	while(r!=0) {
	    t = s0+r; 
	    if(t<=ix0) { 
		s0   = t+r; 
		ix0 -= t; 
		q   += r; 
	    } 
	    ix0 += ix0 + ((ix1&sign)>>31);
	    ix1 += ix1;
	    r>>=1;
	}

	r = sign;
	while(r!=0) {
	    t1 = s1+r; 
	    t  = s0;
	    if((t<ix0)) { 
			s1  = t1+r;
			if(((t1&sign)==sign)&&(s1&sign)==0) s0 += 1;
				ix0 -= t;
				if (ix1 < t1) ix0 -= 1;
				ix1 -= t1;
				q1  += r;
	    } else if(((t==ix0))){
			if(t1<=ix1){
				printf("yes");
				s1  = t1+r;
				if(((t1&sign)==sign)&&(s1&sign)==0) s0 += 1;
					ix0 -= t;
					if (ix1 < t1) ix0 -= 1;
					ix1 -= t1;
					q1  += r;
			} else{
				printf("no");
			}
		}
	    ix0 += ix0 + ((ix1&sign)>>31);
	    ix1 += ix1;
		r>>=1;

	}

    /* 进行舍入处理
	如果(ix0|ix1)!=0，说明还有未舍入的位，需要进一步处理 */
	if((ix0|ix1)!=0) {
	    z = one-tiny;
		/*说明浮点数计算不精确，补偿舍入误差*/
	    if (z>=one) {
	        z = one+tiny;
			/*加法操作会溢出*/
	        if (q1==(u_int32_t)0xffffffff) { q1=0; q += 1;}
			else if (z>one) {
				if (q1==(u_int32_t)0xfffffffe) q+=1;
				q1+=2; 
			} else
				q1 += (q1&1);
	    }
	}
	/*利用计算结果构造平方根*/
	ix0 = (q>>1)+0x3fe00000;
	ix1 =  q1>>1;
	if ((q&1)==1) ix1 |= sign;
	ix0 += (m <<20);
	INSERT_WORDS(z,ix0,ix1);
	return z;
}