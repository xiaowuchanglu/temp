//VBS
//#include <sys/endian.h>
#include <ctype.h>
#include <float.h>
#include <MATH_math.h>
#include <stdint.h>
#include <string.h> //for memset

#include "math_private.h"

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__DragonFly__)
static __inline int digittoint(int c) {
	if ('0' <= c && c <= '9')
		return (c - '0');
	else if ('A' <= c && c <= 'F')
		return (c - 'A' + 10);
	else if ('a' <= c && c <= 'f')
		return (c - 'a' + 10);
	return 0;
}
#endif


/*
 * Scan a string of hexadecimal digits (the format nan(3) expects) and
 * make a bit array (using the local endianness). We stop when we
 * encounter an invalid character, NUL, etc.  If we overflow, we do
 * the same as gcc's __builtin_nan(), namely, discard the high order bits.
 *
 * The format this routine accepts needs to be compatible with what is used
 * in contrib/gdtoa/hexnan.c (for strtod/scanf) and what is used in
 * __builtin_nan(). In fact, we're only 100% compatible for strings we
 * consider valid, so we might be violating the C standard. But it's
 * impossible to use nan(3) portably anyway, so this seems good enough.
 */
OLM_DLLEXPORT void
__scan_nan(u_int32_t *words, int num_words, const char *s)
{
	int si;		/* index into s */
	int bitpos;	/* index into words (in bits) */

	memset(words, 0, num_words * sizeof(u_int32_t));

	/* Allow a leading '0x'. (It's expected, but redundant.) */
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;

	/* Scan forwards in the string, looking for the end of the sequence. */
	for (si = 0; isxdigit(s[si]); si++)
		;

	/* Scan backwards, filling in the bits in words[] as we go. */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	for (bitpos = 0; bitpos < 32 * num_words; bitpos += 4) {
#else
	for (bitpos = 32 * num_words - 4; bitpos >= 0; bitpos -= 4) {
#endif
		if (--si < 0)
			break;
		words[bitpos / 32] |= digittoint(s[si]) << (bitpos % 32);
	}
}

OLM_DLLEXPORT double
nan(const char *s)
{
	union {
		double d;
		u_int32_t bits[2];
	} u;

	__scan_nan(u.bits, 2, s);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	u.bits[1] |= 0x7ff80000;
#else
	u.bits[0] |= 0x7ff80000;
#endif
	return (u.d);
}
