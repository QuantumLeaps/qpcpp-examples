//============================================================================
// Product: DPP example
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
#ifndef APP_HPP
#define APP_HPP

using namespace QP;

enum DPPSignals {
    HUNGRY_SIG = Q_USER_SIG, // sent when Philosopher becomes hungry
   DONE_SIG,                 // sent by Philosopher when done eating
   EAT_SIG,                  // sent by Table to let a philosopher eat
   PAUSE_SIG,                // sent by the user to pause the feast
   MAX_PUB_SIG,              // the last published signal

   MOUSE_CHANGE_SIG,         // mouse change (move or click) event

   KEY_UP_REL_SIG,           // hardkey UP released
   KEY_UP_PRESS_SIG,         // hardkey UP pressed
   KEY_LEFT_REL_SIG,         // hardkey LEFT released
   KEY_LEFT_PRESS_SIG,       // hardkey LEFT pressed
   KEY_CENTER_REL_SIG,       // hardkey CENTER released
   KEY_CENTER_PRESS_SIG,     // hardkey CENTER pressed
   KEY_RIGHT_REL_SIG,        // hardkey RIGHT released
   KEY_RIGHT_PRESS_SIG,      // hardkey RIGHT pressed
   KEY_DOWN_REL_SIG,         // hardkey DOWN released
   KEY_DOWN_PRESS_SIG,       // hardkey DOWN pressed
   KEY_POWER_REL_SIG,        // hardkey POWER released
   KEY_POWER_PRESS_SIG,      // hardkey POWER pressed

   MAX_SIG                   // the last signal
};

struct TableEvt : public QEvt {
    uint8_t philoNum;  // philosopher number
};

struct MouseEvt : public QEvt {
    // payload: GUI_PID_STATE
    int x, y;
    uint8_t Pressed;
    uint8_t Layer;
};

enum { N_PHILO = 5 };  // number of philosophers

extern QActive * const AO_Philo[N_PHILO]; // "opaque" pointers to Philo AO
extern QActive * const AO_Table;          // "opaque" pointer  to Table AO

#endif // APP_HPP
