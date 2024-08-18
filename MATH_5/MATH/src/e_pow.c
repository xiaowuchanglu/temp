
/* __ieee754_pow(x,y) return x**y
 *
 *		      n
 * Method:  Let x =  2   * (1+f)
 *	1. Compute and return log2(x) in two pieces:
 *		log2(x) = w1 + w2,
 *	   where w1 has 53-24 = 29 bit trailing zeros.
 *	2. Perform y*log2(x) = n+y' by simulating muti-precision 
 *	   arithmetic, where |y'|<=0.5.
 *	3. Return x**y = 2**n*exp(y'*log2)
 *
 * Special cases:
 *	1.  (anything) ** 0  is 1
 *	2.  (anything) ** 1  is itself
 *	3.  (anything) ** NAN is NAN
 *	4.  NAN ** (anything except 0) is NAN
 *	5.  +-(|x| > 1) **  +INF is +INF
 *	6.  +-(|x| > 1) **  -INF is +0
 *	7.  +-(|x| < 1) **  +INF is +0
 *	8.  +-(|x| < 1) **  -INF is +INF
 *	9.  +-1         ** +-INF is NAN
 *	10. +0 ** (+anything except 0, NAN)               is +0
 *	11. -0 ** (+anything except 0, NAN, odd integer)  is +0
 *	12. +0 ** (-anything except 0, NAN)               is +INF
 *	13. -0 ** (-anything except 0, NAN, odd integer)  is +INF
 *	14. -0 ** (odd integer) = -( +0 ** (odd integer) )
 *	15. +INF ** (+anything except 0,NAN) is +INF
 *	16. +INF ** (-anything except 0,NAN) is +0
 *	17. -INF ** (anything)  = -0 ** (-anything)
 *	18. (-anything) ** (integer) is (-1)**(integer)*(+anything**integer)
 *	19. (-anything except 0 and inf) ** (non-integer) is NAN
 *
 * Accuracy:
 *	pow(x,y) returns x**y nearly rounded. In particular
 *			pow(integer,integer)
 *	always returns the correct integer provided it is 
 *	representable.
 *
 * Constants :
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 */

#include <MATH_math.h>

#include "math_private.h"
#ifdef __powerpc__
typedef __int32_t int32_t;
#endif

static const double
bp[] = {1.0, 1.5,},
dp_h[] = { 0.0, 5.84962487220764160156e-01,}, /* 0x3FE2B803, 0x40000000 */
dp_l[] = { 0.0, 1.35003920212974897128e-08,}, /* 0x3E4CFDEB, 0x43CFD006 */
zero    =  0.0,
one	=  1.0,
two	=  2.0,
two53	=  9007199254740992.0,	/* 0x43400000, 0x00000000 */
huge	=  1.0e300,
tiny    =  1.0e-300,
	/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
L1  =  5.99999999999994648725e-01, /* 0x3FE33333, 0x33333303 */
L2  =  4.28571428578550184252e-01, /* 0x3FDB6DB6, 0xDB6FABFF */
L3  =  3.33333329818377432918e-01, /* 0x3FD55555, 0x518F264D */
L4  =  2.72728123808534006489e-01, /* 0x3FD17460, 0xA91D4101 */
L5  =  2.30660745775561754067e-01, /* 0x3FCD864A, 0x93C9DB65 */
L6  =  2.06975017800338417784e-01, /* 0x3FCA7E28, 0x4A454EEF */
P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
P5   =  4.13813679705723846039e-08, /* 0x3E663769, 0x72BEA4D0 */
lg2  =  6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
lg2_h  =  6.93147182464599609375e-01, /* 0x3FE62E43, 0x00000000 */
lg2_l  = -1.90465429995776804525e-09, /* 0xBE205C61, 0x0CA86C39 */
ovt =  8.0085662595372944372e-0017, /* -(1024-log2(ovfl+.5ulp)) */
cp    =  9.61796693925975554329e-01, /* 0x3FEEC709, 0xDC3A03FD =2/(3ln2) */
cp_h  =  9.61796700954437255859e-01, /* 0x3FEEC709, 0xE0000000 =(float)cp */
cp_l  = -7.02846165095275826516e-09, /* 0xBE3E2FE0, 0x145B01F5 =tail of cp_h*/
ivln2    =  1.44269504088896338700e+00, /* 0x3FF71547, 0x652B82FE =1/ln2 */
ivln2_h  =  1.44269502162933349609e+00, /* 0x3FF71547, 0x60000000 =24b 1/ln2*/
ivln2_l  =  1.92596299112661746887e-08; /* 0x3E54AE0B, 0xF85DDF44 =1/ln2 tail*/

