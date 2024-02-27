# dpp

# use a recent CMake version
cmake_minimum_required(VERSION 3.23 FATAL_ERROR)
cmake_policy(SET CMP0083 NEW)

# first of all protect against in-source builds
file(REAL_PATH "${CMAKE_SOURCE_DIR}" _srcdir)
file(REAL_PATH "${CMAKE_BINARY_DIR}" _bindir)

if(${_srcdir} STREQUAL ${_bindir})
    message(FATAL_ERROR "  FATAL: In-source builds are not allowed!
        You should create a separate directory for build files.")
endif()
unset(_srcdir)
unset(_bindir)

# set up configurable options
set(PROJECT_NAME dpp CACHE STRING "set to the project name of the qpcpp library project (default: QPCPP)")
set(QPCPP_CFG_KERNEL QV CACHE STRING "set to the kernel to use (QV, QK, QXK) (default: QV)")
set(QPCPP_SDK_PATH "${CMAKE_SOURCE_DIR}/../../.." CACHE STRING "set to the source path of QPCPP (default: ../../..)")

option(QPCPP_CFG_UNIT_TEST  "set to ON, if Q_UTEST shall be enabled (default: OFF)" OFF)
option(QPCPP_CFG_GUI        "set to ON, if a Windows (TM) GUI shall be compiled in (default: ON)" OFF)
option(QPCPP_CFG_VERBOSE   "set to ON, to enable more verbosity. (default: OFF)" OFF)
option(QPCPP_CFG_DEBUG     "set to ON, to enable debug settings. (default: ON)" ON)

# update CMAKE_MODULE_PATH
list(PREPEND CMAKE_MODULE_PATH ${QPCPP_SDK_PATH}/3rd_party/cmake ${CMAKE_SOURCE_DIR}/Source/cmake ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_SOURCE_DIR} ${HOME_PATH}/cmake)

# import QPCPP SDK; download from git, if necessary
include(qpcpp_sdk_import)

if(NOT SW_VERSION)
    set(SW_VERSION "1.0.0" CACHE STRING "Software Version")
endif()

project(
    ${PROJECT_NAME}
    VERSION ${SW_VERSION}
    DESCRIPTION "Dining Philosophers Problem (dpp)"
    LANGUAGES CXX C
)

# add support for SPY configuration if not set via CMAKE_TOOLCHAIN_FILE
if(NOT CMAKE_CXX_FLAGS_SPY)
    foreach(LANG IN ITEMS C CXX ASM)
        set(CMAKE_${LANG}_FLAGS_SPY "${CMAKE_${LANG}_FLAGS_DEBUG} -DQ_SPY")
    endforeach()
endif()

include(CTest)
include(CheckIncludeFile)
include(CheckLibraryExists)

# the project main target
set(TGT ${PROJECT_NAME})
add_executable(${TGT})

# set position independent code compile/link parameters
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14")
    include(CheckPIESupported)
    check_pie_supported()
    set_property(TARGET ${TGT} PROPERTY POSITION_INDEPENDENT_CODE FALSE)
endif()

# add the qpcpp library to the project
qpcpp_sdk_init()

# check for plausible settings
string(TOLOWER ${CMAKE_C_COMPILER_ID} _compiler_)
string(TOLOWER ${QPCPP_CFG_PORT} PORT)
if(NOT (PORT STREQUAL arm-cm AND _compiler_ STREQUAL gnu))
    message(FATAL_ERROR "'${QPCPP_CFG_PORT}'/'${CMAKE_C_COMPILER_ID}' not supported! Only 'arm-cm' port with GNU compiler is currently supported!")
endif()

# add project specific setting to the library buiild
target_compile_definitions(qpcpp PUBLIC
    STM32L053xx
)

target_compile_options(qpcpp PUBLIC
    $<$<BOOL:${QPCPP_CFG_VERBOSE}>:-v>
    -mcpu=cortex-m0plus
)

