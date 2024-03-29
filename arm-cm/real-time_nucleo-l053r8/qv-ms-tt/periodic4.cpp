//$file${.::periodic4.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: real-time-ms-tt.qm
// File:  ${.::periodic4.cpp}
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
//$endhead${.::periodic4.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"           // QP/C++ real-time embedded framework
#include "bsp.hpp"             // Board Support Package
#include "app.hpp"             // Application interface

namespace {
Q_DEFINE_THIS_MODULE("periodic4")
}

//$declare${Components::Periodic4} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Components::Periodic4} ...................................................
class Periodic4 : public QP::QMActive {
private:
    std::uint16_t m_toggles;

public:
    static Periodic4 inst;

public:
    Periodic4();

protected:
    QM_STATE_DECL( initial);
    QM_STATE_DECL( active);
}; // class Periodic4

} // namespace APP
//$enddecl${Components::Periodic4} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${Shared::AO_Periodic4} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Periodic4} ....................................................
QP::QActive * const AO_Periodic4 {&Periodic4::inst};

} // namespace APP
//$enddef${Shared::AO_Periodic4} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Components::Periodic4} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Components::Periodic4} ...................................................
Periodic4 Periodic4::inst;

//${Components::Periodic4::Periodic4} ........................................
Periodic4::Periodic4()
  : QMActive(Q_STATE_CAST(&Periodic4::initial))
{}

//${Components::Periodic4::SM} ...............................................
QM_STATE_DEF(Periodic4, initial) {
    //${Components::Periodic4::SM::initial}
    Q_REQUIRE_ID(300, QP::QEvt::verify_(e)
                 && (e->sig == PERIODIC_SPEC_SIG));

    m_toggles = Q_EVT_CAST(PeriodicSpecEvt)->toggles;

    QS_FUN_DICTIONARY(&Periodic4::active);
    static QP::QMTranActTable const tatbl_ = { // tran-action table
        &active_s, // target state
        {
            Q_ACTION_NULL // zero terminator
        }
    };
    return qm_tran_init(&tatbl_);
}

//${Components::Periodic4::SM::active} .......................................
QP::QMState const Periodic4::active_s = {
    QM_STATE_NULL, // superstate (top)
    &Periodic4::active,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::Periodic4::SM::active}
QM_STATE_DEF(Periodic4, active) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::Periodic4::SM::active::RUN}
        case RUN_SIG: {
            for (std::uint16_t i = m_toggles; i > 0U; --i) {
                BSP::d3on();
                BSP::d3off();
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Periodic4::SM::active::PERIODIC_SPEC}
        case PERIODIC_SPEC_SIG: {
            BSP::d3on();
            m_toggles = Q_EVT_CAST(PeriodicSpecEvt)->toggles;
            BSP::d3off();
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
//$enddef${Components::Periodic4} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
