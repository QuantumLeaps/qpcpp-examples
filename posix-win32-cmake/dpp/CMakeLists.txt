# dpp example implementing cmake controlled generation and build for
#   dpp example on console
#   dpp GUI example, win32 GUI (MS Windows) or (GTK+3 GUI (MS Windows or Linux))
#   with all required configurations Debug, Release Spy
# and qutest support according to the dpp qutest example
# the following presets are defined in CMakePresets.json


# use a recent CMake version
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(VERSION 3.13)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14")
    cmake_policy(SET CMP0083 NEW)
endif()

message(STATUS
"----- Intro ------------------------------------------------------------------------------------
This qpcpp example implements a CMAKE controlled generation and build of
all configurations available in the qpcpp examples for host systems (win32, posix).
The generation and build is selectable via the presets defined in 'CMakePresets.json'.
The following presets are available:
    - dpp - this builds for console output, using the predefined system compiler.
    - mingw - this builds for console output, using a MinGW toolchain via a toolchains file.
    - dpp-gui - this builds for GUI output, using the predefined system compiler. The GUI is
      selected automatically. Win32 on MS Windows, GTK+3 on Posix. Force GTK+3 on MS Windows by
      setting the configuration variable 'USE_GTK'.
      e.g. generate with 'cmake --preset=dpp-gui -DUSE_GTK=TRUE'
    - mingw-gui - as above, using a MinGW toolchain via a toolchains file.

To configure and build one of these presets, do:
    'cmake --preset=<preset>', then build with 'cmake --build --preset=<preset>'
    This builds the 'Debug' configuration by default. You can build any of the standard qpcpp
    configurations ('Debug', 'Relase' or 'Spy') by doing
    'cmake --build --preset=<preset> --config=<config>'
    This behaviour is achieved by the 'Ninja Multi-Config' generator for those build presets.

To configure and build one of the test presets do
    'cmake --preset=test_<dpp|philo|table>' then build with
    'cmake --preset=test_<test>'
    The test presets use the 'Ninja' generator with the 'Spy' configuration and the C preprocessor
    macro 'Q_UTEST' predefined in 'CMakePresets.json'.

The following test configurations can be selected via preset
    - test_dpp - the qutest scenario from <QPCPP>/examples/qutest/dpp/test_dpp.
    - test_philo - the qutest scenario from <QPCPP>/examples/qutest/dpp/test_philo.
    - test_table - the qutest scenario from <QPCPP>/examples/qutest/dpp/test_table.

-- ------------------------------------------------------------------------------------------------
"
)
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
set(PROJECT_NAME dpp CACHE STRING "set to the project name of the qpcpp example project (default: dpp)")
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
    DESCRIPTION "Dining Philosophers Problem (dpp) using qpcpp@win32"
    LANGUAGES CXX C
)

# add support for SPY configuration if not set via CMAKE_TOOLCHAIN_FILE
if(NOT CMAKE_C_FLAGS_SPY)
    foreach(LANG IN ITEMS C CXX ASM)
        set(CMAKE_${LANG}_FLAGS_SPY "${CMAKE_${LANG}_FLAGS_DEBUG} -DQ_SPY")
    endforeach()
endif()

# update the module search path
include(CTest)
include(CheckIncludeFile)
include(CheckLibraryExists)

# the project target
if(QPCPP_CFG_UNIT_TEST)
    message(STATUS "UNIT test for '${CFG_QUTEST_TARGET}'")
    if(CFG_QUTEST_TARGET)
        if(NOT EXISTS qutest/test_${CFG_QUTEST_TARGET}.cpp)
            message(FATAL_ERROR "test configuration '${CFG_QUTEST_TARGET} not found!")
        else()
            add_executable(dpp qutest/bsp.cpp qutest/test_${CFG_QUTEST_TARGET}.cpp)
            if((CFG_QUTEST_TARGET STREQUAL philo) OR (CFG_QUTEST_TARGET STREQUAL table))
                target_sources(dpp PRIVATE ${CFG_QUTEST_TARGET}.cpp)
            else()
                target_sources(dpp PRIVATE philo.cpp table.cpp)
            endif()
            set_property(TARGET dpp PROPERTY OUTPUT_NAME ${CFG_QUTEST_TARGET}_test)
            target_include_directories(dpp PRIVATE ${CMAKE_SOURCE_DIR})
        endif()
    else()
        message(FATAL_ERROR "'CFG_QUTEST_TARGET' not set! Set this to 'dpp', 'philo' or 'table'.")
    endif()
else()
    add_executable(dpp main.cpp philo.cpp table.cpp)
    if(QPCPP_CFG_GUI)
        if(USE_GTK OR CMAKE_SYSTEM_NAME STREQUAL Linux OR UNIX)
            # use GTKMM3+
            target_sources(dpp PRIVATE bsp-gtk.cpp)
        elseif(CMAKE_SYSTEM_NAME STREQUAL Windows OR WIN32)
            # use Win32 GUI
            enable_language(RC)
            target_sources(dpp PRIVATE bsp-gui.cpp Resource.rc)
        else()
            message(FATAL_ERROR "no GUI support available for this build!")
        endif()
    else()
        target_sources(dpp PRIVATE bsp-con.cpp)
    endif()
endif()

# set position independent code compile/link parameters
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14")
    include(CheckPIESupported)
    check_pie_supported()
    set_property(TARGET dpp PROPERTY POSITION_INDEPENDENT_CODE FALSE)
