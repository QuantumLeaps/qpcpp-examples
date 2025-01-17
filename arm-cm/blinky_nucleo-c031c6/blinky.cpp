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
#include "qpcpp.hpp"        // QP/C++ real-time embedded framework
#include "blinky.hpp"       // Blinky Application interface
#include "bsp.hpp"          // Board Support Package

// NOTES:
// 1. This is a simple "Blinky" Active Object (AO) coded manually;
// 2. Code like this can be generated automatically from state diagrams
//    by the QM modeling tool;
// 3. For more complex AOs automatic code generation avoids mistakes.
//

// unnamed namespace for local definitions with internal linkage
namespace {

//Q_DEFINE_THIS_FILE

} // unnamed namespace

namespace APP {

//............................................................................
// Blinky Active Object (AO) class...
class Blinky : public QP::QActive {
private:
    QP::QTimeEvt m_timeEvt; // private time event generator

public:
    Blinky(); // constructor
    static Blinky inst; // the only Blinky instance (Singleton)

// Blinky state machine declaration...
protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(off);
    Q_STATE_DECL(on);
};

// local objects --------------------------------------------------------------
Blinky Blinky::inst;

// global objects ------------------------------------------------------------
QP::QActive * const AO_Blinky = &Blinky::inst;; // global opaque pointer

//............................................................................
// Blinky constructor
Blinky::Blinky()
  : QP::QActive(&initial), // the superclass' constructor
    m_timeEvt(this, TIMEOUT_SIG, 0U) // the members' constructor
{
    // empty
}

//----------------------------------------------------------------------------
// Blinky state machine definition...
//
//        +--------------------+             +--------------------+
// O----->|        off         |---TIMEOUT-->|        on          |
//        +--------------------+             +--------------------+
//        |entry: BSP_ledOff() |             |entry: BSP_ledOn()  |
//        |                    |<--TIMEOUT---|                    |
//        +--------------------+             +--------------------+

//............................................................................
// top-most initial transition:
Q_STATE_DEF(Blinky, initial) {
    Q_UNUSED_PAR(e); // initialization event unused in this case

    // arm the time event to expire in half a second and every half second
    m_timeEvt.armX(BSP::TICKS_PER_SEC/2U, BSP::TICKS_PER_SEC/2U);

    // QS software tracing instrumentation (active only when Q_SPY is defined)
    QS_OBJ_DICTIONARY(&Blinky::inst);
    QS_OBJ_DICTIONARY(&Blinky::inst.m_timeEvt);
    QS_SIG_DICTIONARY(TIMEOUT_SIG, this);

    QS_FUN_DICTIONARY(&Blinky::initial);
    QS_FUN_DICTIONARY(&Blinky::off);
    QS_FUN_DICTIONARY(&Blinky::on);

    return tran(&off); // transition to "off"
}
//............................................................................
// the "off" state
Q_STATE_DEF(Blinky, off) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {         // state entry action
            BSP::ledOff();          // action to execute
            status = Q_RET_HANDLED; // entry action handled
            break;
        }
        case TIMEOUT_SIG: { // TIMEOUT event
            status = tran(&on); // transition to "on"
            break;
        }
        default: {
            status = super(&top); // superstate of this state
            break;
        }
    }
    return status;
}
//............................................................................
// the "on" state
Q_STATE_DEF(Blinky, on) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {         // state entry action
            BSP::ledOn();           // action to execute
            status = Q_RET_HANDLED; // entry action handled
            break;
        }
        case TIMEOUT_SIG: {         // TIMEOUT event
            status = tran(&off);    // transition to "off"
            break;
        }
        default: {
            status = super(&top);   // superstate of this state
            break;
        }
    }
    return status;
}

} // namespace APP

