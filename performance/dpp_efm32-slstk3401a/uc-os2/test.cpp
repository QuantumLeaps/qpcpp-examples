//============================================================================
// Product: DPP example, uC/OS-II kernel
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
