#!/bin/bash

filename=$1_gravity_test
build=$2

if [ -e "$filename.cpp" ]
then
    case $build in
    "build")
        g++ $filename.cpp /usr/src/glad.c -o $filename.exe -I /usr/include -L /usr/lib -lGLEW -lglfw -lGL
        ;;

    "run")
        $2
        ./$filename.exe
        ;;

    *)
        echo "ValueError: Second argument should only be 'build' or 'run'"
        ;;
    esac
else
    echo "NameError: File does not exist (possible values of first argument are '2D' or '3D')"
fi
