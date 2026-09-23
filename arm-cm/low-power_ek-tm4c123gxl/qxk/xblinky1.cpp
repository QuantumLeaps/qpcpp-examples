//============================================================================
// Product: "Low-Power" example, dual-mode QXK kernel
//
// Copyright (C) 2005-2026 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open-source GNU
// General Public License, version 3 or later (GPL-3.0-or-later), or,
// alternatively, under the terms of one of the closed-source Quantum Leaps
// commercial licenses.
//
// Redistributions in source code must retain this top-level comment block.
//
// NOTE:
// The GPL does NOT permit distributing this code as part of proprietary
// (closed-source) programs. For such use, please contact Quantum Leaps for
// a commercial license, which is an alternative to the GPL designed
// explicitly for closed-source distribution.
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
