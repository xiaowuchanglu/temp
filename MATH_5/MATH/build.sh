#!/bin/bash
if [ ! -d "CMakeFiles" ]; then
	echo "delete ..."
else
	rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake libMATH.so Makefile
fi
cmake .
make

