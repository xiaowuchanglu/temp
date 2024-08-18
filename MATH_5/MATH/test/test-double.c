/* Copyright (C) 1997, 1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@suse.de>, 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#define FUNC(function) function
#define FLOAT double
#define TEST_MSG "testing double (without inline functions)\n"
#define MATHCONST(x) x
#define CHOOSE(Clongdouble,Cdouble,Cfloat,Cinlinelongdouble,Cinlinedouble,Cinlinefloat) Cdouble
#define PRINTF_EXPR "e"
#define PRINTF_XEXPR "a"
#define PRINTF_NEXPR "f"
#define TEST_DOUBLE 1

#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES
#endif


/* Copyright (C) 1997, 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@arthur.rhein-neckar.de>, 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* Part of testsuite for libm.

   This file is processed by a perl script.  The resulting file has to
   be included by a master file that defines:

   Makros:
   FUNC(function): converts general function name (like cos) to
   name with correct suffix (e.g. cosl or cosf)
   MATHCONST(x):   like FUNC but for constants (e.g convert 0.0 to 0.0L)
   FLOAT:	   floating point type to test
   - TEST_MSG:	   informal message to be displayed
   CHOOSE(Clongdouble,Cdouble,Cfloat,Cinlinelongdouble,Cinlinedouble,Cinlinefloat):
   chooses one of the parameters as delta for testing
   equality
   PRINTF_EXPR	   Floating point conversion specification to print a variable
   of type FLOAT with printf.  PRINTF_EXPR just contains
   the specifier, not the percent and width arguments,
   e.g. "f".
   PRINTF_XEXPR	   Like PRINTF_EXPR, but print in hexadecimal format.
   PRINTF_NEXPR Like PRINTF_EXPR, but print nice.  */

/* This testsuite has currently tests for:
   acos, acosh, asin, asinh, atan, atan2, atanh,
   cbrt, ceil, copysign, cos, cosh, erf, erfc, exp, exp10, exp2, expm1,
   fabs, fdim, floor, fma, fmax, fmin, fmod, fpclassify,
   frexp, gamma, hypot,
   ilogb, isfinite, isinf, isnan, isnormal,
   isless, islessequal, isgreater, isgreaterequal, islessgreater, isunordered,
   j0, j1, jn,
   ldexp, lgamma, log, log10, log1p, log2, logb,
   modf, nearbyint, nextafter,
   pow, remainder, remquo, rint, lrint, llrint,
   round, lround, llround,
   scalb, scalbn, scalbln, signbit, sin, sincos, sinh, sqrt, tan, tanh, tgamma, trunc,
   y0, y1, yn

   and for the following complex math functions:
   cabs, cacos, cacosh, carg, casin, casinh, catan, catanh,
   ccos, ccosh, cexp, clog, cpow, cproj, csin, csinh, csqrt, ctan, ctanh.

   At the moment the following functions aren't tested:
   drem, significand, nan

   Parameter handling is primitive in the moment:
   --verbose=[0..3] for different levels of output:
   0: only error count
   1: basic report on failed tests (default)
   2: full report on all tests
   -v for full output (equals --verbose=3)
   -u for generation of an ULPs file
 */

/* "Philosophy":

   This suite tests some aspects of the correct implementation of
   mathematical functions in libm.  Some simple, specific parameters
   are tested for correctness but there's no exhaustive
   testing.  Handling of specific inputs (e.g. infinity, not-a-number)
   is also tested.  Correct handling of exceptions is checked
   against.  These implemented tests should check all cases that are
   specified in ISO C99.

   Exception testing: At the moment only divide-by-zero and invalid
   exceptions are tested.  Overflow/underflow and inexact exceptions
   aren't checked at the moment.

   NaN values: There exist signalling and quiet NaNs.  This implementation
   only uses signalling NaN as parameter but does not differenciate
   between the two kinds of NaNs as result.

   Inline functions: Inlining functions should give an improvement in
   speed - but not in precission.  The inlined functions return
   reasonable values for a reasonable range of input values.  The
   result is not necessarily correct for all values and exceptions are
   not correctly raised in all cases.  Problematic input and return
   values are infinity, not-a-number and minus zero.  This suite
   therefore does not check these specific inputs and the exception
   handling for inlined mathematical functions - just the "reasonable"
   values are checked.

   Beware: The tests might fail for any of the following reasons:
   - Tests are wrong
   - Functions are wrong
   - Floating Point Unit not working properly
   - Compiler has errors

   With e.g. gcc 2.7.2.2 the test for cexp fails because of a compiler error.


   To Do: All parameter should be numbers that can be represented as
   exact floating point values.  Currently some values cannot be represented
   exactly and therefore the result is not the expected result.
*/

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include "libm-test-ulps.h"
#include <float.h>
#include <MATH.h>

#if 0 /* XXX scp XXX */
#define FE_INEXACT FE_INEXACT
#define FE_DIVBYZERO FE_DIVBYZERO
#define FE_UNDERFLOW FE_UNDERFLOW
#define FE_OVERFLOW FE_OVERFLOW
#define FE_INVALID FE_INVALID
#endif

#include <limits.h>
#include "math_private.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if 0 /* XXX scp XXX */
#include <argp.h>
#endif


/* Possible exceptions */
#define NO_EXCEPTION			0x0
#define INVALID_EXCEPTION		0x1
#define DIVIDE_BY_ZERO_EXCEPTION	0x2
/* The next flags signals that those exceptions are allowed but not required.   */
#define INVALID_EXCEPTION_OK		0x4
#define DIVIDE_BY_ZERO_EXCEPTION_OK	0x8
#define EXCEPTIONS_OK INVALID_EXCEPTION_OK+DIVIDE_BY_ZERO_EXCEPTION_OK
/* Some special test flags, passed togther with exceptions.  */
#define IGNORE_ZERO_INF_SIGN		0x10

/* Various constants (we must supply them precalculated for accuracy).  */
#define M_PI_6l			.52359877559829887307710723054658383L
#define M_E2l			7.389056098930650227230427460575008L
#define M_E3l			20.085536923187667740928529654581719L
#define M_2_SQRT_PIl		3.5449077018110320545963349666822903L	/* 2 sqrt (M_PIl)  */
#define M_SQRT_PIl		1.7724538509055160272981674833411451L	/* sqrt (M_PIl)  */
#define M_LOG_SQRT_PIl		0.57236494292470008707171367567652933L	/* log(sqrt(M_PIl))  */
#define M_LOG_2_SQRT_PIl	1.265512123484645396488945797134706L	/* log(2*sqrt(M_PIl))  */
#define M_PI_34l		(M_PIl - M_PI_4l)		/* 3*pi/4 */
#define M_PI_34_LOG10El		(M_PIl - M_PI_4l) * M_LOG10El
#define M_PI2_LOG10El		M_PI_2l * M_LOG10El
#define M_PI4_LOG10El		M_PI_4l * M_LOG10El
#define M_PI_LOG10El		M_PIl * M_LOG10El

