/* Do not redefine macros if the system provides them in sys/cdefs.h.
 * The two macros correspond to different platforms. */
#ifndef _BSD_CDEFS_H_
#define _BSD_CDEFS_H_

/*
 * This code has been put in place to help reduce the addition of
 * compiler specific defines in FreeBSD code.  It helps to aid in
 * having a compiler-agnostic source tree.
 */

#if defined(__GNUC__) || defined(__INTEL_COMPILER)

#if __GNUC__ >= 3 || defined(__INTEL_COMPILER)
#define __GNUCLIKE_ASM 3
#else
#define __GNUCLIKE_ASM 2
#endif

#define __CC_SUPPORTS___INLINE__ 1

#endif /* __GNUC__ || __INTEL_COMPILER */

#if defined(__STDC__) || defined(__cplusplus)

#define	__volatile	volatile
#if defined(__cplusplus)
#define	__inline	inline		/* convert to C++ keyword */
#else
#if !defined(__CC_SUPPORTS___INLINE)
#define	__inline			/* delete GCC keyword */
#endif /* ! __CC_SUPPORTS___INLINE */
#endif /* !__cplusplus */

#else	/* !(__STDC__ || __cplusplus) */

#if !defined(__CC_SUPPORTS___INLINE)
#define	__inline
#define	__volatile
#endif	/* !__CC_SUPPORTS___INLINE */
#endif	/* !(__STDC__ || __cplusplus) */

/*
 * Macro to test if we're using a specific version of gcc or later.
 */
#ifndef __GNUC_PREREQ__
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#define	__GNUC_PREREQ__(ma, mi)	\
	(__GNUC__ > (ma) || __GNUC__ == (ma) && __GNUC_MINOR__ >= (mi))
#else
#define	__GNUC_PREREQ__(ma, mi)	0
#endif
#endif /* __GNUC_PREREQ__ */

/*
 * Compiler-dependent macro to help declare pure (no side effects) functions.
 * It is null except for versions of gcc that are known to support the features
 * properly (old versions of gcc-2 supported the dead and pure features
 * in a different (wrong) way), and for icc.  If we do not provide an implementation
 * for a given compiler, let the compile fail if it is told to use
 * a feature that we cannot live without.
 */
#if !defined(__pure2) && (__GNUC_PREREQ__(2, 7) || defined(__INTEL_COMPILER))
#define	__pure2		__attribute__((__const__))
#endif

#endif /* !_BSD_CDEFS_H_ */
