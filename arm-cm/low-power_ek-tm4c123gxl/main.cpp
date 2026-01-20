//============================================================================
// DPP example
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

//............................................................................
int main() {
    static QP::QSubscrList subscrSto[MAX_PUB_SIG];

    static QP::QEvtPtr blinky0QueueSto[10]; // queue storage for Blinky0
#ifdef QXK_HPP_ // QXK kernel?
    static uint32_t const *xblinky1Stack[64]; // stack for XBlinky1
#else
    static QP::QEvtPtr blinky1QueueSto[10]; // queue storage for Blinky1
#endif // QXK_HPP_

    QP::QF::init();  // initialize the framework and the underlying RT kernel

    BSP_init((void *)0); // initialize the BSP and start the AOs

    // initialize publish-subscribe
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    //QP::QF::poolInit(smlPoolSto,
    //                 sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // start the xthreads/active objects...
    AO_Blinky0->start(1U, // priority
                      blinky0QueueSto, Q_DIM(blinky0QueueSto),
                      0, 0U, 0);

#ifdef QXK_HPP_ // QXK kernel?
    XSEM_sw1.init(0U, 1U); // binary signaling semaphore
    XT_Blinky1.start(2U,  // unique QP priority of the AO
                  0, 0U,   // event queue (not used)
                  xblinky1Stack,  // stack storage (must provide in QXK)
                  sizeof(xblinky1Stack), // stack size [bytes]
                  0);     // initial event (or 0)
#else // QV or QK kernels
    AO_Blinky1->start(2U, // priority
                    blinky1QueueSto, Q_DIM(blinky1QueueSto),
                    0, 0U, 0);
#endif // QXK_HPP_

    return QP::QF::run(); // run the QF application
}