#if 1 /* XXX scp XXX */
# define M_El		2.7182818284590452353602874713526625L  /* e */
# define M_LOG2El	1.4426950408889634073599246810018922L  /* log_2 e */
# define M_LOG10El	0.4342944819032518276511289189166051L  /* log_10 e */
# define M_LN2l		0.6931471805599453094172321214581766L  /* log_e 2 */
# define M_LN10l	2.3025850929940456840179914546843642L  /* log_e 10 */
# define M_PIl		3.1415926535897932384626433832795029L  /* pi */
# define M_PI_2l	1.5707963267948966192313216916397514L  /* pi/2 */
# define M_PI_4l	0.7853981633974483096156608458198757L  /* pi/4 */
# define M_1_PIl	0.3183098861837906715377675267450287L  /* 1/pi */
# define M_2_PIl	0.6366197723675813430755350534900574L  /* 2/pi */
# define M_2_SQRTPIl	1.1283791670955125738961589031215452L  /* 2/sqrt(pi) */
# define M_SQRT2l	1.4142135623730950488016887242096981L  /* sqrt(2) */
# define M_SQRT1_2l	0.7071067811865475244008443621048490L  /* 1/sqrt(2) */
#endif

static FILE *ulps_file;	/* File to document difference.  */
static int output_ulps;	/* Should ulps printed?  */

static int noErrors;	/* number of errors */
static int noTests;	/* number of tests (without testing exceptions) */
static int noExcTests;	/* number of tests for exception flags */
static int noXFails;	/* number of expected failures.  */
static int noXPasses;	/* number of unexpected passes.  */

static int verbose;
static int output_max_error;	/* Should the maximal errors printed?  */
static int output_points;	/* Should the single function results printed?  */
static int ignore_max_ulp;	/* Should we ignore max_ulp?  */

static FLOAT minus_zero, plus_zero;
static FLOAT plus_infty, minus_infty, nan_value;

static FLOAT max_error, real_max_error, imag_max_error;


#define MANT_DIG CHOOSE ((LDBL_MANT_DIG-1), (DBL_MANT_DIG-1), (FLT_MANT_DIG-1),  \
                         (LDBL_MANT_DIG-1), (DBL_MANT_DIG-1), (FLT_MANT_DIG-1))


static void
init_max_error (void)
{
  max_error = 0;
  real_max_error = 0;
  imag_max_error = 0;
  feclearexcept (FE_ALL_EXCEPT);
}

static void
set_max_error (FLOAT current, FLOAT *curr_max_error)
{
  if (current > *curr_max_error)
    *curr_max_error = current;
}


/* Should the message print to screen?  This depends on the verbose flag,
   and the test status.  */
static int
print_screen (int ok, int xfail)
{
  if (output_points
      && (verbose > 1
	  || (verbose == 1 && ok == xfail)))
    return 1;
  return 0;
}


/* Should the message print to screen?  This depends on the verbose flag,
   and the test status.  */
static int
print_screen_max_error (int ok, int xfail)
{
  if (output_max_error
      && (verbose > 1
	  || ((verbose == 1) && (ok == xfail))))
    return 1;
  return 0;
}

/* Update statistic counters.  */
static void
update_stats (int ok, int xfail)
{
  ++noTests;
  if (ok && xfail)
    ++noXPasses;
  else if (!ok && xfail)
    ++noXFails;
  else if (!ok && !xfail)
    ++noErrors;
}

static void
print_ulps (const char *test_name, FLOAT ulp, FLOAT max_ulp)
{
  if (output_ulps)
    {
      fprintf (ulps_file, "Test \"%s\":\n", test_name);
      fprintf (ulps_file, "ulps %s: % .4" PRINTF_NEXPR "\n",
	       CHOOSE("ldouble", "double", "float",
		      "ildouble", "idouble", "ifloat"), ulp);
       }
}

static void
print_function_ulps (const char *function_name, FLOAT ulp)
{
  if (output_ulps)
    {
      fprintf (ulps_file, "Function: \"%s\":\n", function_name);
      fprintf (ulps_file, "%s: % .4" PRINTF_NEXPR "\n",
	       CHOOSE("ldouble", "double", "float",
		      "ildouble", "idouble", "ifloat"), ulp);
    }
}


static void
print_max_error (const char *func_name, FLOAT allowed, int xfail)
{
  int ok = 0;

  if (max_error == 0.0 || (max_error <= allowed && !ignore_max_ulp))
    {
      ok = 1;
    }

  if (!ok)
    print_function_ulps (func_name, max_error);


  if (print_screen_max_error (ok, xfail))
    {
      printf ("Maximal error of `%s'\n", func_name);
      printf (" is      : % .4" PRINTF_NEXPR " ulp\n", max_error);
      printf (" accepted: % .4" PRINTF_NEXPR " ulp\n", allowed);
    }

  update_stats (ok, xfail);
}

/* Test whether a given exception was raised.  */
static void
test_single_exception (const char *test_name,
		       int exception,
		       int exc_flag,
		       int fe_flag,
		       const char *flag_name)
{
/* Don't perform these checks if we're compiling with clang, because clang
   doesn't bother to set floating-point exceptions properly */
#ifndef __clang__
#ifndef TEST_INLINE
  int ok = 1;
  if (exception & exc_flag)
    {
      if (fetestexcept (fe_flag))
	{
	  if (print_screen (1, 0))
	    printf ("Pass: %s: Exception \"%s\" set\n", test_name, flag_name);
	}
      else
	{
	  ok = 0;
	  if (print_screen (0, 0))
	    printf ("Failure1: %s: Exception \"%s\" not set\n",
		    test_name, flag_name);
	}
    }
  else
    {
      if (fetestexcept (fe_flag))
	{
	  ok = 0;
	  if (print_screen (0, 0))
	    printf ("Failure2: %s: Exception \"%s\" set\n",
		    test_name, flag_name);
	}
      else
	{
	  if (print_screen (1, 0))
	    printf ("%s: Exception \"%s\" not set\n", test_name,
		    flag_name);
	}
    }
  if (!ok)
    ++noErrors;

#endif
#endif // __clang__
}


/* Test whether exceptions given by EXCEPTION are raised.  Ignore thereby
   allowed but not required exceptions.
*/
static void
test_exceptions (const char *test_name, int exception)
{
  ++noExcTests;
#ifdef FE_DIVBYZERO
  if ((exception & DIVIDE_BY_ZERO_EXCEPTION_OK) == 0)
    test_single_exception (test_name, exception,
			   DIVIDE_BY_ZERO_EXCEPTION, FE_DIVBYZERO,
			   "Divide by zero");
#endif
#ifdef FE_INVALID
  if ((exception & INVALID_EXCEPTION_OK) == 0)
  {   
    test_single_exception (test_name, exception, INVALID_EXCEPTION, FE_INVALID,
			 "Invalid operation");
  }
#endif
  feclearexcept (FE_ALL_EXCEPT);
}


