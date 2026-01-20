//============================================================================
// QP/C++ main function (calc example)
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
#include "qpcpp.hpp"        // QP/C++ real-time event framework
#include "bsp.hpp"          // Board Support Package
#include "app.hpp"          // Application interface

#include <iostream>

using namespace QP;
using namespace std;

//............................................................................
int main() {

    QF::init();

    cout << "Calculator example, QEP version: "
         << QP_VERSION_STR << endl
         << "Press '0' .. '9'     to enter a digit\n"
            "Press '.'            to enter the decimal point\n"
            "Press '+' or '#'     to add\n"
            "Press '-'            to subtract or negate a number\n"
            "Press '*'            to multiply\n"
            "Press '/'            to divide\n"
            "Press '=' or <Enter> to get the result\n"
            "Press 'c' or 'C'     to Cancel\n"
            "Press 'e' or 'E'     to Cancel Entry\n"
            "Press <Esc>          to quit.\n\n";

    QF::onStartup();

    the_calc->init(0U); // trigger initial transition

    for (;;) { // event loop

        BSP::show_display(); // show the display

        cout << ": ";
        std::uint8_t key_code =
             static_cast<std::uint8_t>(QF::consoleWaitForKey());
        cout << static_cast<char>(key_code) << ' ';

        QSignal sig = 0U; // assume invalid event
        switch (key_code) {
            case 'c': // intentionally fall through
            case 'C': {
                sig = C_SIG;
                break;
            }
            case 'e': // intentionally fall through
            case 'E': {
                sig = CE_SIG;
                break;
            }
            case '0': {
                sig = DIGIT_0_SIG;
                break;
            }
            case '1': // intentionally fall through
            case '2': // intentionally fall through
            case '3': // intentionally fall through
            case '4': // intentionally fall through
            case '5': // intentionally fall through
            case '6': // intentionally fall through
            case '7': // intentionally fall through
            case '8': // intentionally fall through
            case '9': {
                sig = DIGIT_1_9_SIG;
                break;
            }
            case '.': {
                sig = POINT_SIG;
                break;
            }
            case '+': // intentionally fall through
            case '-': // intentionally fall through
            case '*': // intentionally fall through
            case '/': {
                sig = OPER_SIG;
                break;
            }
            case '#': { // alternative '+'
                sig = OPER_SIG;
                key_code = '+';
                break;
            }
            case '=': // intentionally fall through
            case '\r': { // Enter key
                sig = EQUALS_SIG;
                break;
            }
            case '\33': { // ESC key
                sig = OFF_SIG;
                break;
            }
            default: {
                break;
            }
        }

        if (sig != 0) { // valid event generated?
            CalcEvt const calc_evt(sig, key_code);
            the_calc->dispatch(&calc_evt, 0U); // dispatch event
        }
    }

    QF::onCleanup();
    return 0;
}
