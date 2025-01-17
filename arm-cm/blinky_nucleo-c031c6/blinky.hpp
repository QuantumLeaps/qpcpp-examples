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

#ifndef BLINKY_HPP_
#define BLINKY_HPP_

namespace APP {

enum AppSignals : QP::QSignal {
    DUMMY_SIG = QP::Q_USER_SIG,
    //...
    MAX_PUB_SIG, // the last published signal

    TIMEOUT_SIG,
    //...
    MAX_SIG      // the last signal
};

extern QP::QActive * const AO_Blinky; // global opaque pointer

// for the QXK kernel only...
#ifdef QXK_HPP_
extern QP::QXThread * const XT_thr; // global opaque pointer
#endif

} // namespace APP

#endif // BLINKY_HPP_
