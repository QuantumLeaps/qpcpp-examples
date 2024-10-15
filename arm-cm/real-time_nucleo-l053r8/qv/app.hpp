//$file${.::app.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: real-time.qm
// File:  ${.::app.hpp}
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
//$endhead${.::app.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef APP_HPP_
#define APP_HPP_

//$declare${Shared} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AppSignals} ......................................................
enum AppSignals : QP::QSignal {
    PERIODIC_SPEC_SIG = QP::Q_USER_SIG,
    TIMEOUT_SIG,
    SPORADIC_A_SIG,
    SPORADIC_B_SIG,
    REMINDER_SIG,
    // ...
    MAX_SIG  // the last signal
};

//${Shared::PeriodicSpecEvt} .................................................
class PeriodicSpecEvt : public QP::QEvt {
public:
    std::uint16_t toggles;
    std::uint8_t ticks;

public:
    constexpr PeriodicSpecEvt(
        QP::QSignal s,
        std::uint16_t tg,
        std::uint8_t ti)
     : QP::QEvt(s),
       toggles(tg),
       ticks(ti)
    {}
}; // class PeriodicSpecEvt

//${Shared::SporadicSpecEvt} .................................................
class SporadicSpecEvt : public QP::QEvt {
public:
    std::uint16_t toggles;
    std::uint16_t rtc_toggles;

public:
    constexpr SporadicSpecEvt(
        QP::QSignal s,
        std::uint16_t tg,
        std::uint16_t trc_tg)
     : QP::QEvt(s),
       toggles(tg),
       rtc_toggles(trc_tg)
    {}
}; // class SporadicSpecEvt

//${Shared::AO_Periodic1} ....................................................
extern QP::QActive * const AO_Periodic1;

//${Shared::AO_Periodic4} ....................................................
extern QP::QActive * const AO_Periodic4;

//${Shared::AO_Sporadic2} ....................................................
extern QP::QActive * const AO_Sporadic2;

//${Shared::AO_Sporadic3} ....................................................
extern QP::QActive * const AO_Sporadic3;

} // namespace APP
//$enddecl${Shared} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif // APP_HPP_
