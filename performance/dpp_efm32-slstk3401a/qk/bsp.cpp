//============================================================================
// Product: DPP example, EFM32-SLSTK3401A board, preemptive QK kernel
// Last updated for version 8.0.0
// Last updated on  2024-06-07
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005-2021 Quantum Leaps. All rights reserved.
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
#include "qpcpp.hpp"
#include "dpp.hpp"
#include "bsp.hpp"

#include "em_device.h"  // the device specific header (SiLabs)
#include "em_cmu.h"     // Clock Management Unit (SiLabs)
#include "em_gpio.h"    // GPIO (SiLabs)
#include "em_usart.h"   // USART (SiLabs)
// add other drivers if necessary...

Q_DEFINE_THIS_FILE

// namespace DPP *************************************************************
namespace DPP {

// Local-scope objects -------------------------------------------------------
#define LED_PORT    gpioPortF
#define LED0_PIN    4
#define LED1_PIN    5

#define PB_PORT     gpioPortF
#define PB0_PIN     6
#define PB1_PIN     7

static uint32_t l_rnd; // random seed

#ifdef Q_SPY

    QP::QSTimeCtr QS_tickTime_;
    QP::QSTimeCtr QS_tickPeriod_;

    // QS source IDs
    static QP::QSpyId const l_SysTick_Handler { 0U };
    static QP::QSpyId const l_GPIO_EVEN_IRQHandler { 0U };

    static USART_TypeDef * const l_USART0 = ((USART_TypeDef *)(0x40010000UL));