static void
check_float_internal (const char *test_name, FLOAT computed, FLOAT expected,
		      FLOAT max_ulp, int xfail, int exceptions,
		      FLOAT *curr_max_error)
{
  int ok = 0;
  int print_diff = 0;
  FLOAT diff = 0;
  FLOAT ulp = 0;
  test_exceptions (test_name, exceptions);
  if (isnan (computed) && isnan (expected))
    ok = 1;
  else if (isinf (computed) && isinf (expected))
    {
      /* Test for sign of infinities.  */
      if ((exceptions & IGNORE_ZERO_INF_SIGN) == 0
	  && signbit (computed) != signbit (expected))
	{
	  ok = 0;
	  printf ("infinity has wrong sign.\n");
	}
      else
	ok = 1;
    }
  /* Don't calc ulp for NaNs or infinities.  */
  else if (isinf (computed) || isnan (computed) || isinf (expected) || isnan (expected))
    ok = 0;
  else
    {
      diff = FUNC(fabs) (computed - expected);
      /* ilogb (0) isn't allowed.  */
      if (expected == 0.0)
	ulp = diff / FUNC(ldexp) (1.0, - MANT_DIG);
      else
	ulp = diff / FUNC(ldexp) (1.0, FUNC(ilogb) (expected) - MANT_DIG);
      set_max_error (ulp, curr_max_error);
      print_diff = 1;
      if ((exceptions & IGNORE_ZERO_INF_SIGN) == 0
	  && computed == 0.0 && expected == 0.0
	  && signbit(computed) != signbit (expected))
    {
      ok = 0;
	    print_ulps (test_name, ulp, max_ulp);

    }
      else if (ulp == 0.0 || (ulp <= max_ulp && !ignore_max_ulp))
      {
	      ok = 1;
	      print_ulps (test_name, ulp, max_ulp);
      }
      else
	{
	  ok = 0;
	  print_ulps (test_name, ulp, max_ulp);
	}

    }
  if (print_screen (ok, xfail))
    {
      if (!ok)
	printf ("Failure: ");
      printf ("Test: %s\n", test_name);
      printf ("Result:\n");
      printf (" is:         % .20" PRINTF_EXPR "  % .20" PRINTF_XEXPR "\n",
	      computed, computed);
      printf (" should be:  % .20" PRINTF_EXPR "  % .20" PRINTF_XEXPR "\n",
	      expected, expected);
      if (print_diff)
	{
	  printf (" difference: % .20" PRINTF_EXPR "  % .20" PRINTF_XEXPR
		  "\n", diff, diff);
	  printf (" ulp       : % .4" PRINTF_NEXPR "\n", ulp);
	  printf (" max.ulp   : % .4" PRINTF_NEXPR "\n", max_ulp);
	}
    }
  update_stats (ok, xfail);
}


static void
check_float (const char *test_name, FLOAT computed, FLOAT expected,
	     FLOAT max_ulp, int xfail, int exceptions)
{
  check_float_internal (test_name, computed, expected, max_ulp, xfail,
			exceptions, &max_error);
}

/* Check that computed and expected values are equal (int values).  */
static void
check_int (const char *test_name, int computed, int expected, int max_ulp,
	   int xfail, int exceptions)
{
  int diff = computed - expected;
  int ok = 0;
  test_exceptions (test_name, exceptions);
  noTests++;
  if (abs (diff) <= max_ulp)
    ok = 1;

  if (!ok)
    print_ulps (test_name, diff,max_ulp);

  if (print_screen (ok, xfail))
    {
      if (!ok)
	printf ("Failure: ");
      printf ("Test: %s\n", test_name);
      printf ("Result:\n");
      printf (" is:         %d\n", computed);
      printf (" should be:  %d\n", expected);
    }

  update_stats (ok, xfail);
}


/* Check that computed value is true/false.  */
static void
check_bool (const char *test_name, int computed, int expected,
	    long int max_ulp, int xfail, int exceptions)
{
  int ok = 0;

  test_exceptions (test_name, exceptions);
  noTests++;
  if ((computed == 0) == (expected == 0))
    ok = 1;

  if (print_screen (ok, xfail))
    {
      if (!ok)
	printf ("Failure: ");
      printf ("Test: %s\n", test_name);
      printf ("Result:\n");
      printf (" is:         %d\n", computed);
      printf (" should be:  %d\n", expected);
    }

  update_stats (ok, xfail);
}

#if 0  /* XXX scp XXX */
/* This is to prevent messages from the SVID libm emulation.  */
int
matherr (struct exception *x __attribute__ ((unused)))
{
  return 1;
}
#endif

/****************************************************************************
  Tests for single functions of libm.
  Please keep them alphabetically sorted!
****************************************************************************/

static void
copysign_test (void)
{
  init_max_error ();

  check_float ("copysign (0, 4) == 0",  FUNC(copysign) (0, 4), 0, 0, 0, 0);
  check_float ("copysign (0, -4) == -0",  FUNC(copysign) (0, -4), minus_zero, 0, 0, 0);
  check_float ("copysign (-0, 4) == 0",  FUNC(copysign) (minus_zero, 4), 0, 0, 0, 0);
  check_float ("copysign (-0, -4) == -0",  FUNC(copysign) (minus_zero, -4), minus_zero, 0, 0, 0);

  check_float ("copysign (inf, 0) == inf",  FUNC(copysign) (plus_infty, 0), plus_infty, 0, 0, 0);
  check_float ("copysign (inf, -0) == -inf",  FUNC(copysign) (plus_infty, minus_zero), minus_infty, 0, 0, 0);
  check_float ("copysign (-inf, 0) == inf",  FUNC(copysign) (minus_infty, 0), plus_infty, 0, 0, 0);
  check_float ("copysign (-inf, -0) == -inf",  FUNC(copysign) (minus_infty, minus_zero), minus_infty, 0, 0, 0);

  check_float ("copysign (0, inf) == 0",  FUNC(copysign) (0, plus_infty), 0, 0, 0, 0);
  check_float ("copysign (0, -0) == -0",  FUNC(copysign) (0, minus_zero), minus_zero, 0, 0, 0);
  check_float ("copysign (-0, inf) == 0",  FUNC(copysign) (minus_zero, plus_infty), 0, 0, 0, 0);
  check_float ("copysign (-0, -0) == -0",  FUNC(copysign) (minus_zero, minus_zero), minus_zero, 0, 0, 0);

  /* XXX More correctly we would have to check the sign of the NaN.  */
  check_float ("copysign (NaN, 0) == NaN",  FUNC(copysign) (nan_value, 0), nan_value, 0, 0, 0);
  check_float ("copysign (NaN, -0) == NaN",  FUNC(copysign) (nan_value, minus_zero), nan_value, 0, 0, 0);
  check_float ("copysign (-NaN, 0) == NaN",  FUNC(copysign) (-nan_value, 0), nan_value, 0, 0, 0);
  check_float ("copysign (-NaN, -0) == NaN",  FUNC(copysign) (-nan_value, minus_zero), nan_value, 0, 0, 0);

  print_max_error ("copysign", 0, 0);
}


