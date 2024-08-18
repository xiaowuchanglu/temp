cmake_minimum_required(VERSION 3.25)

project(MATH
        VERSION 0.8.0
        LANGUAGES C ASM)

add_library("${PROJECT_NAME}" SHARED)

SET(CMAKE_BUILD_TYPE "Debug")


# Find the relevant folder depending on the architecture
set(MATH_ARCH_FOLDER ${CMAKE_SYSTEM_PROCESSOR})
string(TOLOWER "${MATH_ARCH_FOLDER}" MATH_ARCH_FOLDER)

if(${MATH_ARCH_FOLDER} STREQUAL "amd64" OR ${MATH_ARCH_FOLDER} STREQUAL "x86_64")
    set(MATH_ARCH_FOLDER "amd64")
elseif(${MATH_ARCH_FOLDER} MATCHES "arm64")
    set(MATH_ARCH_FOLDER "aarch64")
elseif(${MATH_ARCH_FOLDER} STREQUAL "x86")
    set(MATH_ARCH_FOLDER "i387")
elseif(${MATH_ARCH_FOLDER} STREQUAL "powerpc")
    set(MATH_ARCH_FOLDER "powerpc")
else()
    message(FATAL_ERROR "${PROJECT_NAME} not set up for detected architecture: ${MATH_ARCH_FOLDER}")
endif()


# Compile flags
list(APPEND C_ASM_COMPILE_FLAGS "-ffp-contract=off" "-fno-fast-math" "-fno-rounding-math" "-fno-math-errno")
list(APPEND C_ASM_COMPILE_FLAGS "-fPIC" "-std=c99" "-fno-builtin")
list(APPEND C_ASM_COMPILE_FLAGS "-Wall" "-Wno-implicit-function-declaration")
list(APPEND C_ASM_COMPILE_FLAGS "-DASSEMBLER" "-D__BSD_VISIBLE" "-O0" "-fprofile-arcs" "-ftest-coverage" )

# Compiler-specific compile flags
if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
    list(APPEND C_ASM_COMPILE_FLAGS "-fno-strict-aliasing" "-ffp-exception-behavior=strict")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    list(APPEND C_ASM_COMPILE_FLAGS "-fno-gnu89-inline")
else()
    message(FATAL_ERROR "${PROJECT_NAME} not set up to be compiled with ${CMAKE_C_COMPILER_ID}")
endif()

# Architecture-specific compile flags - take advantage of sse on x86
if(${MATH_ARCH_FOLDER} STREQUAL "i387")
    list(APPEND C_ASM_COMPILE_FLAGS "-march=i686" "-m32" "-msse2" "-mfpmath=sse")
elseif(${MATH_ARCH_FOLDER} STREQUAL "amd64")
    list(APPEND C_ASM_COMPILE_FLAGS "-m64" "-msse2" "-mfpmath=sse")
endif()

# Suppress warnings if requested
if(MATH_SUPPRESS_WARNINGS)
    list(APPEND C_ASM_COMPILE_FLAGS "-w")
endif()

list(APPEND C_ASM_COMPILE_FLAGS
        "-I${PROJECT_SRC}"
        "-I${PROJECT_SRC}/include"
        "-I${PROJECT_SRC}/${MATH_ARCH_FOLDER}"
        "-I${PROJECT_SRC}/src"
)

# Add compile flags
target_compile_options("${PROJECT_NAME}" PUBLIC ${C_ASM_COMPILE_FLAGS})

list(APPEND C_ASM_LINK_FLAGS "-lgcov")
# Determine if long double and double is the same on current platform
if(${MATH_ARCH_FOLDER} STREQUAL "i387" OR ${MATH_ARCH_FOLDER} STREQUAL "amd64")
    set(LONG_DOUBLE_NOT_DOUBLE 1)
