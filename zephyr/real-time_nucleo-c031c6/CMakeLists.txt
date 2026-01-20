# QPCPP Zephyr example
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(real-time)

target_sources(app PRIVATE
 src/main.cpp
 src/bsp.cpp
 src/periodic1.cpp
 src/periodic4.cpp
 src/sporadic2.cpp
 src/sporadic3.cpp
)

# QPCPP framework directory
# NOTE:
# Adjust this path for a different relative path
# between your project and the QPCPP folder
#
set(QPCPP_DIR ../../..)

# option to turn QSPY software tracing ON or OFF (file prj.conf)
# NOTE:
# change of the option might require re-building the project
#
option(QSPY "QSPY software tracing" OFF)

# QPCPP framework includes and sources...
zephyr_include_directories(
 ${QPCPP_DIR}/include
 ${QPCPP_DIR}/zephyr
 ./src
)
target_sources(app PRIVATE
 ${QPCPP_DIR}/src/qf/qep_hsm.cpp
 ${QPCPP_DIR}/src/qf/qep_msm.cpp
 ${QPCPP_DIR}/src/qf/qf_act.cpp
 ${QPCPP_DIR}/src/qf/qf_qact.cpp
 ${QPCPP_DIR}/src/qf/qf_defer.cpp
 ${QPCPP_DIR}/src/qf/qf_dyn.cpp
 ${QPCPP_DIR}/src/qf/qf_mem.cpp
 ${QPCPP_DIR}/src/qf/qf_ps.cpp
 ${QPCPP_DIR}/src/qf/qf_qact.cpp
 ${QPCPP_DIR}/src/qf/qf_qeq.cpp
 ${QPCPP_DIR}/src/qf/qf_qmact.cpp
 ${QPCPP_DIR}/src/qf/qf_time.cpp
 ${QPCPP_DIR}/zephyr/qf_port.cpp
)

# application-level "idle thread"...
if(QIDLE)
add_compile_definitions(QF_IDLE)
endif()
