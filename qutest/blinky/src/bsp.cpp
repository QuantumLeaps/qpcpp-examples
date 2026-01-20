//============================================================================
// BSP for "Blinky" example
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

namespace {
//Q_DEFINE_THIS_FILE

enum {
   LED = QP::QS_USER
};
} // namespace

//============================================================================
namespace BSP {

//............................................................................
//..........................................................................
void init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    QS_USR_DICTIONARY(LED);
}
//............................................................................
void ledOff(void) {
    QS_BEGIN_ID(LED, APP::AO_Blinky->getPrio())
       QS_U8(1, 0);
    QS_END()
}
//............................................................................
void ledOn(void) {
    QS_BEGIN_ID(LED, APP::AO_Blinky->getPrio())
       QS_U8(1, 1);
    QS_END()
}

} // namespace BSP
