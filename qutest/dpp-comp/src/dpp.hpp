//$file${.::dpp.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: dpp-comp.qm
// File:  ${.::dpp.hpp}
//
// This code has been generated by QM 6.1.1 <www.state-machine.com/qm>.
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
//$endhead${.::dpp.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef DPP_HPP_
#define DPP_HPP_

//$declare${Shared} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AppSignals} ......................................................
enum AppSignals : QP::QSignal {
    EAT_SIG = QP::Q_USER_SIG, // published by Table to let a Philo eat
    DONE_SIG,       // published by Philo when done eating
    PAUSE_SIG,      // published by BSP to pause the application
    SERVE_SIG,      // published by BSP to serve re-start serving forks
    TEST_SIG,       // published by BSP to test the application
    MAX_PUB_SIG,    // the last published signal

    TIMEOUT_SIG,    // posted by time event to Philo
    HUNGRY_SIG,     // posted by hungry Philo to Table
    MAX_SIG         // the last signal
};

//${Shared::produce_sig_dict} ................................................
#ifdef Q_SPY
inline void produce_sig_dict() {
    QS_SIG_DICTIONARY(EAT_SIG,     nullptr);
    QS_SIG_DICTIONARY(DONE_SIG,    nullptr);
    QS_SIG_DICTIONARY(PAUSE_SIG,   nullptr);
    QS_SIG_DICTIONARY(SERVE_SIG,   nullptr);
    QS_SIG_DICTIONARY(TEST_SIG,    nullptr);

    QS_SIG_DICTIONARY(TIMEOUT_SIG, nullptr);
    QS_SIG_DICTIONARY(HUNGRY_SIG,  nullptr);
}
#endif // def Q_SPY

//${Shared::N_PHILO} .........................................................
constexpr std::uint8_t N_PHILO {5};

//${Shared::TableEvt} ........................................................
class TableEvt : public QP::QEvt {
public:
    std::uint8_t philoId;

public:
    constexpr TableEvt(
        QP::QSignal sig,
        std::uint8_t id)
     : QEvt(sig),
       philoId(id)
    {}

#ifdef QEVT_DYN_CTOR
    TableEvt * ctor(std::uint8_t id) {
        philoId = id;

        return this;
    }
#endif // def QEVT_DYN_CTOR
}; // class TableEvt

//${Shared::AO_Table} ........................................................
extern QP::QActive * const AO_Table;

} // namespace APP
//$enddecl${Shared} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$declare${Comp::CompTimeEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Comp::CompTimeEvt} .......................................................
class CompTimeEvt : public QP::QTimeEvt {
public:
    std::uint16_t compNum;

public:
    CompTimeEvt(
        QP::QActive * act,
        std::uint16_t num,
        enum_t const sig,
        std::uint_fast8_t const tickRate);
}; // class CompTimeEvt

} // namespace APP
//$enddecl${Comp::CompTimeEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$declare${Comp::SM_Philo[N_PHILO]} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Comp::SM_Philo[N_PHILO]} .................................................
extern QP::QAsm * const SM_Philo[N_PHILO];

} // namespace APP
//$enddecl${Comp::SM_Philo[N_PHILO]} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif // DPP_HPP_
