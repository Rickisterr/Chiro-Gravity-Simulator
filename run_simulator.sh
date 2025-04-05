#!/bin/bash

filename=$1_gravity_test
build=$2

if [ -e "src/$filename.cpp" ]
then
    case $build in
    "build")
        if [ ! -e "build" ]
        then
            mkdir "build"
        fi
        g++ src/$filename.cpp src/Models.cpp src/SpaceTimeFabric.cpp /usr/src/glad.c -o build/$filename.exe -I /usr/include -L /usr/lib -lGLEW -lglfw -lGL
        ;;

    "run")
        ./build/$filename.exe
        ;;

    *)
        echo "ValueError: Second argument should only be 'build' or 'run'"
        ;;
    esac
else
    echo "NameError: File does not exist (possible values of first argument are '2D' or '3D')"
fi
