//$file${.::cut_qmsm.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qmsm.qm
// File:  ${.::cut_qmsm.hpp}
//
// This code has been generated by QM 6.2.2 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Please contact Quantum Leaps for commercial licensing options, which
// expressly supersede the GNU General Public License and are specifically
// designed for licensees interested in retaining the proprietary status
// of the generated code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::cut_qmsm.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef CUT_QMSM_HPP_
#define CUT_QMSM_HPP_

namespace APP {

enum QMsmTstSignals {
    A_SIG = QP::Q_USER_SIG,
    B_SIG,
    C_SIG,
    D_SIG,
    E_SIG,
    F_SIG,
    G_SIG,
    H_SIG,
    I_SIG,
    MAX_SIG
};

extern QP::QAsm * const the_sm; // opaque pointer to the test SM

bool QMsmTst_isIn(std::uint32_t const state_num);

// BSP functions to display a message and exit
void BSP_display(char const *msg);
void BSP_terminate(int16_t const result);

} // namespace APP

#endif // CUT_QMSM_HPP_