static void
fabs_test (void)
{
  init_max_error ();

  check_float ("fabs (0) == 0",  FUNC(fabs) ((FLOAT)0.0), 0, 0, 0, 0);
  check_float ("fabs (-0) == 0",  FUNC(fabs) (minus_zero), 0, 0, 0, 0);

  check_float ("fabs (inf) == inf",  FUNC(fabs) (plus_infty), plus_infty, 0, 0, 0);
  check_float ("fabs (-inf) == inf",  FUNC(fabs) (minus_infty), plus_infty, 0, 0, 0);
  check_float ("fabs (NaN) == NaN",  FUNC(fabs) (nan_value), nan_value, 0, 0, 0);

  check_float ("fabs (38.0) == 38.0",  FUNC(fabs) ((FLOAT)38.0), 38.0, 0, 0, 0);
  check_float ("fabs (-e) == e",  FUNC(fabs) ((FLOAT)-M_El), M_El, 0, 0, 0);
  
  print_max_error ("fabs", 0, 0);
}


static void
fpclassify_test (void)
{
  init_max_error ();

  check_int ("fpclassify (NaN) == FP_NAN", fpclassify (nan_value), FP_NAN, 0, 0, 0);
  check_int ("fpclassify (inf) == FP_INFINITE", fpclassify (plus_infty), FP_INFINITE, 0, 0, 0);
  check_int ("fpclassify (-inf) == FP_INFINITE", fpclassify (minus_infty), FP_INFINITE, 0, 0, 0);
  check_int ("fpclassify (+0) == FP_ZERO", fpclassify (plus_zero), FP_ZERO, 0, 0, 0);
  check_int ("fpclassify (-0) == FP_ZERO", fpclassify (minus_zero), FP_ZERO, 0, 0, 0);
  check_int ("fpclassify (1000) == FP_NORMAL", fpclassify (1000.0), FP_NORMAL, 0, 0, 0);

  print_max_error ("fpclassify", 0, 0);
}

static void
isfinite_test (void)
{
  init_max_error ();

  check_bool ("isfinite (0) == true", isfinite (0.0), 1, 0, 0, 0);
  check_bool ("isfinite (-0) == true", isfinite (minus_zero), 1, 0, 0, 0);
  check_bool ("isfinite (10) == true", isfinite (10.0), 1, 0, 0, 0);
  check_bool ("isfinite (inf) == false", isfinite (plus_infty), 0, 0, 0, 0);
  check_bool ("isfinite (-inf) == false", isfinite (minus_infty), 0, 0, 0, 0);
  check_bool ("isfinite (NaN) == false", isfinite (nan_value), 0, 0, 0, 0);

  print_max_error ("isfinite", 0, 0);
}

static void
isnormal_test (void)
{
  init_max_error ();

  check_bool ("isnormal (0) == false", isnormal (0.0), 0, 0, 0, 0);
  check_bool ("isnormal (-0) == false", isnormal (minus_zero), 0, 0, 0, 0);
  check_bool ("isnormal (10) == true", isnormal (10.0), 1, 0, 0, 0);
  check_bool ("isnormal (inf) == false", isnormal (plus_infty), 0, 0, 0, 0);
  check_bool ("isnormal (-inf) == false", isnormal (minus_infty), 0, 0, 0, 0);
  check_bool ("isnormal (NaN) == false", isnormal (nan_value), 0, 0, 0, 0);

  print_max_error ("isnormal", 0, 0);
}



static void
signbit_test (void)
{

  init_max_error ();

  check_bool ("signbit (0) == false", signbit (0.0), 0, 0, 0, 0);
  check_bool ("signbit (-0) == true", signbit (minus_zero), 1, 0, 0, 0);
  check_bool ("signbit (inf) == false", signbit (plus_infty), 0, 0, 0, 0);
  check_bool ("signbit (-inf) == true", signbit (minus_infty), 1, 0, 0, 0);

  /* signbit (x) != 0 for x < 0.  */
  check_bool ("signbit (-1) == true", signbit (-1.0), 1, 0, 0, 0);
  /* signbit (x) == 0 for x >= 0.  */
  check_bool ("signbit (1) == false", signbit (1.0), 0, 0, 0, 0);

  print_max_error ("signbit", 0, 0);
}

