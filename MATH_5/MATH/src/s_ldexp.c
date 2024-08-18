#include "cdefs-compat.h"

#include <float.h>
#include <MATH_math.h>

#include "math_private.h"

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
huge   = 1.0e+300,
tiny   = 1.0e-300;
/*
        s_ldexp.c的主要作用是函数的作用是将一个双精度浮点数x乘以2的n次方（即x * 2^n）。这个操作是通过直接操作浮点数的指数部分来实现的，而不是通过实际的乘法或指数运算，这样可以提高计算效率。Ldexp函数的定义域和值域都是所有双精度浮点数。下面是对这个函数实现的详细解释：
        函数接受两个参数：一个双精度浮点数x和一个整数n，目的是计算x * 2^n。
        函数内部首先提取x的高32位和低32位，然后通过位操作提取出x的指数部分k。
        接下来，根据x的值和k的值，分几种情况处理：
        1.如果k为0，即x是0或非规格化数：
        如果x本身就是0（无论正负），直接返回x。
        否则，将x乘以2的54次方，使其变为规格化数，然后重新计算k。
        2.如果x是NaN（非数）或无穷大，直接返回x，即返回NaN或无穷大。
        3.根据n的值调整k，即k = k + n，然后根据调整后的k的值，分几种情况处理：
        如果k大于2046，说明结果会溢出，返回1.0e+300乘以x的符号，表示溢出。
        如果k大于0，说明结果是一个规格化数，直接设置x的指数部分为k，返回x。
        如果k小于等于-54，说明结果可能下溢：
        ·如果n大于50000，为了避免整数溢出，直接返回表示溢出的符号。
        ·否则，返回表示下溢的符号。
        如果结果是非规格化数（k加上54后仍小于等于0），将k加上54，设置x的指数部分，然后将x乘以2的-54次方返回。
*/
OLM_DLLEXPORT double
ldexp (double x, int n)
{
        int32_t k,hx,lx;
        EXTRACT_WORDS(hx,lx,x);
        k = (hx&0x7ff00000)>>20;		/* extract exponent */
        if (k==0) {
                /*
                        ldexp函数对于输入+-0，因为0的任何次方都为0,所以应直接返回x
                */				/* 0 or subnormal x */
                if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
                /*
                        对于尾数部分不符合规格化浮点数的情况，先进行规格化
                */
                x *= two54;
                GET_HIGH_WORD(hx,x);
                k = ((hx&0x7ff00000)>>20) - 54;
                /*
                        若指定相乘的指数值小于-5000,会造成下溢出。所以返回 1.0e-300 * x 可以返回0或-0并产生下溢标志
                */
                if (n< -50000) return tiny*x; 	/*underflow*/
        }
        /*
                对于尾数部分不符合规格化浮点数的情况，先进行规格话，然后提取尾数和指数。若规格化后提取出指数为0x7ff，说明输入为Inf或NaN，此时直接返回x + x，即Inf或NaN
        */
        if (k==0x7ff) return x+x;		/* NaN or Inf */
        k = k+n;
        /*
                对于尾数部分不符合规格化浮点数的情况，先进行规格化，然后提取尾数和指数。若规格化后提取出指数与制定指数相加后大于0x7fe，则在和给定指数n相乘后会上溢，返回Inf并设置上溢标志
        */
        if (k >  0x7fe) return huge*copysign(huge,x); /* overflow  */
        /*
                若规格化后提取出指数与制定指数相加后大于0，返回计算得到的规格化浮点数结果。
        */
        if (k > 0) 				/* normal result */
                {SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20)); return x;}
        if (k <= -54) {
                /*
                        对于尾数部分不符合规格化浮点数的情况，先进行规格化，然后提取尾数和指数。
                        若规格化后提取出指数与制定指数相加后小于-54，说明发生了溢出，如果指定相乘的指数 n > 5000 ，说明此时发生了上溢，根据x的符号返回对应的Inf或-Inf，并设置上溢标志。
                */
                if (n > 50000) 	/* in case integer overflow in n+k */
                        return huge*copysign(huge,x);	/*overflow*/
                /*
                        对于尾数部分不符合规格化浮点数的情况，先进行规格化，然后提取尾数和指数。
                        若规格化后提取出指数与制定指数相加后小于-54，说明发生了溢出，如果指定相乘的指数 n > 5000 ，说明此时发生了上溢，根据x的符号返回对应的Inf或-Inf，并设置上溢标志。
                */
                else return tiny*copysign(tiny,x); 	/*underflow*/
        } 
        /*
                对于尾数部分不符合规格化浮点数的情况，先进行规格化，然后提取尾数和指数。若规格化后提取出指数与制定指数相加后在-54～0之间，说明发生了溢出，如果指定相乘的指数 n <= 5000 ，说明此时需要采用非规格化浮点数来返回结果
        */
        k += 54;				/* subnormal result */
        SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20));
        return x*twom54;
}