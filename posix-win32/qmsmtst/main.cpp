//============================================================================
// QP/C++ main function (qmsmtst example)
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
#include "qpcpp.hpp"     // QP/C++ framework
#include "app.hpp"       // Application

#include <iostream>
#include <fstream>
#include <cstdlib>       // for exit()

Q_DEFINE_THIS_FILE       // name of this file for QP assertions

// local objects -----------------------------------------------------------
static std::ofstream l_out;
static void dispatch(QP::QSignal sig);

//............................................................................
int main(int argc, char *argv[ ]) {

    if (argc > 1) { // file name provided?
        l_out.open(argv[1]);
    }

    if (!l_out.is_open()) { // interactive version?
        std::cout << "QMsmTst example, built on " __DATE__ " at " __TIME__ "\n"
            "QEP: " QP_VERSION_STR "\nEnter x or X quit..." << std::endl;

        APP::the_sm->init(0U); // trigger the initial tran. in the test SM

        for (;;) { // event loop
            std::cout << "\n>>>";
            char inp[4];
            std::cin >> inp[0];

            QP::QSignal sig = 0U;
            if ('a' <= inp[0] && inp[0] <= 'i') { // in range?
                sig = static_cast<QP::QSignal>(inp[0] - 'a' + APP::A_SIG);
            }
            else if ('A' <= inp[0] && inp[0] <= 'I') { // in range?
                sig = static_cast<QP::QSignal>(inp[0] - 'A' + APP::A_SIG);
            }
            else if ((inp[0] == 'x') || (inp[0] == 'X')) { // x or X?
                sig = APP::TERMINATE_SIG; // terminate the interactive test
            }
            else {
                sig = APP::IGNORE_SIG;
            }

            QP::QEvt const e(sig);
            APP::the_sm->dispatch(&e, 0U); // dispatch the event
        }
    }
    else { // batch version
        std::cout << "QMsmTst, output saved to " << argv[1] << std::endl;
        if (l_out) {
            l_out << "QMsmTst example, QP " QP_VERSION_STR << std::endl;
        }
        else {
            std::cout << "QMsmTst example, QP " QP_VERSION_STR << std::endl;
        }

        APP::the_sm->init(0U); // trigger the initial tran. in the test SM

        // dynamic transitions
        dispatch(APP::A_SIG);
        dispatch(APP::B_SIG);
        dispatch(APP::D_SIG);
        dispatch(APP::E_SIG);
        dispatch(APP::I_SIG);
        dispatch(APP::F_SIG);
        dispatch(APP::I_SIG);
        dispatch(APP::I_SIG);
        dispatch(APP::F_SIG);
        dispatch(APP::A_SIG);
        dispatch(APP::B_SIG);
        dispatch(APP::D_SIG);
        dispatch(APP::D_SIG);
        dispatch(APP::E_SIG);
        dispatch(APP::G_SIG);
        dispatch(APP::H_SIG);
        dispatch(APP::H_SIG);
        dispatch(APP::C_SIG);
        dispatch(APP::G_SIG);
        dispatch(APP::C_SIG);
        dispatch(APP::C_SIG);

        l_out.close();
    }

    return 0;
}
//............................................................................
extern "C" Q_NORETURN Q_onError(char const * const file, int_t const  line) {
    std::cerr << "Assertion failed in " << file << ':' << line << std::endl;
    exit(-1);
}
//............................................................................
static void dispatch(QP::QSignal sig) {
    Q_REQUIRE((APP::A_SIG <= sig) && (sig <= APP::I_SIG));
    if (l_out) {
        l_out << '\n' << static_cast<char>('A' + sig - APP::A_SIG) << ':';
    }
    else {
        std::cout << '\n' << static_cast<char>('A' + sig - APP::A_SIG) << ':';
    }
    QP::QEvt e(sig);
    APP::the_sm->dispatch(&e, 0U); // dispatch the event
}

namespace APP {
//............................................................................
void BSP_display(char const *msg) {
    if (l_out) {
        l_out << msg;
    }
    else {
        std::cout << msg;
    }
}
//............................................................................
void BSP_terminate(int16_t const result) {
    std::cout << "Bye, Bye!" << std::endl;
    exit(result);
}

} // namespace APP
