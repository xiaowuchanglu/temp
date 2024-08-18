MATH_HOME=$(abspath ..)
include ../Make.inc

# Set rpath of tests to builddir for loading shared library
MATH_LIB = -L.. -lMATH
ifneq ($(OS),WINNT)
ifneq ($(OS),Darwin)
MATH_LIB += -Wl,-rpath=$(MATH_HOME)
endif
else # WINNT
CFLAGS_add += -DIMPORT_EXPORTS
endif


all: test-double bench-MATH bench-syslibm

bench: bench-syslibm bench-MATH

test-double: test-double.c libm-test-ulps.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CFLAGS_add) $(LDFLAGS) $(LDFLAGS_arch) $@.c -D__BSD_VISIBLE -I ../include -I../src $(MATH_LIB) -o $@

bench-MATH: libm-bench.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CFLAGS_add) $(LDFLAGS) $(LDFLAGS_arch) $< $(MATH_LIB) -I ../include -I../src $(MATH_LIB) -o $@

bench-syslibm: libm-bench.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CFLAGS_add) $(LDFLAGS) $(LDFLAGS_arch) $< -lm -I ../include -I../src $(MATH_LIB) -o $@

clean:
	rm -fr test-double bench-MATH bench-syslibm *.dSYM
