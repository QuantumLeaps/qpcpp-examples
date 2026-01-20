//============================================================================
// Product: Board Support Package (BSP) for the Calculator example
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
#include "qpcpp.hpp"  // QP/C++
#include "bsp.hpp"    // board support package
#include "app.hpp"    // application

#include "safe_std.h" // portable "safe" <stdio.h>/<string.h> facilities
#include <stdlib.h>   // for exit()

using namespace QP;
using namespace std;

#define DISP_WIDTH      15

#ifdef Q_SPY
    #error This application does not support Spy build configuration
#endif

// helper macros to "stringify" values
#define VAL(x) #x
#define STRINGIFY(x) VAL(x)

static char l_display[DISP_WIDTH + 1]; // the calculator display
static int  l_len;  // number of displayed characters

//============================================================================
namespace BSP {

//............................................................................
void clear() {
    memset(l_display, ' ', DISP_WIDTH - 1);
    l_display[DISP_WIDTH - 1] = '0';
    l_display[DISP_WIDTH] = '\0';
    l_len = 0;
}
//............................................................................
void insert(int keyId) {
    if (l_len == 0) {
        l_display[DISP_WIDTH - 1] = (char)keyId;
        ++l_len;
    }
    else if (l_len < (DISP_WIDTH - 1)) {
        MEMMOVE_S(&l_display[0], DISP_WIDTH, &l_display[1], DISP_WIDTH - 1);
        l_display[DISP_WIDTH - 1] = (char)keyId;
        ++l_len;
    }
}
//............................................................................
void display(double value) {
    SNPRINTF_S(l_display, Q_DIM(l_display),
        "%" STRINGIFY(DISP_WIDTH) ".6g", value);
}
//............................................................................
void display_error(char const *err) {
    STRNCPY_S(l_display, DISP_WIDTH, err);
}
//............................................................................
void negate() {
    clear();
    l_display[DISP_WIDTH - 2] = '-';
}
//............................................................................
void show_display(void) {
    PRINTF_S("\n[%" STRINGIFY(DISP_WIDTH) "s] ", l_display);
}
//............................................................................
void stop() {
    PRINTF_S("\n%s\n", "Bye! Bye!");
    fflush(stdout);
    QF::onCleanup();
    exit(0);
}
//............................................................................
double get_value() {
    return strtod(l_display, (char **)0);
}
//............................................................................
void message(char const *msg) {
    PRINTF_S("%s", msg);
}

} // namespace BSP

//============================================================================

namespace QP {

//............................................................................
void QF::onStartup(void) {
    QF::consoleSetup();
}
//............................................................................
void QF::onCleanup(void) {
    QF::consoleCleanup();
}
//............................................................................
void QF::onClockTick(void) {
}

} // namespace QP

//............................................................................
// this function is used by the QP embedded systems-friendly assertions
extern "C" Q_NORETURN Q_onError(char const * const module, int_t const id) {
    FPRINTF_S(stderr, "ERROR in %s:%d", module, id);
    QF::onCleanup();
    exit(-1);
}