    enum AppRecords { // application-specific trace records
        PHILO_STAT = QP::QS_USER,
        COMMAND_STAT
    };

#endif

// ISRs used in this project =================================================
extern "C" {

//............................................................................
Q_NORETURN Q_onError(char const * const module, int_t const id) {
    //
    // NOTE: add here your application-specific error handling
    //
    (void)module;
    (void)id;
    QS_ASSERTION(module, id, 10000U);

#ifndef NDEBUG
    // light up both LEDs
    GPIO->P[LED_PORT].DOUT |= ((1U << LED0_PIN) | (1U << LED1_PIN));
    // for debugging, hang on in an endless loop until PB1 is pressed...
    while ((GPIO->P[PB_PORT].DIN & (1U << PB1_PIN)) != 0) {
    }
#endif

    NVIC_SystemReset();
}
//............................................................................
void assert_failed(char const * const module, int const id); // prototype
void assert_failed(char const * const module, int const id) {
    Q_onError(module, id);
}

//............................................................................
void SysTick_Handler(void); // prototype
void SysTick_Handler(void) {
    // state of the button debouncing, see below
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { 0U, 0U };
    uint32_t current;
    uint32_t tmp;

    QK_ISR_ENTRY();   // inform QK about entering an ISR

#ifdef Q_SPY
    {
        tmp = SysTick->CTRL; // clear SysTick_CTRL_COUNTFLAG
        QS_tickTime_ += QS_tickPeriod_; // account for the clock rollover
    }
#endif

    QP::QTimeEvt::TICK_X(0U, &l_SysTick_Handler); // process time events for rate 0

    // Perform the debouncing of buttons. The algorithm for debouncing
    // adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    // and Michael Barr, page 71.
    //
    current = ~GPIO->P[PB_PORT].DIN; // read PB0 and BP1
    tmp = buttons.depressed; // save the debounced depressed buttons
    buttons.depressed |= (buttons.previous & current); // set depressed
    buttons.depressed &= (buttons.previous | current); // clear released
    buttons.previous   = current; // update the history
    tmp ^= buttons.depressed;     // changed debounced depressed
    if ((tmp & (1U << PB0_PIN)) != 0U) {  // debounced PB0 state changed?
        if ((buttons.depressed & (1U << PB0_PIN)) != 0U) { // PB0 depressed?
            static QP::QEvt const pauseEvt(DPP::PAUSE_SIG);
            QP::QF::PUBLISH(&pauseEvt, &l_SysTick_Handler);
        }
        else {            // the button is released
            static QP::QEvt const serveEvt(DPP::SERVE_SIG);
            QP::QF::PUBLISH(&serveEvt, &l_SysTick_Handler);
        }
    }

    QK_ISR_EXIT();  // inform QK about exiting an ISR
}
//............................................................................
void GPIO_EVEN_IRQHandler(void);  // prototype
void GPIO_EVEN_IRQHandler(void) {
    QK_ISR_ENTRY(); // inform QK about entering an ISR

    // for testing...
    static QP::QEvt const tstEvt{ DPP::MAX_PUB_SIG };
    AO_Table->POST(&tstEvt, &l_GPIO_EVEN_IRQHandler);

    QK_ISR_EXIT();  // inform QK about exiting an ISR
}

//............................................................................
void USART0_RX_IRQHandler(void); // prototype
#ifdef Q_SPY
// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QF-unaware" meaning that it does not interact with
// the QF/QK and is not disabled. Such ISRs don't need to call QK_ISR_ENTRY/
// QK_ISR_EXIT and they cannot post or publish events.
//
void USART0_RX_IRQHandler(void) {
    // while RX FIFO NOT empty
    while ((DPP::l_USART0->STATUS & USART_STATUS_RXDATAV) != 0) {
        uint32_t b = DPP::l_USART0->RXDATA;
        QP::QS::rxPut(b);
    }
    QK_ARM_ERRATUM_838869();
}
#else
void USART0_RX_IRQHandler(void) {}
#endif // Q_SPY

} // extern "C"

// BSP functions =============================================================
void BSP::init(void) {
    // NOTE: SystemInit() already called from the startup code
    //  but SystemCoreClock needs to be updated
    //
    SystemCoreClockUpdate();

    // NOTE: The VFP (hardware Floating Point) unit is configured by QK

    // enable clock for to the peripherals used by this application...
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO,  true);
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO,  true);

    // configure the LEDs
    GPIO_PinModeSet(LED_PORT, LED0_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(LED_PORT, LED1_PIN, gpioModePushPull, 0);
    GPIO_PinOutClear(LED_PORT, LED0_PIN);
    GPIO_PinOutClear(LED_PORT, LED1_PIN);

    // configure the Buttons
    GPIO_PinModeSet(PB_PORT, PB0_PIN, gpioModeInputPull, 1);
    GPIO_PinModeSet(PB_PORT, PB1_PIN, gpioModeInputPull, 1);

    //...
    BSP::randomSeed(1234U);

    if (!QS_INIT(nullptr)) { // initialize the QS software tracing
        Q_ERROR();
    }
    QS_OBJ_DICTIONARY(&l_SysTick_Handler);
    QS_OBJ_DICTIONARY(&l_GPIO_EVEN_IRQHandler);
    QS_USR_DICTIONARY(PHILO_STAT);
    QS_USR_DICTIONARY(COMMAND_STAT);

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_SM_RECORDS); // state machine records
    QS_GLB_FILTER(QP::QS_AO_RECORDS); // active object records
    QS_GLB_FILTER(QP::QS_UA_RECORDS); // all user records
}
//............................................................................
void BSP::displayPhilStat(uint8_t n, char const *stat) {
    if (stat[0] == 'e') {
        GPIO->P[LED_PORT].DOUT |=  (1U << LED0_PIN);
    }
    else {
        GPIO->P[LED_PORT].DOUT &=  ~(1U << LED0_PIN);
    }

    QS_BEGIN_ID(PHILO_STAT, AO_Philo[n]->getPrio()) // app-specific record begin
        QS_U8(1, n);  // Philosopher number
        QS_STR(stat); // Philosopher status
    QS_END()
}
//............................................................................
void BSP::displayPaused(uint8_t paused) {
    if (paused != 0U) {
        GPIO->P[LED_PORT].DOUT |=  (1U << LED0_PIN);
    }
    else {
        GPIO->P[LED_PORT].DOUT &= ~(1U << LED0_PIN);
    }
}
//............................................................................
uint32_t BSP::random(void) { // a very cheap pseudo-random-number generator
    // lock the scheduler around l_rnd up to the (N_PHILO + 1U) ceiling
    QP::QSchedStatus lockStat = QP::QK::schedLock(N_PHILO + 1U);
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    //
    uint32_t rnd = l_rnd * (3U*7U*11U*13U*23U);
    l_rnd = rnd; // set for the next time
    QP::QK::schedUnlock(lockStat); // unlock sched after accessing l_rnd

    return (rnd >> 8);
}
//............................................................................
void BSP::randomSeed(uint32_t seed) {
    l_rnd = seed;
}

//............................................................................
void BSP::terminate(int16_t result) {
    (void)result;
}

} // namespace DPP


// namespace QP **************************************************************
namespace QP {

// QF callbacks ==============================================================
void QF::onStartup(void) {
    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    SysTick_Config(SystemCoreClock / DPP::BSP::TICKS_PER_SEC);

    // assign all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE1
    //
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    // DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    //
    NVIC_SetPriority(USART0_RX_IRQn, 0U); // kernel unaware interrupt
    NVIC_SetPriority(GPIO_EVEN_IRQn, QF_AWARE_ISR_CMSIS_PRI);
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 1U);
    // ...

