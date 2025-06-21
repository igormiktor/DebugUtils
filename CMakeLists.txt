cmake_minimum_required( VERSION 3.10.0 )
project( DebugUtilsTest VERSION 1.0.0 LANGUAGES C CXX )

set( CMAKE_C_STANDARD 11 )
set( CMAKE_CXX_STANDARD 23 )

add_executable( DUTest main.cpp )
target_compile_definitions( DUTest PRIVATE -DDEBUGUTILS_ON=1 )


