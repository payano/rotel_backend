cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -pedantic -O2 -Werror -Wextra -fexceptions")

project(rotel)


############################################################
# Create a library
############################################################

#Generate the static library from the library sources
add_library(rotel SHARED
    src/RotelA14.cpp
    src/RotelBase.cpp
    src/JNICalls.cpp
)


############################################################
# Include directories 
############################################################
include_directories(
    include
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
#add_executable(testApp
#    src/main.cpp
#)
