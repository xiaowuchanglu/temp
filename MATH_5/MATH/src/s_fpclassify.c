#include <MATH_math.h>
#include "math_private.h"
#include "fpmath.h"
OLM_DLLEXPORT int
__fpclassifyd(double d)
{
	union IEEEd2bits u;

	u.d = d;
	if (u.bits.exp == 2047) {
		if (u.bits.manl == 0 && u.bits.manh == 0) {
			return FP_INFINITE;
		} else {
			return FP_NAN;
		}
	} else if (u.bits.exp != 0) {
		return FP_NORMAL;
	} else if (u.bits.manl == 0 && u.bits.manh == 0) {
		return FP_ZERO;
	} else {
		return FP_SUBNORMAL;
	}
}