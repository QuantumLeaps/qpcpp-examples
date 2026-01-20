//***************************************************************************
// Product: Start sequence example
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
#include "qpcpp.hpp"  // QP/C++ framework API
#include "bsp.hpp"    // Board Support Package
#include "app.hpp"    // Application

//Q_DEFINE_THIS_FILE

static std::uint8_t status;

//............................................................................
void BSP::init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    QS_SIG_DICTIONARY(START_SIG, nullptr);
    QS_OBJ_DICTIONARY(AO_Gizmo);
    QS_OBJ_DICTIONARY(&status);
}
//............................................................................
void BSP::msg(char const *msg) {
    QS_BEGIN_ID(QP::QS_USER1, 0U)
       QS_STR(msg);
    QS_END()
}
//............................................................................
std::uint8_t BSP::getStatus(void) {
    return status;
}

