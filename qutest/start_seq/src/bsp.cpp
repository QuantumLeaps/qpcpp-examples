//***************************************************************************
// Product: Start sequence example
// Last updated for version 7.2.0
// Last updated on  2022-12-22
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005-2020 Quantum Leaps. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"  // QP/C++ framework API
#include "bsp.hpp"    // Board Support Package interface
#include "gizmo.hpp"  // Gizmo AO

using namespace QP;

//Q_DEFINE_THIS_FILE

static std::uint8_t status;

//............................................................................
void BSP::init() {
    QS_SIG_DICTIONARY(START_SIG, nullptr);

    QS_OBJ_DICTIONARY(AO_Gizmo);
    QS_OBJ_DICTIONARY(&status);
}
//............................................................................
void BSP::msg(char const *msg) {
    QS_BEGIN_ID(QP::QS_USER1, 0U)
       QS_STR(msg);
    QS_END()
}
//............................................................................
std::uint8_t BSP::getStatus(void) {
    return status;
}