# the main target's sources
target_sources(${TGT} PRIVATE
    $<LOWER_CASE:${QPCPP_CFG_KERNEL}>/bsp.cpp
    main.cpp
    philo.cpp
    table.cpp
)

add_subdirectory(${QPCPP_SDK_PATH}/3rd_party/nucleo-l053r8 nucleo-l053r8)
target_include_directories(dpp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(dpp PRIVATE
    STM32L053xx
    $<$<CONFIG:Spy>:Q_SPY>
)

target_link_options(dpp PRIVATE
    $<$<BOOL:${QPCPP_CFG_VERBOSE}>:-v>
    -T${CMAKE_CURRENT_SOURCE_DIR}/$<LOWER_CASE:${QPCPP_CFG_KERNEL}>/${_compiler_}/dpp-$<LOWER_CASE:${QPCPP_CFG_KERNEL}>.ld
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${TGT}.map
    -mcpu=cortex-m0plus
)

# link the library with the application
target_link_libraries(dpp PRIVATE
    qpcpp
)

# show configuration results
message(STATUS
"Configured project ${PROJECT_NAME} for ${PORT}
 Configuration options set:
    PROJECT_NAME                = ${QPCPP_PROJECT}
    SW_VERSION                  = ${SW_VERSION}

    CONFIG_GUI                  = ${QPCPP_CFG_GUI}
    CONFIG_QSPY                 = ${QPCPP_CFG_QSPY}
    CONFIG_UNIT_TEST            = ${QPCPP_CFG_UNIT_TEST}
    CONFIG_DEBUG                = ${CONFIG_DEBUG}

    QPCPP_CFG_KERNEL            = ${QPCPP_CFG_KERNEL}
    QPCPP_DIR                   = ${QPCPP_DIR}"
)

if(QPCPP_CFG_VERBOSE)
message(STATUS
" System information:
    CMAKE_VERSION               = ${CMAKE_VERSION}
    CMAKE_CROSSCOMPILING        = ${CMAKE_CROSSCOMPILING}
    CMAKE_HOST_SYSTEM           = ${CMAKE_HOST_SYSTEM}
    CMAKE_HOST_SYSTEM_NAME      = ${CMAKE_HOST_SYSTEM_NAME}
    CMAKE_HOST_LINUX            = ${CMAKE_HOST_LINUX}
    CMAKE_HOST_UNIX             = ${CMAKE_HOST_UNIX}
    CMAKE_HOST_WIN32            = ${CMAKE_HOST_WIN32}
    CMAKE_SYSTEM                = ${CMAKE_SYSTEM}
    CMAKE_SYSTEM_NAME           = ${CMAKE_SYSTEM_NAME}
    CMAKE_SYSTEM_PROCESSOR      = ${CMAKE_SYSTEM_PROCESSOR}
    WIN32                       = ${WIN32}
    MSYS                        = ${MSYS}
    MINGW                       = ${MINGW}
    UNIX                        = ${UNIX}
    LINUX                       = ${LINUX}

    CMAKE_BUILD_TYPE            = ${CMAKE_BUILD_TYPE}
    CMAKE_CONFIGURATION_TYPES   = ${CMAKE_CONFIGURATION_TYPES}

    CMAKE_C_COMPILER            = ${CMAKE_C_COMPILER}
    CMAKE_C_COMPILER_ID         = ${CMAKE_C_COMPILER_ID}
    CMAKE_C_COMPILER_VERSION    = ${CMAKE_C_COMPILER_VERSION}
    CMAKE_C_FLAGS               = ${CMAKE_C_FLAGS}

    CMAKE_CXX_COMPILER          = ${CMAKE_CXX_COMPILER}
    CMAKE_CXX_FLAGS             = ${CMAKE_CXX_FLAGS}

    CMAKE_ASM_COMPILER          = ${CMAKE_ASM_COMPILER}
    CMAKE_ASM_FLAGS             = ${CMAKE_ASM_FLAGS}"
)
endif()
