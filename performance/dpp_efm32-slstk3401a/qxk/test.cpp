//============================================================================
// DPP example for QXK
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
#include "test.hpp"

namespace DPP {
//............................................................................
static void thread_function(QP::QXThread * const me) {
    (void)me; // unused parameter
    XT_Sema.init(1U); // 1 count
    for (;;) {
        (void)XT_Sema.wait(QP::QXTHREAD_NO_TIMEOUT); // wait forever
        BSP::ledOn();
        QP::QXThread::delay(1U);  // block for 1 clock tick
        BSP::ledOff();
    }
}

// local extended thread object ..............................................
static QP::QXThread l_test(&thread_function, 0U);

// global pointer to the test thread .........................................
QP::QXThread * const XT_Test = &l_test;
QP::QXSemaphore XT_Sema;

} // namespace DPP
