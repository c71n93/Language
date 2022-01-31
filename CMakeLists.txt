cmake_minimum_required(VERSION 2.4)
project(Language)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(Language
        include/Language.h
        include/Language_DSL.h
        include/Processor_include/Proc_General.h
        include/Processor_include/Processor.h
        include/Processor_include/Assembler.h
        src/Language_Errors_Func.cpp
        src/Language_Main.cpp
        src/Language_Tree_Dump.cpp
        src/Language_Tree_Func.cpp
        src/Language_File&Text_Func.cpp
        src/Language_Lexical_Analysis_Func.cpp
        src/Language_Back-end.cpp)
