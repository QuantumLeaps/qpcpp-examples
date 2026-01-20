//============================================================================
// Product: "Low-Power" example, dual-mode QXK kernel
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open-source GNU
// General Public License (GPL) or under the terms of one of the closed-
// source Quantum Leaps commercial licenses.
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// NOTE:
// The GPL does NOT permit the incorporation of this code into proprietary
// programs. Please contact Quantum Leaps for commercial licensing options,
// which expressly supersede the GPL and are designed explicitly for
// closed-source distribution.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"        // QP/C++ real-time event framework
#include "bsp.hpp"          // Board Support Package
#include "app.hpp"          // Application

//Q_DEFINE_THIS_FILE

// local objects -------------------------------------------------------------
static void XBlinky1_run(QP::QXThread * const me);

// global objects ------------------------------------------------------------
QP::QXThread XT_Blinky1(&XBlinky1_run, // thread routine
                        1U); // associate the thread with tick rate-1
QP::QXSemaphore XSEM_sw1;

//............................................................................
static void XBlinky1_run(QP::QXThread * const me) {
    (void)me; // unused parameter
    bool isActive = false;
    for (;;) {
        if (!isActive) {
            XSEM_sw1.wait();
            isActive = true;
        }
        else {
            BSP_tickRate1_on(); // turn on the tick rate-1 !
            for (uint8_t count = 13U; count > 0U; --count) {
                BSP_led1_on();
                QP::QXThread::delay(2U);
                BSP_led1_off();
                QP::QXThread::delay(2U);
            }
            isActive = false;
        }
    }
}
