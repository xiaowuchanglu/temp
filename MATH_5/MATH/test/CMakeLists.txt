cmake_minimum_required(VERSION 3.25)

project(MATH_TEST
        VERSION 0.8.0
        LANGUAGES C ASM)

SET(CMAKE_BUILD_TYPE "Debug")
SET(PROJECT_LIB_DIR "/home/xiaobai/MATH_5/MATH/")
SET(PROJECT_MATH_DIR "/home/xiaobai/MATH_5/MATH/")
# 生成符号表调试
add_definitions("-Wall -g")

link_directories (${PROJECT_LIB_DIR}/)
add_executable(${PROJECT_NAME}   test-double.c)


include_directories( ${PROJECT_LIB_DIR}/ ${PROJECT_LIB_DIR}/include/ ${PROJECT_LIB_DIR}/src/ ${PROJECT_LIB_DIR}/test/)
target_link_libraries (${PROJECT_NAME}  ${PROJECT_LIB_DIR}/libMATH.so)
