//============================================================================
// Product: "Blinky" on LAUCHXL2-TMS570LS12 board, preemptive QK kernel
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

#include "sys_common.h"
#include "sys_core.h"
#include "sys_vim.h"
#include "system.h"
#include "gio.h"
#include "rti.h"
#include "het.h"
#include "sci.h"
// add other drivers if necessary...

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE  // file name for assertions

// Local objects -------------------------------------------------------------
constexpr std::uint32_t LED2_PIN  {1U};
constexpr gioPORT_t    *LED2_PORT {gioPORTB};

constexpr std::uint32_t LED3_PIN  {2U};
constexpr gioPORT_t    *LED3_PORT {gioPORTB};

// NOTE: Switch-A is multiplexed on the same port/pin as LED3,
// so you can use one or the other but not both simultaneously.
//
//constexpr std::uint32_t SWA_PIN   {2U};
//constexpr gioPORT_t    *SWA_PORT  {gioPORTB};

constexpr std::uint32_t SWB_PIN   {15U};
constexpr hetBASE_t    *SWB_PORT  {hetREG1};

constexpr t_isrFuncPTR *VIM_RAM   {reinterpret_cast<t_isrFuncPTR *>(0xFFF82000U)};

// Local-scope objects -----------------------------------------------------

#ifdef Q_SPY
    enum AppRecords { // application-specific trace records
        LED_STAT = QP::QS_USER,
    };

    // QSpy source IDs...
    static QP::QSpyId const l_rtiCompare0 = { QP::QS_ID_AP };
    static QP::QSpyId const l_ssiTest = { QP::QS_ID_AP + 1U };
#endif // Q_SPY

} // unnamed namespace

//============================================================================
// Error handler

extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the error handler is intended only
    // for debugging and MUST be changed for deployment of the application.
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U); // report assertion to QS

#ifndef NDEBUG
    for (;;) { // for debugging, hang on in an endless loop...
    }
#else
    systemREG1->SYSECR = 0; // perform system reset
    for (;;) { // explicitly "no-return"
    }
#endif
}
//............................................................................
// assertion failure handler for the startup code and libraries
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

// ISRs used in the application ==============================================
// CAUTION: ISRs MUST be both __stackless and __arm!
QK_IRQ_BEGIN(rtiCompare0)
    rtiREG1->INTFLAG = 1U;    // clear the interrutp source
    QP::QTimeEvt::TICK_X(0U, &l_rtiCompare0); // time events at rate 0
QK_IRQ_END()

//............................................................................
QK_IRQ_BEGIN(ssiTest)  // System Software Interrupt for testing
    systemREG1->SSIF = 0x01; // clear the SSI0 source
    // for testing...
    static QP::QEvt const tstEvt{ APP::TIMEOUT_SIG };
    APP::AO_Blinky->POST(&tstEvt, &l_ssiTest);
QK_IRQ_END()

//............................................................................
#ifdef Q_SPY
// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QP-unaware" meaning that it does not interact with
// the QP and is not disabled. Such ISRs don't need to be defined with
// QK_IRQ_BEGIN()/QK_IRQ_END().

#if defined __IAR_SYSTEMS_ICC__
    FIQ
#elif defined __TI_ARM__
    #pragma CODE_STATE(32)
    #pragma INTERRUPT(FIQ)
#else
    #error Unsupported compiler
#endif
void sciHighLevel(void) {
    uint32_t vec = scilinREG->INTVECT0;
    if (vec == 11U) { // SCI receive interrupt
        std::uint32_t b = scilinREG->RD;
        QP::QS::rxPut(b);
    }
}
#endif // Q_SPY

} // extern "C"

//============================================================================
namespace BSP {

//............................................................................
void init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    // configure the LEDs
    gioInit();
    LED2_PORT->DIR |= (1U << LED2_PIN); // set as output
    LED3_PORT->DIR |= (1U << LED3_PIN); // set as output

    // configure the Buttons
    SWB_PORT->DIR  &= (1U << SWB_PIN);    // set as input

    // initialize QS software tracing...
    if (!QS_INIT(arg)) {
        Q_ERROR();
    }

