#include "cdefs-compat.h"
#include <MATH_math.h>
#include "math_private.h"

#define	DECL(type, fn)			\
OLM_DLLEXPORT type					\
fn(type x, type y)			\
{					\
					\
	if (isnan(x))			\
		return (x);		\
	if (isnan(y))			\
		return (y);		\
	return (x > y ? x - y : 0.0);	\
}

DECL(double, fdim)
