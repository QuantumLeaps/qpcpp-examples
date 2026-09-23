//============================================================================
// qhsmtst example
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
#ifndef QHSMTST_HPP_
#define QHSMTST_HPP_

namespace APP {

enum QHsmTstSignals : QP::QSignal {
    A_SIG = QP::Q_USER_SIG,
    B_SIG,
    C_SIG,
    D_SIG,
    E_SIG,
    F_SIG,
    G_SIG,
    H_SIG,
    I_SIG,
    X_SIG, // not used
    MAX_SIG
};

extern QP::QActive * const AO_QHsmTst;

} // namespace APP

#endif // QHSMTST_HPP_