    // QS dictionaries...
    QS_OBJ_DICTIONARY(&l_rtiCompare0);
    QS_OBJ_DICTIONARY(&l_ssiTest);
    QS_SIG_DICTIONARY(APP::TIMEOUT_SIG, nullptr);
    QS_USR_DICTIONARY(LED_STAT);

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_GRP_ALL);  // enable all QS trace records
    QS_GLB_FILTER(-QP::QS_QF_TICK); // exclude the tick record

    // mutable events not used -- no need to call QP::QF::poolInit()
    // publish-subscribe not used -- no need to call QP::QActive::psInit()
}
//............................................................................
void ledOn() {
    LED2_PORT->DSET = (1U << LED2_PIN);
    // application-specific trace record
    QS_BEGIN_ID(LED_STAT, APP::AO_Blinky->getPrio())
        QS_STR("ON"); // LED status
    QS_END()
}
//............................................................................
void ledOff() {
    LED2_PORT->DCLR = (1U << LED2_PIN);
    // application-specific trace record
    QS_BEGIN_ID(LED_STAT, APP::AO_Blinky->getPrio())
        QS_STR("OFF"); // LED status
    QS_END()
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start AOs...
    static QEvtPtr blinkyQueueSto[10];
    APP::AO_Blinky->start(
        1U,                    // QP prio. of the AO
        blinkyQueueSto,        // event queue storage
        Q_DIM(blinkyQueueSto), // queue length [events]
        nullptr, 0U);          // no stack storage

    rtiInit(); // configure RTI with UC counter of 7
    rtiSetPeriod(rtiCOUNTER_BLOCK0,
        static_cast<std::uint32_t>((RTI_FREQ*1E6/(7+1))/BSP::TICKS_PER_SEC));
    rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
    rtiStartCounter(rtiCOUNTER_BLOCK0);

    VIM_RAM[2 + 1] = (t_isrFuncPTR)&rtiCompare0; // install the IRQ
    vimREG->FIRQPR0 &= ~(1U << 2U);   // designate interrupt as IRQ, NOTE0
    vimREG->REQMASKSET0 = (1U << 2U); // enable RTI interrupt

    VIM_RAM[21 + 1] = (t_isrFuncPTR)&ssiTest ; // install the IRQ
    vimREG->FIRQPR0 &= ~(1U << 21);   // designate interrupt as IRQ, NOTE0
    vimREG->REQMASKSET0 = (1U << 21); // enable interrupt

    QF_INT_ENABLE_ALL(); // enable all interrupts (IRQ and FIQ)
}
//............................................................................
void QF::onCleanup() {
    QS_EXIT();
}
//............................................................................
void QK::onIdle() {
    // toggle User LED on and then off, see NOTE01
    QF_INT_DISABLE();
    LED3_PORT->DSET = (1U << LED3_PIN);
    LED3_PORT->DCLR = (1U << LED3_PIN);
    QF_INT_ENABLE();

#ifdef Q_SPY
    QS::rxParse();  // parse all the received bytes

    //if (sciIsTxReady(scilinREG)) {
    if ((scilinREG->FLR & (uint32)SCI_TX_INT) != 0U) { // is TX empty?
        QF_INT_DISABLE();
        std::uint16_t b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  // not End-Of-Data?
            //sciSendByte(scilinREG, b);
            scilinREG->TD = b; // put into the TD register
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-R MCU.
    _gotoCPUIdle_(); // wait for interrupt
#endif
}

//============================================================================
// QS callbacks...
#ifdef Q_SPY

//............................................................................
bool QS::onStartup(void const *arg) {
    Q_UNUSED_PAR(arg);

    static std::uint8_t qsTxBuf[2*1024]; // buffer for QS-TX channel
    initBuf(qsTxBuf, sizeof(qsTxBuf));

    static std::uint8_t qsRxBuf[100];    // buffer for QS-RX channel
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    sciInit();
    VIM_RAM[13 + 1] = (t_isrFuncPTR)&sciHighLevel; // install the ISR
    vimREG->FIRQPR0 |= (1U << 13U);   // designate interrupt as FIQ
    vimREG->REQMASKSET0 = (1U << 13U); // enable interrupt

    return 1U; // return success
}
//............................................................................
void QS::onCleanup() {
}
//............................................................................
QSTimeCtr QS::onGetTime() { // NOTE: invoked with interrupts DISABLED
    return rtiREG1->CNT[0].FRCx; // free running RTI counter0
}
//............................................................................
// NOTE:
// No critical section in QS_onFlush() to avoid nesting of critical sections
// in case QS::onFlush() is called from Q_onError().
void QS::onFlush() {
    for (;;) {
        std::uint16_t b = getByte();
        if (b != QS_EOD) {
            while ((scilinREG->FLR & (uint32)SCI_TX_INT) == 0U) {
            }
            scilinREG->TD = b;
        }
        else {
            break;
        }
    }
}
//............................................................................
void QS::onReset() {
    systemREG1->SYSECR = 0U; // perform system reset
}
//............................................................................
void QS::onCommand(std::uint8_t cmdId, std::uint32_t param1,
    std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}

#endif // Q_SPY

} // namespace QP

//============================================================================
// NOTE0:
// The FIQ-type interrupts are never disabled in this QP port, therefore
// they can always preempt any code, including the IRQ-handlers (ISRs).
// Therefore, FIQ-type interrupts are "kernel-unaware" and must NEVER call
// any QP services, such as posting events.
//
// NOTE1:
// One of the LEDs is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
