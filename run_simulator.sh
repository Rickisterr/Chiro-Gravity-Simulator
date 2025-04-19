#!/bin/bash

# filename=$1_gravity_test
filename=3D_gravity_sim
build=$1

if [ -e "src/$filename.cpp" ]
then
    case $build in
    "build")
        if [ ! -e "build" ]
        then
            mkdir "build"
        fi
        g++ src/$filename.cpp src/Models.cpp src/SpaceTimeFabric.cpp src/glad.c -o build/$filename.exe -I include -L lib -lglew32 -lglfw3 -lopengl32 -lgdi32
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
