//============================================================================
// QP/C++ Real-Time Event Framework (RTEF)
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
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
// The GPL (see <www.gnu.org/licenses/gpl-3.0>) does NOT permit the
// incorporation of the QP/C software into proprietary programs. Please
// contact Quantum Leaps for commercial licensing options, which expressly
// supersede the GPL and are designed explicitly for licensees interested
// in using QP/C in closed-source proprietary applications.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "blinky.hpp"

// unnamed namespace for local definitions with internal linkage
namespace {

//Q_DEFINE_THIS_FILE

} // unnamed namespace

namespace APP {

//............................................................................
class Blinky : public QP::QActive {
private:
    QP::QTimeEvt m_timeEvt;

public:
    Blinky();
    static Blinky inst;

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(off);
    Q_STATE_DECL(on);
};

// local objects --------------------------------------------------------------
Blinky Blinky::inst;

// global objects ------------------------------------------------------------
QP::QActive * const AO_Blinky = &Blinky::inst; // opaque pointer

//............................................................................
Blinky::Blinky()
  : QP::QActive(&initial),
    m_timeEvt(this, TIMEOUT_SIG, 0U)
{
    // empty
}

// HSM definition ------------------------------------------------------------
Q_STATE_DEF(Blinky, initial) {
    (void)e; // unused parameter

    // arm the time event to expire in half a second and every half second
    m_timeEvt.armX(BSP::TICKS_PER_SEC/2U, BSP::TICKS_PER_SEC/2U);
    return tran(&off);
}
//............................................................................
Q_STATE_DEF(Blinky, off) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            BSP::ledOff();
            status = Q_RET_HANDLED;
            break;
        }
        case TIMEOUT_SIG: {
            status = tran(&on);
            break;
        }
        default: {
            status = super(&top);
            break;
        }
    }
    return status;
}
//............................................................................
Q_STATE_DEF(Blinky, on) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            BSP::ledOn();
            status = Q_RET_HANDLED;
            break;
        }
        case TIMEOUT_SIG: {
            status = tran(&off);
            break;
        }
        default: {
            status = super(&top);
            break;
        }
    }
    return status;
}

} // namespace APP