static void
sqrt_test (void)
{
  errno = 0;
  FUNC(sqrt_my) (1);
  if (errno == ENOSYS)
    /* Function not implemented.  */
    return;

  init_max_error ();

  check_float ("sqrt (0) == 0",  FUNC(sqrt_my) (0), 0, 0, 0, 0);
  check_float ("sqrt (NaN) == NaN",  FUNC(sqrt_my) (nan_value), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("sqrt (inf) == inf",  FUNC(sqrt_my) (plus_infty), plus_infty, 0, 0, 0);

  check_float ("sqrt (-0) == -0",  FUNC(sqrt_my) (minus_zero), minus_zero, 0, 0, 0);

  /* sqrt (x) == NaN plus invalid exception for x < 0.  */
  check_float ("sqrt (-1) == NaN plus invalid exception",  FUNC(sqrt_my) (-1), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("sqrt (-inf) == NaN plus invalid exception",  FUNC(sqrt_my) (minus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("sqrt (NaN) == NaN",  FUNC(sqrt_my) (nan_value), nan_value, 0, 0, INVALID_EXCEPTION);

  check_float ("sqrt (2209) == 47",  FUNC(sqrt_my) (2209), 47, 0, 0, 0);
  check_float ("sqrt (4) == 2",  FUNC(sqrt_my) (4), 2, 0, 0, 0);
  check_float ("sqrt (2) == M_SQRT2l",  FUNC(sqrt_my) (2), M_SQRT2l, 0, 0, 0);
  check_float ("sqrt (0.25) == 0.5",  FUNC(sqrt_my) (0.25), 0.5, 0, 0, 0);
  check_float ("sqrt (6642.25) == 81.5",  FUNC(sqrt_my) (6642.25), 81.5, 0, 0, 0);
  check_float ("sqrt (15239.9025) == 123.45",  FUNC(sqrt_my) (15239.9025L), 123.45L, DELTA1562, 0, 0);
  check_float ("sqrt (0.7) == 0.83666002653407554797817202578518747",  FUNC(sqrt_my) (0.7L), 0.83666002653407554797817202578518747L, 0, 0, 0);

  print_max_error ("sqrt", DELTAsqrt, 0);
}

static void
sin_test (void)
{
  errno = 0;
  FUNC(sin) (0);
  if (errno == ENOSYS)
    /* Function not implemented.  */
    return;

  init_max_error ();

  check_float ("sin (0) == 0",  FUNC(sin) (0), 0, 0, 0, 0);
  check_float ("sin (-0) == -0",  FUNC(sin) (minus_zero), minus_zero, 0, 0, 0);
  check_float ("sin (inf) == NaN plus invalid exception",  FUNC(sin) (plus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("sin (-inf) == NaN plus invalid exception",  FUNC(sin) (minus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("sin (NaN) == NaN",  FUNC(sin) (nan_value), nan_value, 0, 0, INVALID_EXCEPTION);

  check_float ("sin (pi/6) == 0.5",  FUNC(sin) (M_PI_6l), 0.5, 0, 0, 0);
  check_float ("sin (-pi/6) == -0.5",  FUNC(sin) (-M_PI_6l), -0.5, 0, 0, 0);
  check_float ("sin (pi/2) == 1",  FUNC(sin) (M_PI_2l), 1, 0, 0, 0);
  check_float ("sin (pi) == 0",  FUNC(sin) (M_PI_2l * 2.0), 0, 0.5515, 0, 0);
  check_float ("sin (3pi/ 2) == -1",  FUNC(sin) (M_PI_2l * 3.0), -1, 0, 0, 0);
  check_float ("sin (2pi) == 0",  FUNC(sin) (M_PI_2l * 4.0), 0, 1.1031, 0, 0);
  check_float ("sin (-pi/2) == -1",  FUNC(sin) (-M_PI_2l), -1, 0, 0, 0);
  check_float ("sin (0.7) == 0.64421768723769105367261435139872014",  FUNC(sin) (0.7L), 0.64421768723769105367261435139872014L, DELTA1524, 0, 0);
  print_max_error ("sin", DELTAsin, 0);

}
static void
cos_test (void)
{
  errno = 0;
  FUNC(cos) (0);
  if (errno == ENOSYS)
    /* Function not implemented.  */
    return;

  init_max_error ();

  check_float ("cos (0) == 1",  FUNC(cos) (0), 1, 0, 0, 0);
  check_float ("cos (-0) == 1",  FUNC(cos) (minus_zero), 1, 0, 0, 0);
  check_float ("cos (inf) == NaN plus invalid exception",  FUNC(cos) (plus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("cos (-inf) == NaN plus invalid exception",  FUNC(cos) (minus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("cos (NaN) == NaN",  FUNC(cos) (nan_value), nan_value, 0, 0, INVALID_EXCEPTION);

  check_float ("cos (M_PI_6l * 2.0) == 0.5",  FUNC(cos) (M_PI_6l * 2.0), 0.5, DELTA582, 0, 0);
  check_float ("cos (M_PI_6l * 4.0) == -0.5",  FUNC(cos) (M_PI_6l * 4.0), -0.5, DELTA583, 0, 0);
  check_float ("cos (pi/2) == 0",  FUNC(cos) (M_PI_2l), 0, DELTA584, 0, 0);
  check_float ("cos (pi) == -1",  FUNC(cos) (M_PI_2l * 2.0), -1, DELTA584, 0, 0);
  check_float ("cos (3 * pi / 2) == 0",  FUNC(cos) (M_PI_2l * 3.0), 0, 0.8273, 0, 0);
  check_float ("cos (4 * pi / 2) == 1",  FUNC(cos) (M_PI_2l * 4.0), 1, DELTA584, 0, 0);




  check_float ("cos (0.7) == 0.76484218728448842625585999019186495",  FUNC(cos) (0.7L), 0.76484218728448842625585999019186495L, DELTA585, 0, 0);

  print_max_error ("cos", DELTAcos, 0);
}
static void
tan_test (void)
{
  errno = 0;
  FUNC(tan) (0);
  if (errno == ENOSYS)
    /* Function not implemented.  */
    return;

  init_max_error ();

  check_float ("tan (0) == 0",  FUNC(tan) (0), 0, 0, 0, 0);
  check_float ("tan (-0) == -0",  FUNC(tan) (minus_zero), minus_zero, 0, 0, 0);
  check_float ("tan (inf) == NaN plus invalid exception",  FUNC(tan) (plus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("tan (-inf) == NaN plus invalid exception",  FUNC(tan) (minus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("tan (NaN) == NaN",  FUNC(tan) (nan_value), nan_value, 0, 0, INVALID_EXCEPTION);

  check_float ("tan (pi/4) == 1",  FUNC(tan) (M_PI_4l), 1, DELTA1569, 0, 0);
  check_float ("tan (0.7) == 0.84228838046307944812813500221293775",  FUNC(tan) (0.7L), 0.84228838046307944812813500221293775L, DELTA1570, 0, 0);
  print_max_error ("tan", DELTAtan, 0);
}

static void
asin_test (void)
{
  errno = 0;
  FUNC(asin) (0);
  if (errno == ENOSYS)
    /* Function not implemented.  */
    return;

  init_max_error ();

  check_float ("asin (inf) == NaN plus invalid exception",  FUNC(asin) (plus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("asin (-inf) == NaN plus invalid exception",  FUNC(asin) (minus_infty), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("asin (NaN) == NaN",  FUNC(asin) (nan_value), nan_value, 0, 0, INVALID_EXCEPTION);

  /* asin x == NaN plus invalid exception for |x| > 1.  */
  check_float ("asin (1.1) == NaN plus invalid exception",  FUNC(asin) (1.1L), nan_value, 0, 0, INVALID_EXCEPTION);
  check_float ("asin (-1.1) == NaN plus invalid exception",  FUNC(asin) (-1.1L), nan_value, 0, 0, INVALID_EXCEPTION);

  check_float ("asin (0) == 0",  FUNC(asin) (0), 0, 0, 0, 0);
  check_float ("asin (-0) == -0",  FUNC(asin) (minus_zero), minus_zero, 0, 0, 0);
  check_float ("asin (0.5) == pi/6",  FUNC(asin) (0.5), M_PI_6l, DELTA24, 0, 0);
  check_float ("asin (-0.5) == -pi/6",  FUNC(asin) (-0.5), -M_PI_6l, DELTA25, 0, 0);
  check_float ("asin (1.0) == pi/2",  FUNC(asin) (1.0), M_PI_2l, DELTA26, 0, 0);
  check_float ("asin (-1.0) == -pi/2",  FUNC(asin) (-1.0), -M_PI_2l, DELTA27, 0, 0);
  check_float ("asin (0.7) == 0.77539749661075306374035335271498708",  FUNC(asin) (0.7L), 0.77539749661075306374035335271498708L, DELTA28, 0, 0);

  print_max_error ("asin", DELTAasin, 0);
}

static void
initialize (void)
{
  plus_zero = 0.0;
  nan_value = 0.0;
	int32_t high_word = 0x7FF40000;
	int32_t low_word = 0x00000000;
	SET_HIGH_WORD(nan_value, high_word);
	SET_LOW_WORD(nan_value, low_word);
  minus_zero = FUNC(copysign) (0.0, -1.0);
  plus_infty = CHOOSE (HUGE_VALL, HUGE_VAL, HUGE_VALF,
		       HUGE_VALL, HUGE_VAL, HUGE_VALF);
  minus_infty = CHOOSE (-HUGE_VALL, -HUGE_VAL, -HUGE_VALF,
			-HUGE_VALL, -HUGE_VAL, -HUGE_VALF);
  (void) &plus_zero;
  (void) &nan_value;
  (void) &minus_zero;
  (void) &plus_infty;
  (void) &minus_infty;

  /* Clear all exceptions.  From now on we must not get random exceptions.  */
  feclearexcept (FE_ALL_EXCEPT);
}

#if 0
/* function to check our ulp calculation.  */
void
check_ulp (void)
{
  int i;

  FLOAT u, diff, ulp;
  /* This gives one ulp.  */
  u = FUNC(nextafter) (10, 20);
  check_equal (10.0, u, 1, &diff, &ulp);
  printf ("One ulp: % .4" PRINTF_NEXPR "\n", ulp);

  /* This gives one more ulp.  */
  u = FUNC(nextafter) (u, 20);
  check_equal (10.0, u, 2, &diff, &ulp);
  printf ("two ulp: % .4" PRINTF_NEXPR "\n", ulp);

  /* And now calculate 100 ulp.  */
  for (i = 2; i < 100; i++)
    u = FUNC(nextafter) (u, 20);
  check_equal (10.0, u, 100, &diff, &ulp);
  printf ("100 ulp: % .4" PRINTF_NEXPR "\n", ulp);
}
#endif

int
main (int argc, char **argv)
{
#if 0 /* XXX scp XXX */
  int remaining;
#endif

  double m_inf = 0;
  SET_HIGH_WORD(m_inf, 0xfff00000);
  SET_LOW_WORD(m_inf, 0x00000000);

  double inf = 0;
  SET_HIGH_WORD(inf, 0x7ff00000);
  SET_LOW_WORD(inf, 0x00000000);

  double m_zero = 0.0;
  SET_HIGH_WORD(m_zero, 0x80000000);
  SET_LOW_WORD(m_zero, 0x0);

  double zero = 0.0;
  SET_HIGH_WORD(zero, 0x00000000);
  SET_LOW_WORD(zero, 0x0);

  double q_nan = 0.0;
  SET_HIGH_WORD(q_nan, 0x7ff80000);
  SET_LOW_WORD(q_nan, 0x0);

  double s_nan = 0.0;
  SET_HIGH_WORD(s_nan, 0x7ff40000);
  SET_LOW_WORD(s_nan, 0x0);

  double onee300 = 1.0e300;
  double m_onee300 = -1.0e300;
  double oneeminus300 = 1.0e-300;
  double m_oneeminus300 = -1.0e-300;
  double one = 1.0;

  // 检查溢出异常
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  double ex = onee300 * onee300;
  printf("1.0e300 * 1.0e300 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_OVERFLOW)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("1.0e300 * 1.0e300 is overflow\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :1.0e300 * 1.0e300\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_onee300 * m_onee300;
  printf("-1.0e300 * -1.0e300 is %f %d\n", ex, isNormal(ex));

  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_OVERFLOW)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("-1.0e300 * -1.0e300 is overflow\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :-1.0e300 * -1.0e300\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = oneeminus300 * oneeminus300;
  printf("1.0e-300 * 1.0e-300 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_UNDERFLOW)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("1.0e-300 * 1.0e-300 is underflow\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :1.0e-300 * 1.0e-300\n");
      }
  } else {
      printf("no exception.\n");
  }  

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_oneeminus300 * m_oneeminus300;
  printf("-1.0e-300 * -1.0e-300 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_UNDERFLOW)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("-1.0e-300 * -1.0e-300 is underflow\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :-1.0e-300 * -1.0e-300\n");
      }
  } else {
      printf("no exception.\n");
  }





//检测0/0
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero / zero;
  printf("0 / 0 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_DIVBYZERO)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("0 / 0 is divbyzero\n");
      } else if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("0 / 0 is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :0 / 0 \n");
      }
  } else {
      printf("no exception.\n");
  } 

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero / m_zero;
  printf("0 / -0 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_DIVBYZERO)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("0 / -0 is divbyzero\n");
      } else if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("0 / -0 is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :0 / -0 \n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero / zero;
  printf("-0 / 0 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_DIVBYZERO)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("-0 / 0 is divbyzero\n");
      } else if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("-0 / 0 is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :-0 / 0 \n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero / m_zero;
  printf("-0 / -0 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_DIVBYZERO)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("-0 / -0 is divbyzero\n");
      } else if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("-0 / -0 is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :-0 / -0 \n");
      }
  } else {
      printf("no exception.\n");
  }


//检查1/0

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = 1.0 / zero;
  printf("1.0 / 0.0 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_DIVBYZERO)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("1.0 / 0.0 is divbyzero\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :1.0 / 0.0\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = 1.0 / m_zero;
  printf("1.0 / -0.0 is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_DIVBYZERO)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("1.0 / -0.0 is divbyzero\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :1.0 / -0.0\n");
      }
  } else {
      printf("no exception.\n");
  }

// qNaN qNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan + q_nan;
  printf("qnan + qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan + qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan + qnan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan - q_nan;
  printf("qnan - qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan - qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan - qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan * q_nan;
  printf("qnan * qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan * qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan * qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan / q_nan;
  printf("qnan / qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan / qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan / qnan\n");
      }
  } else {
      printf("no exception.\n");
  }



// sNaN sNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan + s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan + snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan + snan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan - s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan - snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan - snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan * s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan * snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan * snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan / s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan / snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan / snan\n");
      }
  } else {
      printf("no exception.\n");
  }


// inf inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf + inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf + inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf + inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf - inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf - inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf - inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf * inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf * inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf * inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf / inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf / inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf / inf\n");
      }
  } else {
      printf("no exception.\n");
  }

// m_inf m_inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf + m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf + m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf + m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf - m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf - m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf - m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf * m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf * m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf * m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf / m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf / m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf / m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


// qNaN zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan + zero;
  printf("qnan + zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan + zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan + zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan - zero;
  printf("qnan - zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan - zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan - zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan * zero;
  printf("qnan * zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan * zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan * zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan / zero;
  printf("qnan / zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan / zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan / zero\n");
      }
  } else {
      printf("no exception.\n");
  }

// snan zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan + zero;
  printf("snan + zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan + zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan + zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan - zero;
  printf("snan - zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan - zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan - zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan * zero;
  printf("snan * zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan * zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan * zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan / zero;
  printf("snan / zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan / zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan / zero\n");
      }
  } else {
      printf("no exception.\n");
  }

// inf zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf + zero;
  printf("inf + zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf + zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf + zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf - zero;
  printf("inf - zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf - zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf - zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf * zero;
  printf("inf * zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf * zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf * zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf / zero;
  printf("inf / zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf / zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf / zero\n");
      }
  } else {
      printf("no exception.\n");
  }


// m_inf zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf + zero;
  printf("m_inf + zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf + zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf + zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf - zero;
  printf("m_inf - zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf - zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf - zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf * zero;
  printf("m_inf * zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf * zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf * zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf / zero;
  printf("m_inf / zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf / zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf / zero\n");
      }
  } else {
      printf("no exception.\n");
  }



// qNaN m_zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan + m_zero;
  printf("qnan + m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan + m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan + m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan - m_zero;
  printf("qnan - m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan - m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan - m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan * m_zero;
  printf("qnan * m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan * m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan * m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan / m_zero;
  printf("qnan / m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan / m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan / m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

// snan m_zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan + m_zero;
  printf("snan + m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan + m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan + m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan - m_zero;
  printf("snan - m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan - m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan - m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan * m_zero;
  printf("snan * m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan * m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan * m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan / m_zero;
  printf("snan / m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan / m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan / m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

// inf m_zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf + m_zero;
  printf("inf + m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf + m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf + m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf - m_zero;
  printf("inf - m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf - m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf - m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf * m_zero;
  printf("inf * m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf * m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf * m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf / m_zero;
  printf("inf / m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf / m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf / m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


// m_inf m_zero
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf + m_zero;
  printf("m_inf + m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf + m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf + m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf - m_zero;
  printf("m_inf - m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf - m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf - m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf * m_zero;
  printf("m_inf * m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf * m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf * m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf / m_zero;
  printf("m_inf / m_zero is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf / m_zero is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf / m_zero\n");
      }
  } else {
      printf("no exception.\n");
  }

// zero qNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero + q_nan;
  printf("zero + qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero + qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero + qnan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero - q_nan;
  printf("zero - qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero - qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero - qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero * q_nan;
  printf("zero * qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero * qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero * qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero / q_nan;
  printf("zero / qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero / qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero / qnan\n");
      }
  } else {
      printf("no exception.\n");
  }



// zero sNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero + s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero + snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero + snan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero - s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero - snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero - snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero * s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero * snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero * snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero / s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero / snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero / snan\n");
      }
  } else {
      printf("no exception.\n");
  }


// zero inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero + inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero + inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero + inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero - inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero - inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero - inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero * inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero * inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero * inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero / inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero / inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero / inf\n");
      }
  } else {
      printf("no exception.\n");
  }

// zero m_inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero + m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero + m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero + m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero - m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero - m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero - m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero * m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero * m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero * m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = zero / m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("zero / m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :zero / m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


// m_zero qNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero + q_nan;
  printf("m_zero + qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero + qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero + qnan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero - q_nan;
  printf("m_zero - qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero - qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero - qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero * q_nan;
  printf("m_zero * qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero * qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero * qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero / q_nan;
  printf("m_zero / qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero / qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero / qnan\n");
      }
  } else {
      printf("no exception.\n");
  }



// m_zero sNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero + s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero + snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero + snan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero - s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero - snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero - snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero * s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero * snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero * snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero / s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero / snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero / snan\n");
      }
  } else {
      printf("no exception.\n");
  }


// m_zero inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero + inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero + inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero + inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero - inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero - inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero - inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero * inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero * inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero * inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero / inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero / inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero / inf\n");
      }
  } else {
      printf("no exception.\n");
  }

// m_zero m_inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero + m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero + m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero + m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero - m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero - m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero - m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero * m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero * m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero * m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_zero / m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_zero / m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_zero / m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


// qNaN one
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan + one;
  printf("qnan + one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan + one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan + one\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan - one;
  printf("qnan - one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan - one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan - one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan * one;
  printf("qnan * one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan * one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan * one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = q_nan / one;
  printf("qnan / one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("qnan / one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :qnan / one\n");
      }
  } else {
      printf("no exception.\n");
  }

// snan one
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan + one;
  printf("snan + one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan + one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan + one\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan - one;
  printf("snan - one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan - one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan - one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan * one;
  printf("snan * one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan * one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan * one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = s_nan / one;
  printf("snan / one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("snan / one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :snan / one\n");
      }
  } else {
      printf("no exception.\n");
  }

// inf one
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf + one;
  printf("inf + one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf + one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf + one\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf - one;
  printf("inf - one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf - one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf - one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf * one;
  printf("inf * one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf * one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf * one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = inf / one;
  printf("inf / one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("inf / one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :inf / one\n");
      }
  } else {
      printf("no exception.\n");
  }


// m_inf one
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf + one;
  printf("m_inf + one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf + one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf + one\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf - one;
  printf("m_inf - one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf - one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf - one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf * one;
  printf("m_inf * one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf * one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf * one\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = m_inf / one;
  printf("m_inf / one is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("m_inf / one is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :m_inf / one\n");
      }
  } else {
      printf("no exception.\n");
  }

// one qNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one + q_nan;
  printf("one + qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one + qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one + qnan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one - q_nan;
  printf("one - qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one - qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one - qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one * q_nan;
  printf("one * qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one * qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one * qnan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one / q_nan;
  printf("one / qnan is %f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one / qnan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one / qnan\n");
      }
  } else {
      printf("no exception.\n");
  }



// one sNaN
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one + s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one + snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one + snan\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one - s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one - snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one - snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one * s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one * snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one * snan\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one / s_nan;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one / snan is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one / snan\n");
      }
  } else {
      printf("no exception.\n");
  }


// one inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one + inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one + inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one + inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one - inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one - inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one - inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one * inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one * inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one * inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one / inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one / inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one / inf\n");
      }
  } else {
      printf("no exception.\n");
  }

// one m_inf
  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one + m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one + m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one + m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }

  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one - m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one - m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one - m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one * m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one * m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one * m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }


  if (fetestexcept(FE_ALL_EXCEPT)) {
      feclearexcept(FE_ALL_EXCEPT);
      printf("error have exceptions.\n");
  } else {
      printf("no exception.\n");
  }
  ex = one / m_inf;
  printf("%f %d\n", ex, isNormal(ex));
  if (fetestexcept(FE_ALL_EXCEPT)) {
      if(fetestexcept(FE_INVALID)) {
        feclearexcept(FE_ALL_EXCEPT);
        printf("one / m_inf is invalid\n");
      } else {
        feclearexcept(FE_ALL_EXCEPT);
        printf("error exception :one / m_inf\n");
      }
  } else {
      printf("no exception.\n");
  }






























  printf("\n\n\n\n\n\n%f\n", pow(0, 0));

  printf("%f\n", pow(0, m_inf));

  printf("%f\n", pow(m_inf, 0.5));

  printf("%f\n", floor_my(m_inf));
  double m_nan = 0;
  SET_HIGH_WORD(m_nan, 0xfff10000);
  SET_LOW_WORD(m_nan, 0x00000000);
  printf("%f\n", m_nan + m_nan);
 
  printf("%f\n", floor_my(m_nan));


  printf("%f\n", ceil_my(3.8e30));

  printf("%f\n", (float)atan_my((float)-0.0));

  double pi = 3.1415926535897931160E+00;
  printf("%f\n", (float)tan((float)pi/2));
  printf("%f\n", pow(m_inf, 0.5));
  

  printf("\natan2 = %f\n", atan2(m_zero, m_zero));
  double floor_x = -0.5;
  floor_my (floor_x);
  double threeefive = 0;
  SET_HIGH_WORD(threeefive, 0x7ff00000);
  SET_LOW_WORD(threeefive, 0x0);
  printf("\n\n threefive %.17f \n\n", threeefive);
  double atan2_result = atan2(threeefive, -threeefive);
  printf("\n\n new \n\n");
  printf("\n\n atan2_result %.17f \n\n", atan2_result);

  int input1_val = 1;
  int input2_val = 2;
  int *input1_ptr = &input1_val;
  int *input2_ptr = &input2_val;
  double x[1] = {3.0};
  double y[3]={0.0, 0.0, 0.0};
  int e0 = 0;
  int nx = 1;
  int prec = 0;
  union IEEEd2bits temp_x;
  u_int32_t temp_x_high = 0xc3200000;
  u_int32_t temp_x_low = 0xffffffff;
  SET_HIGH_WORD(temp_x.d, temp_x_high);
  SET_LOW_WORD(temp_x.d, temp_x_low);
  GET_HIGH_WORD(temp_x_high, temp_x.d);
  GET_LOW_WORD(temp_x_low,temp_x.d);
  printf("\n temp_x_high is %x \n", temp_x_high);
  temp_x.d = -0.5;
  SET_LOW_WORD(temp_x.d, 0x0);
  floor_my(temp_x.d);

  for(temp_x_low = 0x0;temp_x_low <= 0xffffffff;temp_x_low ++)
  {
    SET_LOW_WORD(temp_x.d, temp_x_low);
    floor_my(temp_x.d);
  }
  floor_my(temp_x.d);

  // for(temp_x_high = 0x1; temp_x_high <= 0xffffffff;temp_x_high ++)
  // {
  //   SET_HIGH_WORD(x[0], temp_x_high);
  //   for(temp_x_low = 0x1;temp_x_low <= 0xffffffff;temp_x_low ++)
  //   {
  //     printf("\n temp_x_high is %x ", temp_x_high);
  //     printf(" temp_x_low is %x \n", temp_x_low);
  //     SET_LOW_WORD(x[0], temp_x_low);
  //     int n = __kernel_rem_pio2(x, y, e0, nx, prec);
  //   }
  // }
  int n = __kernel_rem_pio2(x, y, e0, nx, prec);
  printf("\n\n n = %d \n\n", n);
  printf("\n\n y0 = %f \n\n", y[0]);
  printf("\n\n y1 = %f \n\n", y[1]);

  // double x = 0;
  // SET_HIGH_WORD(x, 0x7fe11111);
  // SET_LOW_WORD(x, 0x11111111);
  // x = -1.79E+307;
  
  // double n = (1 << 31) - 1;
  // n = 1844;
  // n = ldexp(x, n);
  // printf("\n\n temp = %f \n\n", n);
  

  temp_x_high = 0x00080000;
  temp_x_low = 0x00008abc;
  SET_HIGH_WORD(temp_x.d, temp_x_high);
  // for(temp_x_low = 0x1;temp_x_low < 0xffffffff;temp_x_low ++)
  // {
  //   printf("\n temp_x_low is %x \n", temp_x_low);
  //   SET_LOW_WORD(temp_x.d, temp_x_low);
  //   sqrt_my(temp_x.d);

  // }
  SET_LOW_WORD(temp_x.d, temp_x_low);
  temp_x.d = 1.1125369292711479e-308;
  sqrt_my(temp_x.d);




  // temp_x.d = 2.0;
  // int32_t temp_x_high;
  // int32_t temp_x_low;
  // GET_HIGH_WORD(temp_x_high, temp_x.d);
  // GET_LOW_WORD(temp_x_low, temp_x.d);
  // printf("\n\n %x \n\n", temp_x_high);
  // printf("\n\n %x \n\n", temp_x_low);
  

  printf("%f\n", pow(-1.0, inf));
  double huge		= 1.0e+300;
  double little = 2.0e-300;
  printf("\n\n\noverflow is %f\n\n\n", huge * huge);
  printf("%f\n", -2.0 * little);
  double result = 0.0;
  int32_t high_word = 0x7f800000;
  int32_t low_word = 0x00000000;
  SET_HIGH_WORD(result, high_word);
  SET_LOW_WORD(result, low_word);
  float result_f = 0.0;
  SET_FLOAT_WORD(result_f, high_word);
  union IEEEf2bits u;
	u.f = result_f;
  printf("test result of %f = %d\n", result_f, isNormalf(0));
  // printf("sin(1.7e308) = %.20f\n", sin(1.7e308));
  // printf("sin(-1.7e308) = %.20f\n", sin(-1.7e308));
  // printf("sin(1.7e309) = %.20f\n", sin(1.7e309));
  // printf("sin(-1.7e309) = %.20f\n", sin(-1.7e309));
  // printf("sin(-0) = %.20f\n", sin(-0.0));
  
  // printf("sin(3.4e38) = %.20f\n", (float)(sin((float)3.4e38)));
  // printf("sin(-3.4e38) = %.20f\n", (float)(sin((float)-3.4e38)));
  // printf("sin(3.4e39) = %.20f\n", (float)(sin((float)3.4e39)));
  // printf("sin(-3.4e39) = %.20f\n", (float)(sin((float)-3.4e39)));
  // printf("sin(-0) = %.20f\n", (float)(sin((float)-0.0)));
  // printf("%f\n", asin(1.000001));
  // printf("%f\n", asin(2.0));

  // sqrt_my(64.0);
  verbose = 1;
  output_ulps = 1;
  output_max_error = 1;
  output_points = 1;
  /* XXX set to 0 for releases.  */
  ignore_max_ulp = 0;
  
  // high_word = 0x3e500000;
  // low_word = 0xffffffff;
  // SET_HIGH_WORD(result, high_word);
  // SET_LOW_WORD(result, low_word);
  // printf("%.30f\n", result);
  // printf("%.16f", sqrt_my(30));
  
#if 0 /* XXX scp XXX */
  /* Parse and process arguments.  */
  argp_parse (&argp, argc, argv, 0, &remaining, NULL);

  if (remaining != argc)
    {
      fprintf (stderr, "wrong number of arguments");
      argp_help (&argp, stdout, ARGP_HELP_SEE, program_invocation_short_name);
      exit (EXIT_FAILURE);
    }
#endif

  if (output_ulps)
    {
      ulps_file = fopen ("ULPs", "a");
      if (ulps_file == NULL)
	{
	  perror ("can't open file `ULPs' for writing: ");
	  exit (1);
	}
    }


  initialize ();
  printf (TEST_MSG);

#if 0
  check_ulp ();
#endif

  /* Keep the tests a wee bit ordered (according to ISO C99).  */
  /* Classification macros:  */
  fpclassify_test ();
  isfinite_test ();
  isnormal_test ();
  signbit_test ();
  
  /* Power and absolute value functions:  */
  fabs_test ();

  /* Trigonometric functions:  */
  sin_test ();
  cos_test ();
  tan_test ();
  asin_test ();
  /* Manipulation functions:  */
  copysign_test ();
  sqrt_test ();
  /* maximum, minimum and positive difference functions */
  if (output_ulps)
    fclose (ulps_file);

  printf ("\nTest suite completed:\n");
  printf ("  %d test cases plus %d tests for exception flags executed.\n",
	  noTests, noExcTests);
  if (noXFails)
    printf ("  %d expected failures occurred.\n", noXFails);
  if (noXPasses)
    printf ("  %d unexpected passes occurred.\n", noXPasses);
  if (noErrors)
    {
      printf ("  %d errors occurred.\n", noErrors);
      return 1;
    }
  printf ("  All tests passed successfully.\n");

  return 0;
}

/*
 * Local Variables:
 * mode:c
 * End:
 */
