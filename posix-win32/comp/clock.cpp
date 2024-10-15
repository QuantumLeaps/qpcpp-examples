//$file${.::clock.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: comp.qm
// File:  ${.::clock.cpp}
//
// This code has been generated by QM 7.0.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// Copyright (c) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                 ____________________________________
//                /                                   /
//               /    GGGGGGG    PPPPPPPP   LL       /
//              /   GG     GG   PP     PP  LL       /
//             /   GG          PP     PP  LL       /
//            /   GG   GGGGG  PPPPPPPP   LL       /
//           /   GG      GG  PP         LL       /
//          /     GGGGGGG   PP         LLLLLLL  /
//         /___________________________________/
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open-source software licensed under the GNU
// General Public License (GPL) as published by the Free Software Foundation
// (see <https://www.gnu.org/licenses>).
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
//
//$endhead${.::clock.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "alarm.hpp"
#include "clock.hpp"

#include "safe_std.h"   // portable "safe" <stdio.h>/<string.h> facilities

Q_DEFINE_THIS_FILE

// Active object class -------------------------------------------------------
//$declare${Components::AlarmClock} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Components::AlarmClock} ..................................................
class AlarmClock : public QP::QActive {
private:
    uint32_t m_current_time;
    Alarm m_alarm;

public:
    QTimeEvt m_timeEvt;

public:
    AlarmClock();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(timekeeping);
    Q_STATE_DECL(mode24h);
    Q_STATE_DECL(mode12h);
    Q_STATE_DECL(final);
}; // class AlarmClock
//$enddecl${Components::AlarmClock} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Local objects -------------------------------------------------------------
static AlarmClock l_alarmClock; // the single instance of the AO

// Global-scope objects ------------------------------------------------------
QActive * const APP_alarmClock = &l_alarmClock; // "opaque" AO pointer

//............................................................................
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Components::AlarmClock} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Components::AlarmClock} ..................................................

//${Components::AlarmClock::AlarmClock} ......................................
AlarmClock::AlarmClock()
: QActive(Q_STATE_CAST(&AlarmClock::initial)),
  m_alarm(), // orthogonal component ctor
  m_timeEvt(this, TICK_SIG, 0U)
{}

//${Components::AlarmClock::SM} ..............................................
Q_STATE_DEF(AlarmClock, initial) {
    //${Components::AlarmClock::SM::initial}
    (void)e; // unused parameter
    m_current_time = 0U;

    // (!) trigger the initial transition in the component
    m_alarm.init(m_prio);

    QS_FUN_DICTIONARY(&AlarmClock::timekeeping);
    QS_FUN_DICTIONARY(&AlarmClock::mode24h);
    QS_FUN_DICTIONARY(&AlarmClock::mode12h);
    QS_FUN_DICTIONARY(&AlarmClock::final);

    return tran(&timekeeping);
}

//${Components::AlarmClock::SM::timekeeping} .................................
Q_STATE_DEF(AlarmClock, timekeeping) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::AlarmClock::SM::timekeeping}
        case Q_ENTRY_SIG: {
            // periodic timeout every second
            m_timeEvt.armX(BSP_TICKS_PER_SEC, BSP_TICKS_PER_SEC);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::AlarmClock::SM::timekeeping}
        case Q_EXIT_SIG: {
            m_timeEvt.disarm();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::initial}
        case Q_INIT_SIG: {
            status_ = tran(&mode24h);
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::CLOCK_24H}
        case CLOCK_24H_SIG: {
            status_ = tran(&mode24h);
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::CLOCK_12H}
        case CLOCK_12H_SIG: {
            status_ = tran(&mode12h);
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::ALARM}
        case ALARM_SIG: {
            BSP_showMsg("Wake up!!!");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::ALARM_SET, ALARM_ON, ALARM_OFF}
        case ALARM_SET_SIG: // intentionally fall through
        case ALARM_ON_SIG: // intentionally fall through
        case ALARM_OFF_SIG: {
            // (!) synchronously dispatch to the orthogonal component
            m_alarm.dispatch(e, m_prio);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::TERMINATE}
        case TERMINATE_SIG: {
            BSP_showMsg("--> final");
            status_ = tran(&final);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${Components::AlarmClock::SM::timekeeping::mode24h} ........................
Q_STATE_DEF(AlarmClock, mode24h) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::AlarmClock::SM::timekeeping::mode24h}
        case Q_ENTRY_SIG: {
            BSP_showMsg("*** 24-hour mode");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::mode24h::TICK}
        case TICK_SIG: {
            // roll over in 24-hr mode?
            if (++m_current_time == 24U*60U) {
                m_current_time = 0U;
            }
            BSP_showTime24H("", m_current_time, 60U);

            // temporary synchronous event for the component
            TimeEvt const pe(TIME_SIG, m_current_time);

            // (!) synchronously dispatch to the orthogonal component
            m_alarm.dispatch(&pe, m_prio);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&timekeeping);
            break;
        }
    }
    return status_;
}

//${Components::AlarmClock::SM::timekeeping::mode12h} ........................
Q_STATE_DEF(AlarmClock, mode12h) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::AlarmClock::SM::timekeeping::mode12h}
        case Q_ENTRY_SIG: {
            BSP_showMsg("*** 12-hour mode");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::AlarmClock::SM::timekeeping::mode12h::TICK}
        case TICK_SIG: {
            // roll over in 12-hr mode?
            if (++m_current_time == 12U*60U) {
                m_current_time = 0U;
            }
            BSP_showTime12H("", m_current_time, 60U);

            // temporary synchronous event for the component
            TimeEvt const pe(TIME_SIG, m_current_time);

            // (!) synchronously dispatch to the orthogonal component
            m_alarm.dispatch(&pe, m_prio);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&timekeeping);
            break;
        }
    }
    return status_;
}

//${Components::AlarmClock::SM::final} .......................................
Q_STATE_DEF(AlarmClock, final) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::AlarmClock::SM::final}
        case Q_ENTRY_SIG: {
            BSP_showMsg("Bye! Bye!");
            QF::stop(); // terminate the application
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//$enddef${Components::AlarmClock} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
