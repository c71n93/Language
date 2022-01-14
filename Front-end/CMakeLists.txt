cmake_minimum_required(VERSION 3.21)
project(Front_end)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(Front_end
        include/Language.h
        include/Language_DSL.h
        src/Language_Errors_Func.cpp
        src/Language_Main.cpp
        src/Language_Tree_Dump.cpp
        src/Language_Tree_Func.cpp
        src/Language_File&Text_Func.cpp
        src/Language_Lexical_Analysis_Func.cpp)
