#1. Declare minimum CMake version required to build this project
cmake_minimum_required(VERSION 3.20.0)
#2. Locate Zephyr build system as a CMake module
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#3. Name your project
project(blinky)
#4. Tell Zephyr which source files to use
target_sources(app PRIVATE src/main.c)