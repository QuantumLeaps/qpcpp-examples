//============================================================================
// Product: DPP example with lwIP
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
#ifndef APP_HPP_
#define APP_HPP_

using namespace QP;

enum DPPSignals {
   EAT_SIG = Q_USER_SIG, // published by Table to let a philosopher eat
   DONE_SIG,             // published by Philosopher when done eating
   BTN_DOWN_SIG,      // published by ISR_SysTick when user button is pressed
   BTN_UP_SIG,        // published by ISR_SysTick when user button is released

   DISPLAY_IPADDR_SIG,   // published by lwIPMgr to display IP address
   DISPLAY_CGI_SIG,      // published by lwIPMgr to display CGI text
   DISPLAY_UDP_SIG,      // published by lwIPMgr to display UDP text
   MAX_PUB_SIG,          // the last published signal

   HUNGRY_SIG,           // posted directly to Table from hungry Philosopher
   SEND_UDP_SIG,         // posted directly to lwIPMgr to send text via UDP
   LWIP_DRIVER_GROUP,    // LwIP driver signal group
   LWIP_DRIVER_END = LWIP_DRIVER_GROUP + 10, // reserve 10 signals
   LWIP_SLOW_TICK_SIG,   // slow tick signal for LwIP manager
   MAX_SIG               // the last signal
};

struct TableEvt : public QEvt {
    uint8_t philoNum; // philosopher number
};

#define MAX_TEXT_LEN 16
struct TextEvt : public QEvt {
    char text[MAX_TEXT_LEN]; // text to deliver
};

// number of philosophers
#define N_PHILO 5U

extern QActive * const AO_Philo[N_PHILO]; // "opaque" pointers to Philo  AO
extern QActive * const AO_Table;          // "opaque" pointer to Table   AO
extern QActive * const AO_LwIPMgr;        // "opaque" pointer to LwIPMgr AO

#endif // APP_HPP_