/*
	e_pow.c����Ҫ�����Ǽ��㸡������������ģ�������x��y�η���������ʵ�ֻ���һ�ֳ�Ϊ��������ָ���ĳ˷�������ѧԭ����
	�������ԭ�������ǿ��Խ�������ת��Ϊ�˷��Ͷ������㣬Ȼ����ͨ��ָ������õ������������˵���ݺ���x��y�η�����ͨ��������ָ���Ĺ�ϵ�����㣬�� x^y = 2 ^ (y * log2(x))��
	Pow�����Ķ������ֵ��������˫���ȸ������������Ǹú����Ĺ���ԭ������ϸ���ͣ�
	���ȣ��������������x��y�Ƿ�������ֵ������0��1�������ͷ�����NaN�����������Щ����ֵ��������ֱ�ӷ�����Ӧ�Ľ����
	Ȼ�󣬺�������y�Ƿ������������x�Ǹ�������y���������������᷵�ط�����NaN�������x�Ǹ�������y������������ķ��Żᱻ��Ϊ����
	����������������y�ľ���ֵ�Ƿ����2^31������ǣ����������x��ֵ����������󡢸�������0��
	Ȼ�󣬺��������log2(x)��ֵ�����������Ϊ�����֣�һ����ȷ��29λ���������ֺ�һ��С�����֡�
	���ţ�������ͨ��ģ��ྫ������������y*log2(x)��ֵ������������Ƚ�y��Ϊ�������ֺ�С�����֣�Ȼ��ֱ���log2(x)���������ֺ�С��������ˣ�Ȼ���ĸ��˻���ӵõ������
	��󣬺�����ͨ��ָ�����㣬���㲢����2��(ylog2(x))�η���ֵ������������Ƚ�ylog2(x)��Ϊ�������ֺ�С�����֣��ֱ�����2��С�����ֵĴη���2���������ֵĴη���Ȼ�������������˵õ����ս����
	����������������yΪ0������1�����xΪ1������1����ʹy��NaN�����x��y��NaN������NaN������xΪ������yΪ���������������yΪ�������ض�����ֵ�����������xΪ�����0���ض�����ֵ�������
*/
OLM_DLLEXPORT double
__ieee754_pow(double x, double y)
{
	double z=0,ax=0,z_h=0,z_l=0,p_h=0,p_l=0;
	double y1=0,t1=0,t2=0,r=0,s=0,t=0,u=0,v=0,w=0;
	int32_t i=0,j=0,k=0,yisint=0,n=0;
	int32_t hx=0,hy=0,ix=0,iy=0;
	u_int32_t lx=0,ly=0;

	EXTRACT_WORDS(hx,lx,x);
	EXTRACT_WORDS(hy,ly,y);
	ix = hx&0x7fffffff;		//	x��λ�ľ���ֵ
	iy = hy&0x7fffffff;		//	y��λ�ľ���ֵ

    /* 
		x��yΪNaNʱ������SNaN����
 	*/
	if(ix > 0x7ff00000 || ((ix==0x7ff00000)&&(lx!=0)) ||
	   iy > 0x7ff00000 || ((iy==0x7ff00000)&&(ly!=0))) 
	{
		double result = (x+0.0)+(y+0.0);
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}
    /* 
		��y == 0ʱ��ֱ�ӷ���1.0
	*/
	if((iy|ly)==0) return one;

    /*
		��x==1.0ʱ������1.0 
	*/
	if (hx==0x3ff00000 && lx == 0) return one;


	/*
		���÷������ڱ���xΪ����ʱy�����
		yisint = 0 ��ʾy��������
		yisint = 1 ��ʾyΪ����
		yisint = 2 ��ʾyΪż��
	*/
	yisint  = 0;
	if(hx<0) {	
		/*
			yΪż��ʱ���÷���Ϊ2
		*/
	    if(iy>=0x43400000) yisint = 2; /* even integer y */
	    else if(iy>=0x3ff00000) {
			k = (iy>>20)-0x3ff;	   /* exponent */
			if(k>20) {
				j = ly>>(52-k);
				if((j<<(52-k))==ly) yisint = 2-(j&1);	// yΪ����ʱ���÷���
			} else if(ly==0) {
				j = iy>>(20-k);
				if((j<<(20-k))==iy) yisint = 2-(j&1);	// yΪ����ʱ���÷���
			}
	    }		
	} 

    /* special value of y */
	if(ly==0) { 	
	    if (iy==0x7ff00000) {	/* y is +-inf */
			/*
				����y��Inf�����
			*/
	        if(((ix-0x3ff00000)|lx)==0)
			{
				/*
					x��-1ʱ��-1 ^ +-Inf ΪNaN
				*/
				double result = 1.0;
				int32_t high_word = 0x7FF40000;
				int32_t low_word = 0x00000000;
				SET_HIGH_WORD(result, high_word);
				SET_LOW_WORD(result, low_word);
				return result;
			}	/* (-1)**+-inf is NaN */
	        else if (ix >= 0x3ff00000)/* (|x|>1)**+-inf = inf,0 */
				/*
					|x| > 1 ʱ���������η�����������󣬸������η�����0
				*/
		    	return (hy>=0)? y: zero;
	        else			/* (|x|<1)**-,+inf = inf,0 */
				/*
					|x| < 1 ʱ���������η�����0���������η�����Inf
				*/
		    	return (hy<0)?-y: zero;
	    } 
	    if(iy==0x3ff00000) {	/* y is  +-1 */
			/*
				y = -1 ʱ������ 1/x
			*/
			if(hy<0) return one/x; 
			/*
				y = 1 ʱ������x����
			*/
			else return x;
	    }
		/*
			������y = 2ʱ������x * x
		*/
            if(hy==0x40000000) return x*x;   /* y is  2 */
		/*
			������y = 3ʱ������x * x * x
		*/
            if(hy==0x40080000) return x*x*x; /* y is  3 */
		/*
			������y = 4ʱ������x * x * x * x
		*/
            if(hy==0x40100000) {             /* y is  4 */
                u = x*x;
                return u*u;
            }
		/*
			������y = 0.5��x>=0ʱ������ƽ����
		*/
	    if(hy==0x3fe00000) {             /* y is  0.5 */
			if(hx>=0)	/* x >= +0 */
            return sqrt_my(x);
	    }
	}

	ax   = fabs(x);
    /* special value of x */
	if(lx==0) {
	    if(ix==0x7ff00000||ix==0||ix==0x3ff00000){
			z = ax;			/*x is +-0,+-inf,+-1*/
			/*
				+-0��+-1��+-Inf�ĸ����η����ؾ���ֵ���෴����Ϊ��ֵ
			*/
			if(hy<0) z = one/z;	/* z = (1/|x|) */
			if(hx<0) {
				if((((ix-0x3ff00000)|yisint)==0) ||(((ix-0x7ff00000)|yisint)==0)) {
					/*
						-1�͸�����ķ������η�����NaN
					*/
					double result = x + x;
					int32_t high_word = 0x7FF40000;
					int32_t low_word = 0x00000000;
					SET_HIGH_WORD(result, high_word);
					SET_LOW_WORD(result, low_word);
					return result;
				} else if(yisint==1) 
					/*
						����y����ż�������ؽ������
					*/
				z = -z;		/* (x<0)**odd = -(|x|**odd) */
			}
			return z;
	    }
	}
    
    /* CYGNUS LOCAL + fdlibm-5.3 fix: This used to be
	n = (hx>>31)+1;
       but ANSI C says a right shift of a signed negative quantity is
       implementation defined.  */
	n = ((u_int32_t)hx>>31)-1;

    /*
		x < 0 �ķ������η�����NaN 
	*/
	if((n|yisint)==0) 
	{
		double result = x + x;
		int32_t high_word = 0x7FF40000;
		int32_t low_word = 0x00000000;
		SET_HIGH_WORD(result, high_word);
		SET_LOW_WORD(result, low_word);
		return result;
	}

	s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
	if((n|(yisint-1))==0) s = -one;/* (-ve)**(odd int) */

    /* |y| is huge */
	if(iy>0x41e00000) { /* if |y| > 2**31 */
	    if(iy>0x43f00000){	/* if |y| > 2**64, must o/uflow */
			/*
				�������η��������硢�������η���������
			*/
			if(ix<=0x3fefffff) return (hy<0)? huge*huge:tiny*tiny;
			/*
				�������η��������硢�������η���������
			*/
			if(ix>=0x3ff00000) return (hy>0)? huge*huge:tiny*tiny;
	    }
	/* over/underflow if x is not close to one */
		/*
			�������η��������硢�������η���������
		*/
	    if(ix<0x3fefffff) return (hy<0)? s*huge*huge:s*tiny*tiny;
		/*
			�������η��������硢�������η���������
		*/
	    if(ix>0x3ff00000) return (hy>0)? s*huge*huge:s*tiny*tiny;
	/* |1-x|  <= 2**-20, ͨ�� x-x^2/2+x^3/3-x^4/4 ����log(x)  */
	    t = ax-one;		/* t has 20 trailing zeros */
	    w = (t*t)*(0.5-t*(0.3333333333333333333333-t*0.25));
	    u = ivln2_h*t;	/* ivln2_h has 21 sig. bits */
	    v = (t*ivln2_l)-(w*ivln2);
	    t1 = u+v;
	    SET_LOW_WORD(t1,0);
	    t2 = v-(t1-u);
	} else {
	    double ss,s2,s_h,s_l,t_h,t_l;
	    n = 0;
	    if(ix<0x00100000)
		{
			/* ����x�Ƿǹ��������� */
			ax *= two53;	//	���㸡������񻯺��Ӧ��β��
			n -= 53;	// ��������񻯺��Ӧ��ָ��
			GET_HIGH_WORD(ix,ax); 
		}
	    n  += ((ix)>>20)-0x3ff;
	    j  = ix&0x000fffff;
	/* determine interval */
	    ix = j|0x3ff00000;		/* normalize ix */
	    if(j<=0x3988E) k=0;		/* |x|<sqrt_my(3/2) */
	    else if(j<0xBB67A) k=1;	/* |x|<sqrt_my(3)   */
	    else {
			k=0;
			n+=1;
			ix -= 0x00100000;
		}
	    SET_HIGH_WORD(ax,ix);

	/* compute ss = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
	    u = ax-bp[k];		/* bp[0]=1.0, bp[1]=1.5 */
	    v = one/(ax+bp[k]);
	    ss = u*v;
	    s_h = ss;
	    SET_LOW_WORD(s_h,0);
	/* 
		ʹ������С�ķ��������λs_l
		t_h=ax+bp[k] High 
	*/
	    t_h = zero;
	    SET_HIGH_WORD(t_h,((ix>>1)|0x20000000)+0x00080000+(k<<18));
	    t_l = ax - (t_h-bp[k]);
	    s_l = v*((u-s_h*t_h)-s_h*t_l);
	/* ʹ�ö���ʽչ������ log(ax) */
	    s2 = ss*ss;
	    r = s2*s2*(L1+s2*(L2+s2*(L3+s2*(L4+s2*(L5+s2*L6)))));
	    r += s_l*(s_h+ss);
	    s2  = s_h*s_h;
	    t_h = 3.0+s2+r;
	    SET_LOW_WORD(t_h,0);
	    t_l = r-((t_h-3.0)-s2);
	/* u+v = ss*(1+...) */
	    u = s_h*t_h;
	    v = (s_l*t_h)+(t_l*ss);
	/* 2/(3log2)*(ss+...) */
	    p_h = u+v;	// 3*log(ax) / 2
	    SET_LOW_WORD(p_h,0);
	    p_l = v-(p_h-u);
		/* ʹ�ó���2 / 3*log2������õ� log2(ax) */
	    z_h = cp_h*p_h;		/* cp_h+cp_l = 2/(3*log2) */
	    z_l = cp_l*p_h+p_l*cp+dp_l[k];
	/* log2(ax) = (ss+..)*2/(3*log2) = n + dp_h + z_h + z_l */
	    t = (double)n;
	    t1 = (((z_h+z_l)+dp_h[k])+t);
	    SET_LOW_WORD(t1,0);
	    t2 = z_l-(((t1-t)-dp_h[k])-z_h);
	}

    /* 	Ϊ����߾��ȣ��� y �ֽ�Ϊ��λy1�������y2���� log2(x)�ֽ�Ϊ��λt1�������t2��
		ͨ������(y1 + y2) * (t1 + t2)���õ����ս��
		split up y into y1+y2 and compute (y1+y2)*(t1+t2) 
	*/
    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
	y1  = y;
	SET_LOW_WORD(y1,0);
	p_l = (y-y1)*t1+y*t2;
	p_h = y1*t1;
	z = p_l+p_h;
	EXTRACT_WORDS(j,i,z);
	/*
		��2��ָ�����ڵ���1024��С�ڵ���-1075,ֱ�ӿ��ܲ������������
	 */
	if (j>=0x40900000) {				/* z >= 1024 */
	    if(((j-0x40900000)|i)!=0)			/* if z > 1024 */
			return s*huge*huge;			/* overflow */
	    else {
			if(p_l+ovt>z-p_h) return s*huge*huge;	/* overflow */
	    }
	} else if((j&0x7fffffff)>=0x4090cc00 ) {	/* z <= -1075 */
	    if(((j-0xc090cc00)|i)!=0) 		/* z < -1075 */
			return s*tiny*tiny;		/* underflow */
	    else {
			if(p_l<=z-p_h) return s*tiny*tiny;	/* underflow */
	    }
	}
    /*
     * ���ö���ʽ�ƽ��������շ��ؽ�� 2**(p_h+p_l)
     */
	i = j&0x7fffffff;
	k = (i>>20)-0x3ff;
	n = 0;
	if(i>0x3fe00000) {		/* if |z| > 0.5, set n = [z+0.5] */
	    n = j+(0x00100000>>(k+1));
	    k = ((n&0x7fffffff)>>20)-0x3ff;	/* new k for n */
	    t = zero;
	    SET_HIGH_WORD(t,n&~(0x000fffff>>k));
	    n = ((n&0x000fffff)|0x00100000)>>(20-k);
	    if(j<0) n = -n;
	    p_h -= t;
	} 
	t = p_l+p_h;
	SET_LOW_WORD(t,0);
	u = t*lg2_h;
	v = (p_l-(t-p_h))*lg2+t*lg2_l;
	z = u+v;
	w = v-(z-u);
	t  = z*z;
	t1  = z - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	r  = (z*t1)/(t1-two)-(w+z*w);
	z  = one-(r-z);
	GET_HIGH_WORD(j,z);
	j += (n<<20);
	if((j>>20)<=0) z = ldexp(z,n);	/* ���ڷǹ��������������Ϊ�ǹ��������ʽ */
	else SET_HIGH_WORD(z,j);
	return s*z;
}