elseif(${MATH_ARCH_FOLDER} STREQUAL "aarch64")
    if(NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
        set(LONG_DOUBLE_NOT_DOUBLE 1)
    endif()
endif()

# Project Source
set(PROJECT_SRC "${PROJECT_SOURCE_DIR}")

# Common
list(APPEND MATH_C_SOURCE
        # src
        "${PROJECT_SRC}/src/common.c"
        "${PROJECT_SRC}/src/e_fmod.c"
        "${PROJECT_SRC}/src/e_remainder.c"
        "${PROJECT_SRC}/src/e_rem_pio2.c"
        "${PROJECT_SRC}/src/k_cos.c"
        "${PROJECT_SRC}/src/k_rem_pio2.c"
        "${PROJECT_SRC}/src/k_sin.c"
        "${PROJECT_SRC}/src/k_tan.c"
        "${PROJECT_SRC}/src/s_copysign.c"
        "${PROJECT_SRC}/src/s_fdim.c"
        "${PROJECT_SRC}/src/s_floor.c"
        "${PROJECT_SRC}/src/s_fmax.c"
        "${PROJECT_SRC}/src/s_fmin.c"
        "${PROJECT_SRC}/src/s_ilogb.c"
        "${PROJECT_SRC}/src/s_fpclassify.c"
        "${PROJECT_SRC}/src/s_isinf.c"
        "${PROJECT_SRC}/src/s_isfinite.c"
        "${PROJECT_SRC}/src/s_isnormal.c"
        "${PROJECT_SRC}/src/s_isnan.c"
        "${PROJECT_SRC}/src/s_nextafter.c"
        "${PROJECT_SRC}/src/s_nexttoward.c"
        "${PROJECT_SRC}/src/s_remquo.c"
        "${PROJECT_SRC}/src/s_rint.c"
        "${PROJECT_SRC}/src/s_round.c"
        "${PROJECT_SRC}/src/s_signbit.c"
        "${PROJECT_SRC}/src/s_sin.c"
        "${PROJECT_SRC}/src/s_trunc.c"
        "${PROJECT_SRC}/src/s_lrint.c"
        "${PROJECT_SRC}/src/s_lround.c"
        "${PROJECT_SRC}/src/s_llrint.c"
        "${PROJECT_SRC}/src/s_llround.c"
        "${PROJECT_SRC}/src/s_nearbyint.c"
        "${PROJECT_SRC}/src/s_fabs.c"
        "${PROJECT_SRC}/src/s_cos.c"
        "${PROJECT_SRC}/src/s_tan.c"
        "${PROJECT_SRC}/src/e_asin.c"
        "${PROJECT_SRC}/src/e_acos.c"
        "${PROJECT_SRC}/src/s_atan.c"
        "${PROJECT_SRC}/src/e_sqrt.c"
        "${PROJECT_SRC}/src/isNormal.c"
        "${PROJECT_SRC}/src/e_atan2.c"
        "${PROJECT_SRC}/src/e_sinh.c"
        "${PROJECT_SRC}/src/e_cosh.c"
        "${PROJECT_SRC}/src/s_tanh.c"
        "${PROJECT_SRC}/src/s_expm1.c"
        "${PROJECT_SRC}/src/e_exp.c"
        "${PROJECT_SRC}/src/s_frexp.c"
        "${PROJECT_SRC}/src/s_ldexp.c"
        "${PROJECT_SRC}/src/k_exp.c"
        "${PROJECT_SRC}/src/e_log.c"
        "${PROJECT_SRC}/src/e_log10.c"
        "${PROJECT_SRC}/src/k_log.h"
        "${PROJECT_SRC}/src/s_floor.c"
        "${PROJECT_SRC}/src/s_ceil.c"
        "${PROJECT_SRC}/src/s_modf.c"
        "${PROJECT_SRC}/src/e_pow.c"


)

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    list(APPEND MATH_C_SOURCE
            "${PROJECT_SRC}/src/s_nan.c"
    )
endif()


 

# Architecture-specific sources
if (${MATH_ARCH_FOLDER} STREQUAL "amd64")
    list(APPEND MATH_C_SOURCE
        "${PROJECT_SRC}/amd64/fenv.c"
    )

elseif(${MATH_ARCH_FOLDER} STREQUAL "aarch64")
    list(APPEND MATH_C_SOURCE
            "${PROJECT_SRC}/aarch64/fenv.c"
    )

elseif(${MATH_ARCH_FOLDER} STREQUAL "i387")
    list(APPEND MATH_C_SOURCE
            "${PROJECT_SRC}/i387/fenv.c"
    )


    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        list(APPEND MATH_ASM_SOURCE
                "${PROJECT_SRC}/i387/s_scalbn.S"
        )
    endif()

elseif(${MATH_ARCH_FOLDER} STREQUAL "powerpc")
        list(APPEND MATH_C_SOURCE
                "${PROJECT_SRC}/powerpc/fenv.c"
        )
else()
	message(FATAL_ERROR "${PROJECT_NAME} CMake build is not set up for ${MATH_ARCH_FOLDER}")
endif()


# Filter out C implementation from compilation list if a native implementation exists
foreach(FILE_TO_REMOVE ${MATH_ASM_SOURCE})
    # Get filename and strip out extension
    cmake_path(GET FILE_TO_REMOVE FILENAME FILENAME_TO_REMOVE)
    cmake_path(REMOVE_EXTENSION FILENAME_TO_REMOVE OUTPUT_VARIABLE FILENAME_TO_REMOVE)
    message(DEBUG "Filename to remove: ${FILENAME_TO_REMOVE}")

    # Go through files and remove one with the same name
    foreach(CUR_FILE ${MATH_C_SOURCE})
        cmake_path(GET CUR_FILE FILENAME CUR_FILENAME)
        cmake_path(REMOVE_EXTENSION CUR_FILENAME OUTPUT_VARIABLE CUR_FILENAME)

        if(${CUR_FILENAME} STREQUAL ${FILENAME_TO_REMOVE})
            list(REMOVE_ITEM MATH_C_SOURCE ${CUR_FILE})
            message(DEBUG "Removed source file from compilation list: ${CUR_FILE}")
            break()
        endif()
    endforeach()
endforeach()


# Add sources
target_sources("${PROJECT_NAME}" PRIVATE ${MATH_C_SOURCE}
                                         ${MATH_ASM_SOURCE}
)


# Include directories
list(APPEND MATH_INCLUDE_DIRS
        "${PROJECT_SRC}"
        "${PROJECT_SRC}/include"
        "${PROJECT_SRC}/${MATH_ARCH_FOLDER}"
        "${PROJECT_SRC}/src"
)

target_include_directories("${PROJECT_NAME}" PUBLIC ${MATH_INCLUDE_DIRS})
# Add link flags
target_link_libraries(
    MATH
    gcov
)
