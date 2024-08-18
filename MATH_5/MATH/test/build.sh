#!/bin/bash
if [ ! -d "CMakeFiles" ]; then
	echo "delete ..."
else
	rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
fi
cmake .
make

