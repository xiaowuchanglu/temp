#ifndef _MATH_PRIVATE_H_
#define	_MATH_PRIVATE_H_

#include "MATH_defs.h"
#include "cdefs-compat.h"
#include "types-compat.h"
#include "fpmath.h"
#include <stdint.h>
#include "math_private_openbsd.h"

/*
 * The original fdlibm code used statements like:
 *	n0 = ((*(int*)&one)>>29)^1;		* index of high word *
 *	ix0 = *(n0+(int*)&x);			* high word of x *
 *	ix1 = *((1-n0)+(int*)&x);		* low word of x *
 * to dig two 32 bit words out of the 64 bit IEEE floating point
 * value.  That is non-ANSI, and, moreover, the gcc instruction
 * scheduler gets it wrong.  We instead use the following macros.
 * Unlike the original code, we determine the endianness at compile
 * time, not at run time; I don't see much benefit to selecting
 * endianness at run time.
 */

/*
 * A union which permits us to convert between a double and two 32 bit
 * ints.
 */

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__

typedef union
{
  double value;
  struct
  {
    u_int32_t msw;
    u_int32_t lsw;
  } parts;
  struct
  {
    u_int64_t w;
  } xparts;
} ieee_double_shape_type;

#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__

typedef union
{
  double value;
  struct
  {
    u_int32_t lsw;
    u_int32_t msw;
  } parts;
  struct
  {
    u_int64_t w;
  } xparts;
} ieee_double_shape_type;

#endif

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Get a 64-bit int from a double. */
#define EXTRACT_WORD64(ix,d)					\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix) = ew_u.xparts.w;						\
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Set a double from a 64-bit int. */
#define INSERT_WORD64(d,ix)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.xparts.w = (ix);						\
  (d) = iw_u.value;						\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

/*
 * A union which permits us to convert between a float and a 32 bit
 * int.
 */

typedef union
{
  float value;
  /* FIXME: Assumes 32 bit int.  */
  unsigned int word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

/* Get expsign as a 16 bit int from a long double.  */

#define	GET_LDBL_EXPSIGN(i,d)					\
do {								\
  union IEEEl2bits ge_u;					\
  ge_u.e = (d);							\
  (i) = ge_u.xbits.expsign;					\
} while (0)

/* Set expsign of a long double from a 16 bit int.  */

#define	SET_LDBL_EXPSIGN(d,v)					\
do {								\
  union IEEEl2bits se_u;					\
  se_u.e = (d);							\
  se_u.xbits.expsign = (v);					\
  (d) = se_u.e;							\
} while (0)


#ifndef __FreeBSD__
#define	STRICT_ASSIGN(type, lval, rval)	((lval) = (rval))
#else
#ifdef FLT_EVAL_METHOD
// Attempt to get strict C99 semantics for assignment with non-C99 compilers.
#if FLT_EVAL_METHOD == 0 || __GNUC__ == 0
#define	STRICT_ASSIGN(type, lval, rval)	((lval) = (rval))
#else
#define	STRICT_ASSIGN(type, lval, rval) do {	\
	volatile type __lval;			\
						\
	if (sizeof(type) >= sizeof(long double))	\
		(lval) = (rval);		\
	else {					\
		__lval = (rval);		\
		(lval) = __lval;		\
	}					\
} while (0)
#endif
#endif
#endif

/*
 * Common routine to process the arguments to nan(), nanf(), and nanl().
 */
void __scan_nan(u_int32_t *__words, int __num_words, const char *__s);

/*
 * Mix 1 or 2 NaNs.  First add 0 to each arg.  This normally just turns
 * signaling NaNs into quiet NaNs by setting a quiet bit.  We do this
 * because we want to never return a signaling NaN, and also because we
 * don't want the quiet bit to affect the result.  Then mix the converted
 * args using addition.  The result is typically the arg whose mantissa
 * bits (considered as in integer) are largest.
 *
 * Technical complications: the result in bits might depend on the precision
 * and/or on compiler optimizations, especially when different register sets
 * are used for different precisions.  Try to make the result not depend on
 * at least the precision by always doing the main mixing step in long double
 * precision.  Try to reduce dependencies on optimizations by adding the
 * the 0's in different precisions (unless everything is in long double
 * precision).
 */
#define	nan_mix(x, y)	(((x) + 0.0L) + ((y) + 0))

#ifdef __GNUCLIKE_ASM

/* Asm versions of some functions. */

#ifdef __amd64__
static __inline int
irint(double x)
{
	int n;

	__asm__("cvtsd2si %1,%0" : "=r" (n) : "x" (x));
	return (n);
}
#define	HAVE_EFFICIENT_IRINT
#endif

#ifdef __i386__
static __inline int
irint(double x)
{
	int n;

	__asm__("fistl %0" : "=m" (n) : "t" (x));
	return (n);
}
#define	HAVE_EFFICIENT_IRINT
#endif

#endif /* __GNUCLIKE_ASM */

/*
 * ieee style elementary functions
 *
 * We rename functions here to improve other sources' diffability
 * against fdlibm.
 */
#define	__ieee754_sqrt	sqrt_my
#define	__ieee754_asin	asin
#define	__ieee754_acos	acos_my
#define	__ieee754_remainder remainder
#define	__ieee754_fmod	fmod
#define	__ieee754_atan2	atan2
#define	__ieee754_sinh	sinh
#define	__ieee754_exp	exp
double	__ldexp_exp(double,int);
#define	__ieee754_cosh	cosh
#define	__ieee754_log	log
#define	__ieee754_log10	log10
#define	__ieee754_pow	pow


/* fdlibm kernel function */
int	__kernel_rem_pio2(double*,double*,int,int,int);
/* double precision kernel functions */
#ifdef INLINE_REM_PIO2
__inline
#endif
int	__ieee754_rem_pio2(double,double*);
double	__kernel_sin(double,double,int);
double	__kernel_cos(double,double);
double	__kernel_tan(double,double,int);

#endif /* !_MATH_PRIVATE_H_ */
