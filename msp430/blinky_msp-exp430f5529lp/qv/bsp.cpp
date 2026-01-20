//============================================================================
// Product: "Blinky" example on MSP-EXP430F5529LP board, preemptive QV kernel
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
#include "app.hpp"          // Application

#include <msp430f5529.h>  // MSP430 variant used
// add other drivers if necessary...

#ifdef Q_SPY
#error Simple Blinky Application does not support QP/Spy software tracing
#endif

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE

// Local-scope objects -------------------------------------------------------
constexpr std::uint32_t BSP_MCK    {1000000U};
constexpr std::uint32_t BSP_SMCLK  {1000000U};

constexpr std::uint32_t LED1       {1U << 0U};
constexpr std::uint32_t LED2       {1U << 7U};

constexpr std::uint32_t BTN_S1     {1U << 1U};

} // unnamed namespace

//============================================================================
// Error handler and ISRs...

extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the error handler is intended only
    // for debugging and MUST be changed for deployment of the application
    // (assuming that you ship your production code with assertions enabled).
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);

#ifndef NDEBUG
    P4OUT |=  LED2;  // turn LED2 on
    for (;;) { // for debugging, hang on in an endless loop...
    }
#endif

    WDTCTL = 0xDEAD;
    for (;;) { // explicitly "no-return"
    }
}
//............................................................................
// assertion failure handler for the startup code and libraries
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

// ISRs used in the application ==============================================

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    __interrupt void TIMER0_A0_ISR(void); // prototype
    #pragma vector=TIMER0_A0_VECTOR
    __interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
    __attribute__ ((interrupt(TIMER0_A0_VECTOR)))
    void TIMER0_A0_ISR(void)
#else
    #error MSP430 compiler not supported!
#endif
{
    QTimeEvt::TICK_X(0U, nullptr);  // time events at rate 0

#ifdef NDEBUG
    __low_power_mode_off_on_exit(); // turn the low-power mode OFF, NOTE1
#endif
}

} // extern "C"

//============================================================================
namespace BSP {

//..........................................................................
void init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // leave the MCK and SMCLK at default DCO setting

    // configure pins for LEDs
    P1DIR |= LED1;  // set LED1 pin to output
    P4DIR |= LED2;  // set LED2 pin to output

    // no software tracing -- no need to initialize and setup QS
    // no dynamic events -- no need to call QF_poolInit();
    // no publish-subscribe -- no need to call QActive_psInit();
}
//............................................................................
void ledOn() {
    P1OUT |=  LED1;
}
//............................................................................
void ledOff() {
    P1OUT &= ~LED1;
}
//............................................................................
void terminate(int16_t result) {
    Q_UNUSED_PAR(result);
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start AOs...
    static QP::QEvtPtr blinkyQueueSto[10];
    APP::AO_Blinky->start(
        1U,                      // QP prio. of the AO
        blinkyQueueSto,          // event queue storage
        Q_DIM(blinkyQueueSto),   // queue length [events]
        nullptr, 0U);            // no stack storage

    TA0CCTL0 = CCIE;  // CCR0 interrupt enabled
    TA0CCR0 = BSP_MCK / BSP::TICKS_PER_SEC;
    TA0CTL = TASSEL_2 | MC_1 | TACLR; // SMCLK, upmode, clear TAR
}
//............................................................................
void QF::onCleanup() {
}
//............................................................................
void QV::onIdle(void) { // CAUTION: called with interrupts DISABLED, see NOTE1
    // toggle LED2 on and then off, see NOTE2
    P4OUT |=  LED2;  // turn LED2 on
    P4OUT &= ~LED2;  // turn LED2 off

    // NOTE: interrupts still DISABLED...
#ifdef NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular MSP430 MCU.
    __low_power_mode_1(); // enter LPM1; also ENABLES interrupts, see NOTE1
#else
    QF_INT_ENABLE(); // just enable interrupts
#endif
}

} // namespace QP
//============================================================================
// NOTE1:
// With the cooperative QV kernel for MSP430, it is necessary to explicitly
// turn the low-power mode OFF in the interrupt, because the return
// from the interrupt will restore the CPU status register, which will
// re-enter the low-power mode. This, in turn, will prevent the QV event-loop
// from running.
//
// NOTE2:
// One of the LEDs is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
