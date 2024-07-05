//============================================================================
// Product: BSP for DPP example (console)
// Last updated for version 7.4.0
// Last updated on  2024-07-31
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Contact information:
// <www.state-machine.com>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"        // QP/C++ real-time embedded framework
#include "dpp.hpp"          // DPP Application interface
#include "bsp.hpp"          // Board Support Package

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_MODULE("bsp")

// Local objects -------------------------------------------------------------
static std::uint32_t l_rnd; // random seed

#ifdef Q_SPY

    enum AppRecords { // application-specific trace records
        BSP_CALL = QP::QS_USER,
    };

#endif

} // unnamed namespace


//============================================================================
namespace BSP {

void init(int argc, char** argv) {
    BSP::randomSeed(1234U);

    // initialize the QS software tracing
    if (!QS_INIT(argc <= 1 ? nullptr : argv[1])) {
        Q_ERROR();
    }

    QS_FUN_DICTIONARY(&BSP::displayPaused);
    QS_FUN_DICTIONARY(&BSP::displayPhilStat);
    QS_FUN_DICTIONARY(&BSP::random);
    QS_FUN_DICTIONARY(&BSP::randomSeed);

    QS_ONLY(APP::produce_sig_dict());

    QS_USR_DICTIONARY(BSP_CALL);
}
//............................................................................
void displayPhilStat(std::uint8_t n, char const *stat) {
    QS_BEGIN_ID(BSP_CALL, 0U) // app-specific record
        QS_FUN(&BSP::displayPhilStat);
        QS_U8(0, n);
        QS_STR(stat);
    QS_END()
}
//............................................................................
void displayPaused(std::uint8_t const paused) {
    QS_TEST_PROBE_DEF(&BSP::displayPaused)

    QS_TEST_PROBE(
        Q_ASSERT_ID(100, 0);
    )
    QS_BEGIN_ID(BSP_CALL, 0U) // app-specific record
        QS_FUN(&BSP::displayPaused);
        QS_U8(0, paused);
    QS_END()
}
//............................................................................
std::uint32_t random() {
    QS_TEST_PROBE_DEF(&BSP::random)
    uint32_t rnd = 123U;

    QS_TEST_PROBE(
        rnd = qs_tp_;
    )
    QS_BEGIN_ID(BSP_CALL, 0U) // app-specific record
        QS_FUN(&BSP::random);
        QS_U32(0, rnd);
    QS_END()
    return rnd;
}
//............................................................................
void randomSeed(std::uint32_t seed) {
    QS_TEST_PROBE_DEF(&BSP::randomSeed)

    QS_TEST_PROBE(
        seed = qs_tp_;
    )
    l_rnd = seed;
    QS_BEGIN_ID(BSP_CALL, 0U) // app-specific record
        QS_FUN(&BSP::randomSeed);
        QS_U32(0, seed);
    QS_END()
}

} // namespace BSP

