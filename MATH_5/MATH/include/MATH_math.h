
#include "MATH_defs.h"

#ifndef MATH_MATH_H
#define	MATH_MATH_H

#if (defined(_WIN32) || defined (_MSC_VER)) && !defined(__WIN32__)
    #define __WIN32__
#endif

#if !defined(__arm__) && !defined(__wasm__)
#define OLM_LONG_DOUBLE
#endif

#ifndef __pure2     //纯函数的预定义宏
#define __pure2
#endif

/*
 * ANSI/POSIX
 */
extern const union __infinity_un {      //全局变量无穷大
	unsigned char	__uc[8];
	double		__ud;
} __infinity;

extern const union __nan_un {       //全局变量非数值
	unsigned char	__uc[sizeof(float)];
	float		__uf;
} __nan;

/* VBS
#if __GNUC_PREREQ__(3, 3) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 800)
#define	__MATH_BUILTIN_CONSTANTS
#endif

#if __GNUC_PREREQ__(3, 0) && !defined(__INTEL_COMPILER)
#define	__MATH_BUILTIN_RELOPS
#endif
*/

//VBS begin
#define __MATH_BUILTIN_CONSTANTS    //数学常数的编译预定义宏
#define	__MATH_BUILTIN_RELOPS       //数学运算关系的预定义宏
#ifndef __ISO_C_VISIBLE
#define __ISO_C_VISIBLE 1999
#endif
//VBS end

// 如果定义了宏，常量HUGE_VAL是函数_builtin_huge_val()计算的结果，如果没有定义，取双精度浮点数的无穷大值
#ifdef __MATH_BUILTIN_CONSTANTS
#define	HUGE_VAL	__builtin_huge_val()
#else
#define	HUGE_VAL	(__infinity.__ud)
#endif

#if __ISO_C_VISIBLE >= 1999
#define	FP_ILOGB0	(-INT_MAX)     //浮点数为0时的指数（
#define	FP_ILOGBNAN	INT_MAX

#define	MATH_ERRNO	1   //出错
#define	MATH_ERREXCEPT	2       //浮点数异常
#define	math_errhandling	MATH_ERREXCEPT      //确定函数库错误处理机制

#define	FP_FAST_FMAF	1       //支持单精度浮点数的快速FMA计算
#ifdef __ia64__
#define	FP_FAST_FMA	1       //双精度浮点数的快速FMA计算
#endif

/* Symbolic constants to classify floating point numbers. */
#define	FP_INFINITE	0x01        //浮点数是无穷大
#define	FP_NAN		0x02        //浮点数非数值
#define	FP_NORMAL	0x04        //浮点数是正常数值
#define	FP_SUBNORMAL	0x08        //浮点数接近于0但不等于0
#define	FP_ZERO		0x10        //浮点数是0

/*
    判断是否是双精度浮点数有限数
*/
#define	fpclassify(x)               \
    ((sizeof (x) == sizeof (double)) ? __fpclassifyd(x) \
    : __fpclassifyd((double)x)) 

/*
    判断是否是双精度浮点数有限数
*/
#define	isfinite(x)					\
    ((sizeof (x) == sizeof (double)) ? __isfinite(x)	\
    : __isfinite((double)x))

/*
    判断是否是双精度无穷大
*/
#define	isinf(x)					\
    ((sizeof (x) == sizeof (double)) ? isinf(x)	\
    : __isinf((double)x))
/*
    判断是否是双精度非数值
*/
#define	isnan(x) (isnan(x))

/*
    判断是否是双精度正常数
*/
#define	isnormal(x)					\
    ((sizeof (x) == sizeof (double)) ? __isnormal(x)	\
    : __isnormal((double)x))

#ifdef __MATH_BUILTIN_RELOPS
#define	isgreater(x, y)		__builtin_isgreater((x), (y))   //判断x是否大于y
#define	isgreaterequal(x, y)	__builtin_isgreaterequal((x), (y))  //判断x是否大于等于y
#define	isless(x, y)		__builtin_isless((x), (y))  //判断x是否小于y
#define	islessequal(x, y)	__builtin_islessequal((x), (y)) //判断x是否小于等于y
#define	islessgreater(x, y)	__builtin_islessgreater((x), (y))   //不等于
#define	isunordered(x, y)	__builtin_isunordered((x), (y)) //不可比较，一个是NAN
#else
#define	isgreater(x, y)		(!isunordered((x), (y)) && (x) > (y))
#define	isgreaterequal(x, y)	(!isunordered((x), (y)) && (x) >= (y))
#define	isless(x, y)		(!isunordered((x), (y)) && (x) < (y))
#define	islessequal(x, y)	(!isunordered((x), (y)) && (x) <= (y))
#define	islessgreater(x, y)	(!isunordered((x), (y)) && \
					((x) > (y) || (y) > (x)))
