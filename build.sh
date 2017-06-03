#!/bin/sh
g++ -g -Wall ./src/**/*.cpp \
	-I/usr/include/python3.6/ \
	-I/usr/include/GL/ \
	-I/home/campbell/Projects/Libraries/soil/src/ \
	-I/home/campbell/Projects/Libraries/shapelib-1.4.0/ \
	-I/home/campbell/Projects/Libraries/imgui-1.50/ \
	-I./include \
	-o ./build/PyEngine
