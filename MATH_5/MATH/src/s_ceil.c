
#include "cdefs-compat.h"

/*
 * ceil(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to ceil(x).
 */

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double huge = 1.0e300;
/*
	s_ceil.c的主要作用是返回大于或等于x的最小整数值，即对x进行向上取整。它通过直接操作浮点数的位表示来实现这一功能，而不是使用传统的算术运算。这种方法被称为“位操作”或“位扭曲”。Ceil函数的定义域和值域都是所有双精度浮点数。下面是该函数的工作原理的详细解释：
	提取数据：函数首先从double类型的x中提取出两个32位的整数，这两个整数分别代表了x的符号位、指数位和部分尾数位。
	指数位处理：通过计算出x的指数部分，并减去偏移量0x3ff（即1023）得到实际的指数值。
	判断指数范围：根据指数的大小，函数将分为四种主要情况来处理：指数小于0、指数在0到19之间、指数在20到51之间以及指数大于51。
	1.指数小于0（x的绝对值小于1）：对于这种情况，如果x不等于0，函数会根据x的符号返回0或1。具体来说，如果x是正数，返回1.0；如果x是负数，返回-0.0（如果x已经是0，则直接返回0）。
	2. 指数在0到19之间（x有小数部分）：这意味着x的整数部分可以完全由高位部分表示，而小数部分则由低位部分表示。函数会检查x是否已经是整数。如果不是，它会根据x的符号向上取整，即如果x是正数，就增加x的整数部分，然后将小数部分设置为0。
	3. 指数在20到51之间（x的整数部分较大）：在这种情况下，整数部分和小数部分可能同时由高位和低位部分表示。函数同样会检查x是否已经是整数。如果不是，它会适当增加x的整数部分，并将小数部分设置为0。
	4. 指数大于51：（x的数值非常大）：在这种情况下，由于x的数值太大，小数部分无法表示，因此x已经是整数，函数直接返回x。
	处理特殊情况：对于无穷大和非数字（NaN），函数直接返回输入值。
	设置和返回结果：在处理完上述情况后，函数会将调整后的高位和低位部分重新组合成一个浮点数，并返回这个结果。如果在处理过程中x的值被修改了（即x不等于其向上取整的结果），则会引发不精确的异常，表明结果是一个近似值。
*/
OLM_DLLEXPORT double
ceil_my(double x)
{
	int32_t i0,i1,j0;
	u_int32_t i,j;
	EXTRACT_WORDS(i0,i1,x);
	/*
		提取x的指数
	*/
	j0 = ((i0>>20)&0x7ff)-0x3ff;
	if(j0<20) {
		/*
			输入浮点数的指数小于0表示x的绝对值小于1。在这种情况下，x的向上取整结果只能是最接近的整数1或0
		*/
	    if(j0<0) { 	/* raise inexact if x != 0 */
			if(huge+x>0.0) {/* return 0*sign(x) if |x|<1 */
				/*
					x < 0的情况下，直接返回0
				*/
				if(i0<0) {i0=0x80000000;i1=0;}
				/*
					x >= 0的情况下，直接返回1
				*/
				else if((i0|i1)!=0) { i0=0x3ff00000;i1=0;}
			}
	    } else {
			/*
				指数在0～20之间
			*/
			i = (0x000fffff)>>j0;
			/*
				如果是整数，直接返回
			*/
			if(((i0&i)|i1)==0) return x; /* x is integral */
			if(huge+x>0.0) {	/* raise inexact flag */
				/*
					如果是正数，整体加1，且尾数置0
				*/
				if(i0>0) i0 += (0x00100000)>>j0;
				/*
					如果是非正数，直接将小数置0
				*/
				i0 &= (~i); i1=0;
			}
	    }
	} else if (j0>51) {
	    if(j0==0x400) /* inf or NaN */
		{
			if (i0 == 0x7ff00000 && i1 == 0 || i0 == 0xfff00000 && i1 == 0) /*Inf*/
			{
				/*
					输入Inf时，直接返回inf
				*/
				return x;
			} else 	//NaN
			{	
				/*
					输入NaN时，直接返回NaN
				*/
				double result = x + x;
				int32_t high_word = 0x7FF40000;
				int32_t low_word = 0x00000000;
				SET_HIGH_WORD(result, high_word);
				SET_LOW_WORD(result, low_word);
				return result;
			}
			/*
				对于指数大于51的数直接返回
			*/
		}	
	    else return x;		/* x is integral */
	} else {
		/*
			指数在20到51之间
		*/
	    i = ((u_int32_t)(0xffffffff))>>(j0-20);
		/*
			如果是整数，直接返回
		*/
	    if((i1&i)==0) return x;	/* x is integral */
	    if(huge+x>0.0) { 		/* raise inexact flag */
			/*
				如果是正数，整体加1，且小数置0
			*/
			if(i0>0) {
				if(j0==20) i0+=1;
				else {
					j = i1 + (1<<(52-j0));
					if(j<i1) i0+=1;	/* got a carry */
					i1 = j;
				}
			}
			/*
				如果是非正数，直接将小数置0
			*/
			i1 &= (~i);
	    }
	}
	INSERT_WORDS(x,i0,i1);
	return x;
}

