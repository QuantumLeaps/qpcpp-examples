# DPP example for QP/C++ on the host: Windows (win32 port) or POSIX (posix port)
cmake_minimum_required(VERSION 3.25)

# check the setup before project(): one clear error, no follow-ups ........
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/qpcpp.hpp)
    message(FATAL_ERROR "QP/C++ not found. Keep this example inside qpcpp/examples.")
endif()
if(CMAKE_GENERATOR MATCHES "Ninja")
    find_program(CMAKE_MAKE_PROGRAM ninja)
    if(NOT CMAKE_MAKE_PROGRAM)
        message(FATAL_ERROR "Ninja not found on the PATH (https://ninja-build.org)")
    endif()
endif()
if(NOT CMAKE_CXX_COMPILER)           # without the preset: also use g++
    set(CMAKE_CXX_COMPILER g++)
endif()
find_program(HOST_CXX NAMES ${CMAKE_CXX_COMPILER})
if(NOT HOST_CXX)
    message(FATAL_ERROR "C++ compiler '${CMAKE_CXX_COMPILER}' not found on the PATH.\n"
        "Windows (QP-bundle): set PATH=%QTOOLS%\\mingw32\\bin;%PATH%\n"
        "Linux (Debian/Ubuntu): sudo apt install build-essential")
endif()

project(dpp LANGUAGES CXX)     # the name of the program

# compiler settings (the same as in the Makefile) ...........................
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)        # -std=c++17, not -std=gnu++17
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wsign-conversion
        -Wold-style-cast -fno-rtti -fno-exceptions)
    set(CMAKE_CXX_FLAGS_DEBUG   "-g -O")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_SPY     "-g -O")
endif()

# QP/C++ framework ..........................................................
add_library(qpcpp_config INTERFACE)  # qp_config.hpp is in this directory
target_include_directories(qpcpp_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    set(QPCPP_PORT win32)
else()
    set(QPCPP_PORT posix)
endif()
add_subdirectory(../../.. qpcpp)

# the application ...........................................................
add_executable(${PROJECT_NAME}
    main.cpp
    philo.cpp
    table.cpp
    bsp.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE qpcpp::qpcpp)
