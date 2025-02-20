#ifndef _CDEFS_COMPAT_H_
#define	_CDEFS_COMPAT_H_

#if !defined(__BEGIN_DECLS)
#if defined(__cplusplus)
#define	__BEGIN_DECLS	extern "C" {
#define	__END_DECLS	}
#else
#define	__BEGIN_DECLS
#define	__END_DECLS
#endif
#endif /* !defined(__BEGIN_DECLS) */

#ifdef __GNUC__
#if defined(__strong_alias) && defined(__NetBSD__)
#define MATH_strong_reference(sym,alias) __strong_alias(alias,sym)
#elif defined(__strong_reference)
#define MATH_strong_reference(sym,alias) __strong_reference(sym,alias)
#else
#ifdef __APPLE__
#define MATH_strong_reference(sym,aliassym) MATH_weak_reference(sym,aliassym)
#else
#define MATH_strong_reference(sym,aliassym)	\
	OLM_DLLEXPORT extern __typeof (aliassym) aliassym __attribute__ ((__alias__ (#sym)));
#endif /* __APPLE__ */
#endif /* __strong_reference */

#ifdef __wasm__
#define MATH_weak_reference(sym,alias) MATH_strong_reference(sym,alias)
#elif defined(__weak_alias) && defined(__NetBSD__)
#define MATH_weak_reference(sym,alias) __weak_alias(alias,sym)
#elif defined(__weak_reference)
#define MATH_weak_reference(sym,alias) __weak_reference(sym,alias)
#else
#ifdef __ELF__
#ifdef __STDC__
#define MATH_weak_reference(sym,alias)	\
	__asm__(".weak " #alias);	\
	__asm__(".equ "  #alias ", " #sym)
#ifdef __warn_references
#define MATH_warn_references(sym,msg) __warn_references(sym,msg)
#else
#define MATH_warn_references(sym,msg)	\
	__asm__(".section .gnu.warning." #sym);	\
	__asm__(".asciz \"" msg "\"");	\
	__asm__(".previous")
#endif /* __warn_references */
#else
#define MATH_weak_reference(sym,alias)	\
	__asm__(".weak alias");		\
	__asm__(".equ alias, sym")
#ifdef __warn_references
#define MATH_warn_references(sym,msg) __warn_references(sym,msg)
#else
#define MATH_warn_references(sym,msg)	\
	__asm__(".section .gnu.warning.sym"); \
	__asm__(".asciz \"msg\"");	\
	__asm__(".previous")
#endif	/* __warn_references */
#endif	/* __STDC__ */
#elif defined(__clang__) /* CLANG */
#if defined(_WIN32) && defined(_X86_)
#define MATH_asm_symbol_prefix "_"
#else
#define MATH_asm_symbol_prefix ""
#endif
#ifdef __STDC__
#define MATH_weak_reference(sym,alias)     \
    __asm__(".weak_reference " MATH_asm_symbol_prefix #alias); \
    __asm__(".set " MATH_asm_symbol_prefix #alias ", " MATH_asm_symbol_prefix #sym)
#else
#define MATH_weak_reference(sym,alias)     \
    __asm__(".weak_reference MATH_asm_symbol_prefix/**/alias");\
    __asm__(".set MATH_asm_symbol_prefix/**/alias, MATH_asm_symbol_prefix/**/sym")
#endif
#else	/* !__ELF__ */
#ifdef __STDC__
#define MATH_weak_reference(sym,alias)	\
	__asm__(".stabs \"_" #alias "\",11,0,0,0");	\
	__asm__(".stabs \"_" #sym "\",1,0,0,0")
#ifdef __warn_references
#define MATH_warn_references(sym,msg) __warn_references(sym,msg)
#else
#define MATH_warn_references(sym,msg)	\
	__asm__(".stabs \"" msg "\",30,0,0,0");		\
	__asm__(".stabs \"_" #sym "\",1,0,0,0")
#endif /* __warn_references */
#else
#define MATH_weak_reference(sym,alias)	\
	__asm__(".stabs \"_/**/alias\",11,0,0,0");	\
	__asm__(".stabs \"_/**/sym\",1,0,0,0")
#ifdef __warn_references
#define MATH_warn_references(sym,msg) __warn_references(sym,msg)
#else
#define MATH_warn_references(sym,msg)	\
	__asm__(".stabs msg,30,0,0,0");			\
	__asm__(".stabs \"_/**/sym\",1,0,0,0")
#endif	/* __warn_references */
#endif	/* __STDC__ */
#endif	/* __ELF__ */
#endif  /* __weak_reference */
#endif	/* __GNUC__ */


#endif /* _CDEFS_COMPAT_H_ */