endif()

# set the qpc include path to find our local `qp_config.h´
set(QPCPP_CFG_QPCONFIG_H_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR})
# add the qpcpp library to the project
qpcpp_sdk_init()

# add GTKMM-3.0 support
if(QPCPP_CFG_GUI AND (USE_GTK OR UNIX OR (DEFINED QPCPP_CFG PORT AND QPCPP_CFG_PORT MATCHES posix) OR (CMAKE_SYSTEM_NAME STREQUAL Linux)))
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTKMM3 REQUIRED IMPORTED_TARGET gtkmm-3.0) # QUIET IMPORTED_TARGET)

    if (NOT GTKMM3_FOUND)
        message(FATAL_ERROR "The package GTKMM3, required for this build, could not be found on the system!")
    endif()

    # Add other flags to the compiler
    # target_compile_options(dpp PRIVATE ${GTKMM3_CFLAGS_OTHER})

    # Setup CMake to use GTK+, tell the compiler where to look for headers
    # and to the linker where to look for libraries
    # target_include_directories(dpp PRIVATE ${GTKMM3_INCLUDE_DIRS})
    # target_link_directories(dpp PRIVATE ${GTKMM3_LIBRARY_DIRS})
    # target_link_options(dpp PRIVATE ${GTKMM3_LDFLAGS_OTHER})

    # Link the target to the GTK+ libraries
    target_link_libraries(dpp PRIVATE PkgConfig::GTKMM3) # ${GTKMM3_LIBRARIES})

    find_program(GLIB_COMPILE_RESOURCES
       NAMES glib-compile-resources glib-compile-resources.exe
    )
    if(NOT GLIB_COMPILE_RESOURCES)
        message(FATAL_ERROR "Program 'glib-compile-resources' cannot be found. Aborting!")
    endif()
    add_custom_command(
        OUTPUT dppImages.c dppImages.h
        MAIN_DEPENDENCY dppImages.xml
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dppImages.xml ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND  ${GLIB_COMPILE_RESOURCES}
                 --generate-source
                 --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/Res
                 --internal
                 --c-name=dppImg
                 --compiler=gcc
                 dppImages.xml
        COMMAND  ${GLIB_COMPILE_RESOURCES}
                 --generate-header
                 --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/Res
                 --internal
                 --c-name=dppImg
                 --compiler=gcc
                 dppImages.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Create GTK+3 Resources"
        VERBATIM
    )
    target_sources(dpp PRIVATE dppImages.c)
    target_include_directories(dpp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()

# set general defines and options
target_compile_definitions(dpp PRIVATE
    $<$<BOOL:${QPCPP_CFG_GUI}>:QWIN_GUI>
    $<$<BOOL:${QPCPP_CFG_UNIT_TEST}>:Q_UTEST>
)

target_compile_options(dpp PRIVATE
    $<$<BOOL:${QPCPP_CFG_VERBOSE}>:-v>
)

target_link_options(dpp PRIVATE
    $<$<BOOL:${QPCPP_CFG_VERBOSE}>:-v>
    LINKER:-Map=${CMAKE_PROJECT_NAME}.map
)

# set up qpcpp library
target_link_libraries(dpp PRIVATE
    qpcpp
)
# should a 'qp_config.h' configuration file be used and is it available
# edit the HINTS in the 'find_file()' call according to your project settings
if(USE_QP_CONFIG)
    find_file(QP_CONFIG qp_config.hpp HINTS ${CMAKE_CURRENT_SOURCE_DIR}) # try to identify 'qp_config.hpp'
    if(QP_CONFIG) # found 'qp_config.hpp'
        cmake_path(GET QP_CONFIG PARENT_PATH QP_CONFIG_DIR) # extract the path from the FQFN
        target_compile_definitions(qpcpp # add -DQP_CONFIG to the qpcpp build
            PUBLIC
                QP_CONFIG
        )
        target_include_directories(qpcpp # add the path to 'qp_config.h' to the list of include paths for qpcpp
            PUBLIC
                ${QP_CONFIG_DIR}
        )
    else() # 'qp_config.h' requested but not find - try to configure and build anyways
        message(WARNING "File 'qp_config.h' not found!")
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL Windows OR WIN32)
    target_compile_options(dpp
        PRIVATE
            $<IF:$<BOOL:${QPCPP_CFG_GUI}>,-mwindows,-mconsole>
    )

    target_link_options(dpp
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:$<IF:$<BOOL:${QPCPP_CFG_GUI}>,-mwindows,-mconsole>>
    )
endif()

# link the library with the application
# show configuration results
message(STATUS
"Configured project ${PROJECT_NAME} for ${PORT}
 Configuration options set:
    PROJECT_NAME                = ${QPCPP_PROJECT}
    SW_VERSION                  = ${SW_VERSION}

    QPCPP_CFG_GUI               = ${QPCPP_CFG_GUI}
    QPCPP_CFG_QSPY              = ${QPCPP_CFG_QSPY}
    QPCPP_CFG_UNIT_TEST         = ${QPCPP_CFG_UNIT_TEST}
    QPCPP_CFG_DEBUG             = ${QPCPP_CFG_DEBUG}

    QPCPP_CFG_KERNEL            = ${QPCPP_CFG_KERNEL}
    QPCPP_SDK_PATH              = ${QPCPP_SDK_PATH}"
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
