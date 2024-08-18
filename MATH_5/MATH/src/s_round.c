#include "cdefs-compat.h"
#include <MATH_math.h>

#include "math_private.h"

OLM_DLLEXPORT double
round(double x)
{
	double t;
	uint32_t hx;

	GET_HIGH_WORD(hx, x);
	if ((hx & 0x7fffffff) == 0x7ff00000)
		return (x + x);

	if (!(hx & 0x80000000)) {
		t = floor_my(x);
		if (t - x <= -0.5)
			t += 1;
		return (t);
	} else {
		t = floor_my(-x);
		if (t + x <= -0.5)
			t += 1;
		return (-t);
	}
}
