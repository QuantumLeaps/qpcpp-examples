# DPP example for QP/C++ on the NUCLEO-C031C6 board (ARM Cortex-M0+)
cmake_minimum_required(VERSION 3.25)
set(QPCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# check the setup before project(): one clear error, no follow-ups ........
if(NOT EXISTS ${QPCPP_DIR}/include/qpcpp.hpp)
    message(FATAL_ERROR "QP/C++ not found. Keep this example inside qpcpp/examples.")
endif()
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "No toolchain file. Use a preset: cmake --list-presets")
elseif(IS_ABSOLUTE "${CMAKE_TOOLCHAIN_FILE}" AND NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    message(FATAL_ERROR "Toolchain file not found (qpcpp/3rd_party missing?).\n"
        "In the qpcpp folder run: git submodule update --init")
endif()
if(CMAKE_GENERATOR MATCHES "Ninja")
    find_program(CMAKE_MAKE_PROGRAM ninja)
    if(NOT CMAKE_MAKE_PROGRAM)
        message(FATAL_ERROR "Ninja not found on the PATH (https://ninja-build.org)")
    endif()
endif()

project(dpp LANGUAGES C CXX)   # the name of the program

# compiler settings (the same as in the Makefiles) ...........................
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)        # -std=c++17, not -std=gnu++17
set(CMAKE_C_STANDARD 11)             # C: the board's startup/system code
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options( # C and C++ only, not for the assembler
    "$<$<COMPILE_LANGUAGE:C,CXX>:-Wall;-Wextra;-Wpedantic;-Wsign-conversion>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti;-fno-exceptions>")
foreach(lang C CXX)
    set(CMAKE_${lang}_FLAGS_DEBUG   "-g -O")
    set(CMAKE_${lang}_FLAGS_RELEASE "-Os -DNDEBUG")    # no -flto on ARMv6-M
    set(CMAKE_${lang}_FLAGS_SPY     "-g -O")
endforeach()

# the board .................................................................
add_subdirectory(${QPCPP_DIR}/3rd_party/nucleo-c031c6 nucleo-c031c6)

# QP/C++ framework ..........................................................
add_library(qpcpp_config INTERFACE)  # qp_config.hpp is in this directory
target_include_directories(qpcpp_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(qpcpp_config INTERFACE nucleo-c031c6::cpu) # CPU flags

set(QPCPP_PORT arm-cm)
if(NOT QPCPP_KERNEL)                 # normally set by the preset
    set(QPCPP_KERNEL qv)
endif()
string(TOLOWER "${QPCPP_KERNEL}" kernel)
add_subdirectory(${QPCPP_DIR} qpcpp)

# the application ...........................................................
add_executable(${PROJECT_NAME}
    main.cpp
    philo.cpp
    table.cpp
    ${kernel}/bsp.cpp
)
if(kernel STREQUAL "qxk")
    target_sources(${PROJECT_NAME} PRIVATE qxk/xthread1.cpp qxk/xthread2.cpp)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE qpcpp::qpcpp nucleo-c031c6)
if(CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
    set(script ${kernel}/armclang/dpp-${kernel}.sct)   # scatter file
else()
    set(script ${kernel}/gnu/dpp-${kernel}.ld)
endif()
link_image(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${script})
