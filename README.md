# Raytracing
## Description
Raytracing project based off the [Raytracing in One Weekend](https://raytracing.github.io/) book series.

## Building and Running
When in the project directory the project can be built with cmake.

    $ cmake -B build
Must be run when changes to the CMakeLists.txt are made.

    $ cmake --build build
Will build the project, under build/Debug

    $ build/Debug/raytracing > outputs/image.ppm
Will run the program, outputting to outputs/image.ppm