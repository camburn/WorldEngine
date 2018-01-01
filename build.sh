#!/bin/sh
echo "Building PyEngine"
g++ -g -Wall ./src/*.cpp ./src/**/*.cpp ~/Projects/Libraries/poly2tri/poly2tri/**/*.cc\
    -I/usr/include/python3.6/ \
    -I/usr/include/GL/ \
    -I/usr/local/include/GLFW/ \
    -I/home/campbell/Projects/Libraries/soil/src/ \
    -I/home/campbell/Projects/Libraries/shapelib-1.4.0/ \
    -I/home/campbell/Projects/Libraries/poly2tri/ \
    -I/home/campbell/Projects/Libraries/imgui-1.50/ \
    -I/home/campbell/Projects/WorldEngine/include/ \
    -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions \
    -lm -lGL -lGLU -lGLEW -lXi -lXmu \
    -l glfw \
    -l SOIL \
    -l shp \
    -l assimp \
    -o ./build/WorldEngine
if [ $? -ne 0 ]; then
    echo "Build Failed"
else
    echo "Build Completed"
fi
