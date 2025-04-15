//============================================================================
// "Blinky" example
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
#include "blinky.hpp"       // Blinky Application interface
#include "bsp.hpp"          // Board Support Package

// NOTES:
// 1. This is an example of a an eXtened thread (with blocking) for the
//    QXK kernel.
// 2. The QXK dual-mode kernel component is NOT provided in the open-source
//    QP/C++ distribution. Please contact Quantum Leaps for free evaluation
//    of the QXK kernel.
// 3. The QS software tracing instrumentation is active only in the
//    "spy" build configuration (Q_SPY macro defined). This build
//    configuration requires QS component, which is NOT provided in
//    the open source QP/C++ distribution. Please contact Quantum Leaps
//    for free evaluation of the softare tracing component.

//----------------------------------------------------------------------------
// unnamed namespace for local definitions with internal linkage
namespace {
//Q_DEFINE_THIS_FILE
}

//----------------------------------------------------------------------------
namespace APP {

// eXtended Thread class declaration...
class XThr : public QP::QXThread {
private:

    static void run(QP::QXThread * const thr); // thread runction

    // data members...
public:
    XThr(); // constructor
    static XThr inst; // XThr instance (Singleton)
    //...
};

//----------------------------------------------------------------------------
// XThr eXtended thread class definition...

XThr XThr::inst; // the XThr instance definition
QP::QXThread * const XT_thr = &XThr::inst; // global opaque pointer

//............................................................................
// XThr "constructor"
XThr::XThr()
 : QXThread(&run, // thread function
            0U)   // tick rate for timeouts
{}

//............................................................................
void XThr::run(QP::QXThread * const thr) {
    // QS software tracing instrumentation (active only when Q_SPY is defined)
    QS_OBJ_DICTIONARY(XT_thr);

    for (;;) { // for-ever loop (must have at least one BLOCKING call)

        delay(BSP::TICKS_PER_SEC);  // delay (BLOCK) for 1 second

        // QS software tracing output (active only when Q_SPY is defined)
        QS_BEGIN_ID(QP::QS_USER, 0U)
            QS_OBJ(XT_thr);
            QS_STR("woken");
        QS_END()
    }
}

} // namespace APP
