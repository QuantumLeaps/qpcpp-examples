//$file${.::sporadic2.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: real-time.qm
// File:  ${.::sporadic2.cpp}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
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
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::sporadic2.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"           // QP/C++ real-time embedded framework
#include "bsp.hpp"             // Board Support Package
#include "app.hpp"             // Application interface

namespace {
Q_DEFINE_THIS_MODULE("sporadic2")
}

//$declare${AOs::Sporadic2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Sporadic2} ..........................................................
class Sporadic2 : public QP::QMActive {
public:
    static Sporadic2 inst;

public:
    Sporadic2();

protected:
    QM_STATE_DECL( initial);
    QM_STATE_DECL( active);
}; // class Sporadic2

} // namespace APP
//$enddecl${AOs::Sporadic2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${Shared::AO_Sporadic2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Sporadic2} ....................................................
QP::QActive * const AO_Sporadic2 {&Sporadic2::inst};

} // namespace APP
//$enddef${Shared::AO_Sporadic2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Sporadic2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Sporadic2} ..........................................................
Sporadic2 Sporadic2::inst;

//${AOs::Sporadic2::Sporadic2} ...............................................
Sporadic2::Sporadic2()
  : QMActive(Q_STATE_CAST(&Sporadic2::initial))
{}

//${AOs::Sporadic2::SM} ......................................................
QM_STATE_DEF(Sporadic2, initial) {
    //${AOs::Sporadic2::SM::initial}
    Q_UNUSED_PAR(e);

    QS_FUN_DICTIONARY(&Sporadic2::active);
    static QP::QMTranActTable const tatbl_ = { // tran-action table
        &active_s, // target state
        {
            Q_ACTION_NULL // zero terminator
        }
    };
    return qm_tran_init(&tatbl_);
}

//${AOs::Sporadic2::SM::active} ..............................................
QP::QMState const Sporadic2::active_s = {
    QM_STATE_NULL, // superstate (top)
    &Sporadic2::active,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${AOs::Sporadic2::SM::active}
QM_STATE_DEF(Sporadic2, active) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Sporadic2::SM::active::SPORADIC_A}
        case SPORADIC_A_SIG: {
            BSP::d5on();
            AO_Sporadic3->POST(e, this); // Sporadic2 --> Sporadic3
            BSP::d5off();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Sporadic2::SM::active::SPORADIC_B}
        case SPORADIC_B_SIG: {
            BSP::d5on();
            AO_Periodic1->POST(BSP::getEvtPeriodic1(1U), this);
            BSP::d5off();

            for (std::uint16_t i = Q_EVT_CAST(SporadicSpecEvt)->toggles;
                 i > 0U; --i)
            {
                BSP::d5on();
                BSP::d5off();
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = Q_RET_SUPER;
            break;
        }
    }
    return status_;
}

} // namespace APP
//$enddef${AOs::Sporadic2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