    // enable IRQs...
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
#ifdef Q_SPY
    NVIC_EnableIRQ(USART0_RX_IRQn); // UART0 interrupt used for QS-RX
#endif
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QK::onIdle(void) {
    // toggle the User LED on and then off, see NOTE01
    QF_INT_DISABLE();
    GPIO->P[LED_PORT].DOUT |=  (1U << LED1_PIN);
    GPIO->P[LED_PORT].DOUT &= ~(1U << LED1_PIN);
    QF_INT_ENABLE();

#ifdef Q_SPY
    QS::rxParse();  // parse all the received bytes

    if ((DPP::l_USART0->STATUS & USART_STATUS_TXBL) != 0) { // is TXE empty?
        uint16_t b;

        QF_INT_DISABLE();
        b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  // not End-Of-Data?
            DPP::l_USART0->TXDATA = (b & 0xFFU); // put into the DR register
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M3 MCU.
    __WFI(); // Wait-For-Interrupt
#endif
}

// QS callbacks ==============================================================
#ifdef Q_SPY
//............................................................................
bool QS::onStartup(void const *arg) {
    static uint8_t qsTxBuf[2*1024]; // buffer for QS transmit channel
    static uint8_t qsRxBuf[100];    // buffer for QS receive channel
    static USART_InitAsync_TypeDef init = {
        usartEnable,      // Enable RX/TX when init completed
        0,                // Use current clock for configuring baudrate
        115200,           // 115200 bits/s
        usartOVS16,       // 16x oversampling
        usartDatabits8,   // 8 databits
        usartNoParity,    // No parity
        usartStopbits1,   // 1 stopbit
        0,                // Do not disable majority vote
        0,                // Not USART PRS input mode
        usartPrsRxCh0,    // PRS channel 0
        0,                // Auto CS functionality enable/disable switch
        0,                // Auto CS Hold cycles
        0                 // Auto CS Setup cycles
    };

    initBuf  (qsTxBuf, sizeof(qsTxBuf));
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    // Enable peripheral clocks
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    // To avoid false start, configure output as high
    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1); // TX pin
    GPIO_PinModeSet(gpioPortA, 1, gpioModeInput, 0);    // RX pin

    // Enable DK RS232/UART switch
    GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 1);
    CMU_ClockEnable(cmuClock_USART0, true);

    // configure the UART for the desired baud rate, 8-N-1 operation
    init.enable = usartDisable;
    USART_InitAsync(DPP::l_USART0, &init);

    // enable pins at correct UART/USART location.
    DPP::l_USART0->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
    DPP::l_USART0->ROUTELOC0 = (DPP::l_USART0->ROUTELOC0 &
                           ~(_USART_ROUTELOC0_TXLOC_MASK
                           | _USART_ROUTELOC0_RXLOC_MASK));

    // Clear previous RX interrupts
    USART_IntClear(DPP::l_USART0, USART_IF_RXDATAV);
    NVIC_ClearPendingIRQ(USART0_RX_IRQn);

    // Enable RX interrupts
    USART_IntEnable(DPP::l_USART0, USART_IF_RXDATAV);
    // NOTE: do not enable the UART0 interrupt in the NVIC yet.
    // Wait till QF::onStartup()


    // Finally enable the UART
    USART_Enable(DPP::l_USART0, usartEnable);

    DPP::QS_tickPeriod_ = SystemCoreClock / DPP::BSP::TICKS_PER_SEC;
    DPP::QS_tickTime_ = DPP::QS_tickPeriod_; // to start the timestamp at zero

    return true; // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {  // NOTE: invoked with interrupts DISABLED
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { // not set?
        return DPP::QS_tickTime_ - static_cast<QSTimeCtr>(SysTick->VAL);
    }
    else { // the rollover occurred, but the SysTick_ISR did not run yet
        return DPP::QS_tickTime_ + DPP::QS_tickPeriod_
               - static_cast<QSTimeCtr>(SysTick->VAL);
    }
}
//............................................................................
// NOTE:
// No critical section in QS::onFlush() to avoid nesting of critical sections
// in case QS::onFlush() is called from Q_onError().
void QS::onFlush(void) {
    for (;;) {
        std::uint16_t b = getByte();
        if (b != QS_EOD) {
            while ((DPP::l_USART0->STATUS & USART_STATUS_TXBL) == 0U) {
            }
            DPP::l_USART0->TXDATA = b;
        }
        else {
            break;
        }
    }
}
//............................................................................
void QS::onReset(void) {
    NVIC_SystemReset();
}
//............................................................................
void QS::onCommand(uint8_t cmdId, uint32_t param1,
                   uint32_t param2, uint32_t param3)
{
    (void)cmdId;
    (void)param1;
    (void)param2;
    (void)param3;

    QS_BEGIN_ID(DPP::COMMAND_STAT, 0U) // app-specific record
        QS_U8(2, cmdId);
        QS_U32(8, param1);
    QS_END()
}

#endif // Q_SPY
//----------------------------------------------------------------------------

} // namespace QP

//============================================================================
// NOTE1:
// The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
// ISR priority that is disabled by the QF framework. The value is suitable
// for the NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
// with the numerical values of priorities equal or higher than
// QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/QK_ISR_ENTRY
// macros or any other QF/QK  services. These ISRs are "QF-aware".
//
// Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
// level (i.e., with the numerical values of priorities less than
// QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
// Such "QF-unaware" ISRs cannot call any QF/QK services. In particular they
// can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
// by which a "QF-unaware" ISR can communicate with the QF framework is by
// triggering a "QF-aware" ISR, which can post/publish events.
//
// NOTE2:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//
