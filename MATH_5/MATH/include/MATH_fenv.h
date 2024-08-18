#if defined(__aarch64__) || defined(__arm__)
#include <MATH_fenv_arm.h>
#elif defined(__x86_64__)
#include <MATH_fenv_amd64.h>
#elif defined(__i386__)
#include <MATH_fenv_i387.h>
#elif defined(__powerpc__) || defined(__ppc__)
#include <MATH_fenv_powerpc.h>
#elif defined(__mips__)
#include <MATH_fenv_mips.h>
#elif defined(__s390__)
#include <MATH_fenv_s390.h>
#elif defined(__riscv)
#include <MATH_fenv_riscv.h>
#elif defined(__loongarch64)
#include <MATH_fenv_loongarch64.h>
#elif defined(__ft2000__)
#include <MATH_fenv_FT2000.h>
#else
#error "Unsupported platform"
#endif
