//============================================================================
// Product: DPP example, uC/OS-II kernel
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
#include "test.h"
#include "assert.h"

Q_DEFINE_THIS_FILE

extern "C" {

OS_EVENT *Sema;  // uC/OS-II semaphore for testing

//............................................................................
void test_thread(void *pdata) { // uC/OS-II task signature
    (void)pdata;
    Sema = OSSemCreate(1);
    Q_ASSERT(Sema != static_cast<OS_EVENT *>(0)); // semaphore must be created
    for (;;) {
        INT8U err;
        OSSemPend(Sema, 0, &err); // wait forever
        DPP::BSP::ledOn();
        OSTimeDly(1);  // 1 clock cycle
        DPP::BSP::ledOff();
    }
}

} // extern "C"
