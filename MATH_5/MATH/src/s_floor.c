/*
 * floor(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to floor(x).
 */
#include "math_private.h"
#include "cdefs-compat.h"
#include <float.h>
#include "MATH_math.h"
static const double huge = 1.0e300;
/*
	s_floor.c的主要作用是将输入的双精度浮点数x向下取整，即返回小于或等于x的最大整数。这个过程不涉及浮点数的四则运算，而是通过位操作（即直接操作数的二进制表示）来实现。floor函数的定义域和值域都是所有双精度浮点数。下面详细解释这个函数的原理和步骤：
	提取数据：函数首先从double类型的x中提取出两个32位的整数，这两个整数分别代表了x的符号位、指数位和部分尾数位。
	指数位处理：通过计算出x的指数部分，并减去偏移量0x3ff（即1023）得到实际的指数值。
	判断指数范围：根据指数的大小，函数将分为三种主要情况来处理：指数小于20、指数大于51以及介于20到51之间。
	1.如果指数小于20（即x的指数小于1023+20=1043），则x的整数部分小于2^20，此时需要进一步处理尾数部分。
	·如果指数小于0，说明x是一个非常小的数，可能接近0。此时，如果x不等于0，则设置不精确标志，并根据x的符号返回0或-1。
	·如果指数大于等于0，则通过移位操作屏蔽掉尾数中不需要的位，然后检查剩余的尾数位是否全为0。如果是，则x已经是整数；如果不是，则需要通过位操作将x向下取整到最接近的整数，并设置不精确标志。
	2. 指数大于51的情况
	·如果指数大于51（即x的指数大于1023+51=1074），则x是一个非常大的数，此时如果指数等于1024，则x可能是无穷大或NaN，直接返回NaN；否则，x已经是整数，直接返回x。
	2.指数在20到51之间的情况
	·如果指数在20到51之间，则通过移位操作屏蔽掉尾数中不需要的位，然后检查剩余的尾数位是否全为0。如果是，则x已经是整数；如果不是，则需要则通过位操作将x向下取整到最接近的整数，并设置不精确标志。
	合并并返回结果：最后，函数将处理后的位表示重新组合成double类型的数值，并返回这个向下取整后的结果。
	对于无穷大和NaN等特殊浮点值的处理，floor函数会特殊处理：对于无穷大，函数将返回x本身，即正无穷大或负无穷大。对于NaN，函数应返回NaN。如果输入x是正零或负零，函数将返回x本身，即正零或负零。
*/
OLM_DLLEXPORT double
floor_my(double x)
{
	int32_t i0,i1,j0;
	u_int32_t i,j;
	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;	//提取x的指数j0
	if(j0<20) {
		/*
			当x的指数小于0时，浮点数的绝对值小于1（即 -1 < x < 1）。在这种情况下，向下取整应返回最接近的整数值
		*/
	    if(j0<0) { 	/* raise inexact if x != 0 */
			if(huge+x>0.0) 
			{/* return 0*sign(x) if |x|<1 */
				/*
					如果输入值x是正数，返回0
				*/
				if(i0>=0) 
				{
					i0=i1=0;
				}
				/*
					如果输入值x是负数，返回-1
				*/
				else if(((i0&0x7fffffff)|i1)!=0)
				{ 
					i0=0xbff00000;
					i1=0;
				}
				else{
					printf("yes");
				}
			}
		/*
			当x的指数在0～20之间时
		*/
	    } else {
			i = (0x000fffff)>>j0;
			/*
				如果是整数，直接返回x
			*/
			if(((i0&i)|i1)==0) return x; /* x is integral */
			
			if(huge+x>0.0) {	/* raise inexact flag */
				/*
					如果x是负数且不是整数，则将高32位加1并去掉小数部分
				*/
				if(i0<0) i0 += (0x00100000)>>j0;
				/*
					如果是正数直接把小数置0
				*/
				i0 &= (~i); i1=0;
			}
	    }
	} else if (j0>51) {
	    if(j0==0x400) /* inf or NaN */
		{
			/*
				输入Inf，返回Inf
			*/
			if (i0 == 0x7ff00000 && i1 == 0 || i0 == 0xfff00000 && i1 == 0) /*Inf*/
			{
				return x;
			} else 	//NaN
			{	
				/*
					输入NaN，返回NaN
				*/
				double result = x + x;
				int32_t high_word = 0x7FF40000;
				int32_t low_word = 0x00000000;
				SET_HIGH_WORD(result, high_word);
				SET_LOW_WORD(result, low_word);
				return result;
			}
		}	
	    else return x;		/* x is integral */
	
	} else {
		/*
		指数j0, 20 <= j0 <=51
		*/
	    i = ((u_int32_t)(0xffffffff))>>(j0-20);
		/*
			如果是整数直接返回
		*/
	    if((i1&i)==0) return x;	/* x is integral */
		/*
			如果 x 是负数且不是整数，则将低32位加适当的值并去掉小数部分
		*/
	    if(huge+x>0.0) { 		/* raise inexact flag */
			if(i0<0) {
				if(j0==20) i0+=1;
				else {
					j = i1+(1<<(52-j0));
					if(j<i1) 
						i0 +=1 ; 	/* got a carry */
					else{
						printf("ok");
					}
					i1=j;
				}
			}
			/*
				如果是正数直接把小数置0
			*/
			i1 &= (~i);
	    }
	}
	INSERT_WORDS(x,i0,i1);
	return x;
}

