#include <MATH_math.h>

#include "fpmath.h"
#include "math_private.h"

OLM_DLLEXPORT int isNormal(double d)
{
    union IEEEd2bits u;
	u.d = d;
    if(u.bits.exp == 2047 && (u.bits.manl != 0 || u.bits.manh != 0)){
        /*如果是SNaN，返回-1*/
        if(u.bits.manh < 524288) return -1;
        /*如果是QNaN，返回-2*/
        return -2;
    }
    if(u.bits.exp == 2047 && u.bits.manl == 0 && u.bits.manh == 0)
    {
        /*如果是Inf，返回-3*/
        if(u.bits.sign == 0) return -3;
        /*如果是-Inf，返回-4*/
        if(u.bits.sign == 1) return -4;
    }
    /*正常范围内数值返回0*/
    return 0;

}

OLM_DLLEXPORT int isNormalf(float f)
{
    union IEEEf2bits u;
	u.f = f;
    if(u.bits.exp == 255 && (u.bits.man != 0)){
        /*如果是SNaN，返回-1*/
        if(u.bits.man < 4194304) return -1;
        /*如果是QNaN，返回-2*/
        return -2;
    }
    if(u.bits.exp == 255 && u.bits.man == 0)
    {
        /*如果是Inf，返回-3*/
        if(u.bits.sign == 0) return -3;
        /*如果是-Inf，返回-4*/
        if(u.bits.sign == 1) return -4;
    }
    /*正常范围内数值返回0*/
    return 0;

}