#define	isunordered(x, y)	(isnan(x) || isnan(y))
#endif /* __MATH_BUILTIN_RELOPS */

/*
    确定双精度浮点数的符号位
*/
#define	signbit(x)					\
    ((sizeof (x) == sizeof (double)) ? __signbit(x)	\
    : __signbit((double)x))

//VBS
//typedef	__double_t	double_t;
//typedef	__float_t	float_t;
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * XOPEN/SVID
 */
#if __BSD_VISIBLE || __XSI_VISIBLE
#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */


#ifndef MATH_ONLY_THREAD_SAFE
OLM_DLLEXPORT extern int signgam;
#endif
#endif /* __BSD_VISIBLE || __XSI_VISIBLE */

/*
 * Most of these functions depend on the rounding mode and have the side
 * effect of raising floating-point exceptions, so they are not declared
 * as __pure2.  In C99, FENV_ACCESS affects the purity of these functions.
 */

#if defined(__cplusplus)
extern "C" {
#endif
/* Symbol present when MATH is used. */
int isMATH(void);

/*
 * ANSI/POSIX
 */
OLM_DLLEXPORT int	__fpclassifyd(double) __pure2;
OLM_DLLEXPORT int	__isfinite(double) __pure2;
OLM_DLLEXPORT int	__isnormal(double) __pure2;
OLM_DLLEXPORT int	__signbit(double) __pure2;
OLM_DLLEXPORT double	sin(double);
OLM_DLLEXPORT double	cos(double);
OLM_DLLEXPORT double	tan(double);
OLM_DLLEXPORT double	asin(double);
OLM_DLLEXPORT double	acos_my(double);
OLM_DLLEXPORT double	atan_my(double);

OLM_DLLEXPORT double	atan2(double, double);
OLM_DLLEXPORT double	fabs(double) __pure2;
OLM_DLLEXPORT double	floor_my(double);
OLM_DLLEXPORT double	sqrt_my(double);
OLM_DLLEXPORT double	fmod(double, double);
OLM_DLLEXPORT double	modf(double, double *);	/* fundamentally !__pure2 */
OLM_DLLEXPORT double	sinh(double);
OLM_DLLEXPORT double	cosh(double);
OLM_DLLEXPORT double	tanh(double);
OLM_DLLEXPORT double	expm1(double);
OLM_DLLEXPORT double	exp(double);
OLM_DLLEXPORT double	frexp(double, int *);	/* fundamentally !__pure2 */
OLM_DLLEXPORT double	ldexp(double, int);
OLM_DLLEXPORT double	log(double);
OLM_DLLEXPORT double	log10(double);
OLM_DLLEXPORT double	ceil_my(double);
OLM_DLLEXPORT double	pow(double, double);
OLM_DLLEXPORT int       __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec);




/*
 * These functions are not in C90.
 */
#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE
OLM_DLLEXPORT int	(isinf)(double) __pure2;
OLM_DLLEXPORT int	(isnan)(double) __pure2;
OLM_DLLEXPORT int	ilogb(double) __pure2;
OLM_DLLEXPORT double	nan(const char *) __pure2;
OLM_DLLEXPORT double	nextafter(double, double);
OLM_DLLEXPORT double	nexttoward(double, long double);
OLM_DLLEXPORT double	remainder(double, double);
OLM_DLLEXPORT double	remquo(double, double, int *);
OLM_DLLEXPORT double	rint(double);
OLM_DLLEXPORT long	lrint(double);
OLM_DLLEXPORT long long llrint(double);
#endif /* __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE */


#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999
OLM_DLLEXPORT double	copysign(double, double) __pure2;
OLM_DLLEXPORT double	fdim(double, double);
OLM_DLLEXPORT double	fmax(double, double) __pure2;
OLM_DLLEXPORT double	fmin(double, double) __pure2;
OLM_DLLEXPORT double	nearbyint(double);
OLM_DLLEXPORT double	round(double);
OLM_DLLEXPORT double	scalbn(double, int);
OLM_DLLEXPORT double	trunc(double);
#endif

/*自定义函数*/
OLM_DLLEXPORT int isNormal(double d);
OLM_DLLEXPORT int isNormalf(float f);

#if defined(__cplusplus)
}
#endif
#endif /* !MATH_MATH_H */
