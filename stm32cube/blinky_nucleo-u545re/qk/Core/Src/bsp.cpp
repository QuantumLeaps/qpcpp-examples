//============================================================================
// Blinky example, NUCLEO-U545RE-Q board, QK kernel
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
#include "qpcpp.hpp"             // QP/C++ real-time embedded framework
#include "blinky.hpp"            // Blinky Application interface
#include "bsp.hpp"               // Board Support Package

#include "stm32u545xx.h"  // CMSIS-compliant header file for the MCU used
// add other drivers if necessary...

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE

// Local-scope objects -------------------------------------------------------
// LED pins available on the board (just one user LED LD2--Green on PA.5)
constexpr std::uint32_t LD2_PIN     {5U};

// Button pins available on the board (just one user Button B1 on PC.13)
constexpr std::uint32_t B1_PIN      {13U};

// Local-scope objects -----------------------------------------------------
#ifdef Q_SPY

// QSpy source IDs
static QP::QSpyId const l_SysTick_Handler = { 0U };

#endif

} // unnamed local namespace

// Local-scope defines -------------------------------------------------------
// macros from STM32Cube LL:
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
    WRITE_REG((REG), ((READ_REG(REG) & (~(CLEARMASK))) | (SETMASK)))

//============================================================================
// Error handler and ISRs...

extern "C" {

int QP_run(void) { // QP framework entry point (from "C")
    QP::QF::init();         // initialize the framework
    BSP::init();            // initialize the BSP
    BSP::start();           // start the AOs/Threads
    return QP::QF::run();   // run the QF application
}

//............................................................................
Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the error handler is intended only
    // for debugging and MUST be changed for deployment of the application
    // (assuming that you ship your production code with assertions enabled).
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U); // report assertion to QS

#ifndef NDEBUG
    // light up the user LED
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
    // for debugging, hang on in an endless loop...
    for (;;) {
    }
#endif
    NVIC_SystemReset();
    for (;;) { // explicitly "no-return"
    }
}
//............................................................................
// assertion failure handler for the STM32 library, including the startup code
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

// ISRs used in the application ==============================================

void SysTick_Handler(void); // prototype
void SysTick_Handler(void) {
    QK_ISR_ENTRY();   // inform QK about entering an ISR

    QP::QTimeEvt::TICK_X(0U, &l_SysTick_Handler); // time events at rate 0

    QK_ISR_EXIT();  // inform QK about exiting an ISR
}

} // extern "C"

//============================================================================
namespace BSP {

static void STM32U545RE_MPU_setup(void) {
    // Set Attr 0
    ARM_MPU_SetMemAttr(0UL,
        ARM_MPU_ATTR(     // Normal memory
            // Outer Write-Through non-transient
            ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL),
            // Inner Write-Through non-transient
            ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL))
    );

    MPU->RNR = 0U; // region 0 (for ROM: read-only, can-execute)
    MPU->RBAR = ARM_MPU_RBAR(0x08000000U,
        ARM_MPU_SH_NON,        // SH: Normal memory (not-shareable)
        1U,                    // RO: Normal memory, read-only
        0U,                    // NP: Normal memory, privileged access only
        0U);                   // XN: eXecute never (disabled)
    MPU->RLAR = ARM_MPU_RLAR(0x0807FFFFU, 0U);

    MPU->RNR = 1U; // region 0 (for RAM1: read-write, execute-never)
    MPU->RBAR = ARM_MPU_RBAR(0x20000000U,
        ARM_MPU_SH_OUTER,      // SH: Normal memory (outer shareable)
        0U,                    // RO: Normal memory, read/write
        0U,                    // NP: Normal memory, privileged access only
        1U);                   // XN: eXecute never
    MPU->RLAR = ARM_MPU_RLAR(0x2003FFFFU, 0U);

    MPU->RNR = 2U; // region 0 (for RAM2: read-write, execute-never)
    MPU->RBAR = ARM_MPU_RBAR(0x28000000U,
        ARM_MPU_SH_OUTER,      // SH: Normal memory (outer shareable)
        0U,                    // RO: Normal memory, read/write
        0U,                    // NP: Normal memory, privileged access only
        1U);                   // XN: eXecute never
    MPU->RLAR = ARM_MPU_RLAR(0x28003FFFU, 0U);

    // Enable MPU with all region definitions
    __DMB();
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk
                | MPU_CTRL_HFNMIENA_Msk
                | MPU_CTRL_ENABLE_Msk;

    // Enable MemManage Faults
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    __DSB();
    __ISB();
}
//..........................................................................
void init() {
    // setup the MPU...
    STM32U545RE_MPU_setup();

    // initialize I-CACHE
    MODIFY_REG(ICACHE->CR, ICACHE_CR_WAYSEL, 0U); // 1-way
    SET_BIT(ICACHE->CR, ICACHE_CR_EN); // enable

    // flash prefetch buffer enable
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);

    // enable PWR clock interface
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_PWREN);

    // NOTE: SystemInit() has been already called from the startup code
    // but SystemCoreClock needs to be updated
    SystemCoreClockUpdate();

    // NOTE: The VFP (hardware Floating Point) unit is configured by QK

    // enable GPIOA clock port for the LED LD4
    RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;

    // set all used GPIOA pins as push-pull output, no pull-up, pull-down
    MODIFY_REG(GPIOA->OSPEEDR,
               GPIO_OSPEEDR_OSPEED0 << (LD2_PIN * GPIO_OSPEEDR_OSPEED1_Pos),
               1U << (LD2_PIN * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==1
    MODIFY_REG(GPIOA->OTYPER,
               1U << LD2_PIN,
               0U << LD2_PIN); // output
    MODIFY_REG(GPIOA->PUPDR,
               GPIO_PUPDR_PUPD0 << (LD2_PIN * GPIO_PUPDR_PUPD1_Pos),
               0U << (LD2_PIN * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL
    MODIFY_REG(GPIOA->MODER,
               GPIO_MODER_MODE0 << (LD2_PIN * GPIO_MODER_MODE1_Pos),
               1U << (LD2_PIN * GPIO_MODER_MODE1_Pos)); // MODE_1

    // enable GPIOC clock port for the Button B1
    RCC->AHB2ENR1 |=  RCC_AHB2ENR1_GPIOCEN;

    // configure Button B1 pin on GPIOC as input, no pull-up, pull-down
    MODIFY_REG(GPIOC->PUPDR,
               GPIO_PUPDR_PUPD0 << (B1_PIN * GPIO_PUPDR_PUPD1_Pos),
               0U << (B1_PIN * GPIO_PUPDR_PUPD1_Pos)); // NO PULL
    MODIFY_REG(GPIOC->MODER,
               GPIO_MODER_MODE0 << (B1_PIN * GPIO_MODER_MODE1_Pos),
               0U << (B1_PIN * GPIO_MODER_MODE1_Pos)); // MODE_0

    // initialize the QS software tracing...
    if (!QS_INIT(nullptr)) {
        Q_ERROR();
    }

    // dictionaries...
    QS_OBJ_DICTIONARY(&l_SysTick_Handler);

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_ALL_RECORDS);   // all records
    QS_GLB_FILTER(-QP::QS_QF_TICK);      // exclude the clock tick
}
//............................................................................
void start() {
    // initialize event pools
    static QF_MPOOL_EL(QP::QEvt) smlPoolSto[20];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // start Active Objects...

    static QP::QEvtPtr blinkyQueueSto[5];
    APP::AO_Blinky->start(
        1U,                         // QP prio. of the AO
        blinkyQueueSto,              // event queue storage
        Q_DIM(blinkyQueueSto),       // queue length [events]
        nullptr, 0U);                // no stack storage
}
//............................................................................
void ledOn() {
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
}
//............................................................................
void ledOff() {
    GPIOA->BRR = (1U << LD2_PIN);  // turn LED off
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
    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    SysTick_Config(SystemCoreClock / BSP::TICKS_PER_SEC);

    // assign all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE1
    NVIC_SetPriority(USART1_IRQn,    0U); // kernel UNAWARE interrupt
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 1U);
    // ...

    // enable IRQs...
#ifdef Q_SPY
    NVIC_EnableIRQ(USART1_IRQn); // UART1 interrupt used for QS-RX
#endif
}
//............................................................................
void QF::onCleanup() {
}
//............................................................................
void QK::onIdle() {
    // toggle an LED on and then off (not enough LEDs, see NOTE2)
    //QF_INT_DISABLE();
    //GPIOA->BSRR = (1U << LD2_PIN); // turn LED[n] on
    //GPIOA->BRR  = (1U << LD2_PIN); // turn LED[n] off
    //QF_INT_ENABLE();

#ifdef Q_SPY
    QS::rxParse();  // parse all the received bytes

    // while Transmit Data Register Empty or TX-FIFO Not Full
    while ((USART1->ISR & USART_ISR_TXE_TXFNF_Msk) != 0U) {
        QF_INT_DISABLE();
        std::uint16_t b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {   // not End-Of-Data?
            USART1->TDR = b; // put into the DR register
        }
        else {
            break; // done for now
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    __WFI(); // Wait-For-Interrupt
#endif
}

//============================================================================
// QS callbacks...
#ifdef Q_SPY
namespace QS {

//............................................................................
static std::uint16_t const QS_UARTPrescTable[12] = {
    1U, 2U, 4U, 6U, 8U, 10U, 12U, 16U, 32U, 64U, 128U, 256U
};

#define __LL_USART_DIV_SAMPLING16(__PERIPHCLK__, __PRESCALER__, __BAUDRATE__) \
  ((((__PERIPHCLK__)/(USART_PRESCALER_TAB[(__PRESCALER__)]))\
    + ((__BAUDRATE__)/2U))/(__BAUDRATE__))

#define QS_UART_DIV_SAMPLING16(__PCLK__, __BAUD__, __CLOCKPRESCALER__) \
  ((((__PCLK__)/QS_UARTPrescTable[(__CLOCKPRESCALER__)]) \
  + ((__BAUD__)/2U)) / (__BAUD__))

// USART1 pins PA.9 and PA.10
constexpr std::uint32_t USART1_TX_PIN {9U};
constexpr std::uint32_t USART1_RX_PIN {10U};

//............................................................................
bool onStartup(void const *arg) {
    Q_UNUSED_PAR(arg);

    static std::uint8_t qsTxBuf[2*1024]; // buffer for QS-TX channel
    initBuf(qsTxBuf, sizeof(qsTxBuf));

    static std::uint8_t qsRxBuf[100];    // buffer for QS-RX channel
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    // enable peripheral clock for USART1 and its pins
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN); // UART1 clock
    SET_BIT(RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOAEN); // GPIOA clock for pins

    // Configure GPIOA to USART1_RX and USART1_TX pins.........................
    MODIFY_REG(GPIOA->OSPEEDR,
        GPIO_OSPEEDR_OSPEED0 << (USART1_TX_PIN * GPIO_OSPEEDR_OSPEED1_Pos),
        0U << (USART1_TX_PIN * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==0
    MODIFY_REG(GPIOA->OSPEEDR,
        GPIO_OSPEEDR_OSPEED0 << (USART1_RX_PIN * GPIO_OSPEEDR_OSPEED1_Pos),
        0U << (USART1_RX_PIN * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==0

    MODIFY_REG(GPIOA->OTYPER,
        (1U << USART1_TX_PIN) | (1U << USART1_RX_PIN),
        ((1U << USART1_TX_PIN) | (1U << USART1_RX_PIN) * 0U)); // output

    MODIFY_REG(GPIOA->PUPDR,
        GPIO_PUPDR_PUPD0 << (USART1_TX_PIN * GPIO_PUPDR_PUPD1_Pos),
        0U << (USART1_TX_PIN * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL
    MODIFY_REG(GPIOA->PUPDR,
        GPIO_PUPDR_PUPD0 << (USART1_RX_PIN * GPIO_PUPDR_PUPD1_Pos),
        0U << (USART1_RX_PIN * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL

    MODIFY_REG(GPIOA->AFR[1],
        GPIO_AFRH_AFSEL8 << ((USART1_TX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos),
        7U << ((USART1_TX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos)); // AF_7
    MODIFY_REG(GPIOA->AFR[1],
        GPIO_AFRH_AFSEL8 << ((USART1_RX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos),
        7U << ((USART1_RX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos)); // AF_7

    MODIFY_REG(GPIOA->MODER,
        GPIO_MODER_MODE0 << (USART1_TX_PIN * GPIO_MODER_MODE1_Pos),
        2U << (USART1_TX_PIN * GPIO_MODER_MODE1_Pos)); // MODE_2
    MODIFY_REG(GPIOA->MODER,
        GPIO_MODER_MODE0 << (USART1_RX_PIN * GPIO_MODER_MODE1_Pos),
        2U << (USART1_RX_PIN * GPIO_MODER_MODE1_Pos)); // MODE_2

    // configure USART1..............................................
    MODIFY_REG(USART1->CR1,
        (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS |
         USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
        0U  | // data==8bits
        0U  | // parity==none
        12U | // direction==TX & RX
        0U);  // oversampling==16
    MODIFY_REG(USART1->CR2,
        USART_CR2_STOP,
        0U);  // stop-bits==1
    MODIFY_REG(USART1->CR3,
        USART_CR3_RTSE | USART_CR3_CTSE,
        0U);  // hardware-flow=NO

    // baud rate
    USART1->BRR = QS_UART_DIV_SAMPLING16(
                       SystemCoreClock, // USART1 clock
                       115200U,         // baud rate
                       0U);             // prescaler

    MODIFY_REG(USART1->PRESC,
        USART_PRESC_PRESCALER,
        0U); // prescaler=DIV1

    // FIFO thresholds
    MODIFY_REG(USART1->CR3,
        USART_CR3_TXFTCFG,
        0U << USART_CR3_TXFTCFG_Pos); // TX-FIFO=threshold-1
    MODIFY_REG(USART1->CR3,
        USART_CR3_RXFTCFG,
        0U << USART_CR3_RXFTCFG_Pos); // RX-FIFO=threshold-1

    SET_BIT(USART1->CR1, USART_CR1_FIFOEN);  // enable FIFO

    // asynch mode
    CLEAR_BIT(USART1->CR2, USART_CR2_LINEN | USART_CR2_CLKEN);
    CLEAR_BIT(USART1->CR3, USART_CR3_SCEN | USART_CR3_IREN | USART_CR3_HDSEL);

    // RXNE and RX FIFO Not Empty Interrupt Enable
    SET_BIT(USART1->CR1, USART_CR1_RXNEIE_Msk);

    SET_BIT(USART1->CR1, USART_CR1_UE); // enable USART

    // configure Timer5 for delivering QS time-stamp...........................
    SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN);

    uint32_t tmp = TIM5->CR1;
    MODIFY_REG(tmp, TIM_CR1_DIR | TIM_CR1_CMS, 0U); // counter-mode=Up
    MODIFY_REG(tmp, TIM_CR1_CKD, 0U); // clock-division=1
    WRITE_REG(TIM5->CR1, tmp);

    WRITE_REG(TIM5->ARR, 0xFFFFFFFFU); // auto-reload
    WRITE_REG(TIM5->PSC, 0U); // prescaler=1
    SET_BIT(TIM5->EGR, TIM_EGR_UG); // update event to reload prescaler

    CLEAR_BIT(TIM5->CR1, TIM_CR1_ARPE); // disable ARR preload
    MODIFY_REG(TIM5->SMCR, TIM_SMCR_SMS | TIM_SMCR_ECE, 0U); // internal clock
    MODIFY_REG(TIM5->CR2, TIM_CR2_MMS, 0U); // reset timer synchronization
    CLEAR_BIT(TIM5->SMCR, TIM_SMCR_MSM); // disable master-slave

    // enable Timer5 to start time stamp
    SET_BIT(TIM5->CR1, TIM_CR1_CEN);

    return true; // return success
}
//............................................................................
void onCleanup() {
}
//............................................................................
QSTimeCtr onGetTime() { // NOTE: invoked with interrupts DISABLED
    return TIM5->CNT; // 32-bit Timer5 count
}
//............................................................................
// NOTE:
// No critical section in QS::onFlush() to avoid nesting of critical sections
// in case QS::onFlush() is called from Q_onError().
void onFlush() {
    for (;;) {
        std::uint16_t b = getByte();
        if (b != QS_EOD) {
            while ((USART1->ISR & USART_ISR_TXE_Msk) == 0U) { // TXE not empty
            }
            USART1->TDR = b;
        }
        else {
            break;
        }
    }
}
//............................................................................
void onReset() {
    NVIC_SystemReset();
}
//............................................................................
void onCommand(std::uint8_t cmdId, std::uint32_t param1,
               std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}

} // namespace QS

#endif // Q_SPY

} // namespace QP

//----------------------------------------------------------------------------
#ifdef Q_SPY

extern "C" {
//............................................................................
// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QF-unaware" meaning that it does not interact with
// the QF/QXK and is not disabled. Such ISRs don't need to call
// QK_ISR_ENTRY/QK_ISR_EXIT and they cannot post or publish events.

void USART1_IRQHandler(void); // prototype
void USART1_IRQHandler(void) { // used in QS-RX (kernel UNAWARE interrupt)
    // while Read Data Register or RX-FIFO Not Empty
    while ((USART1->ISR & USART_ISR_RXNE_Msk ) != 0U) {
        std::uint8_t b = USART1->RDR;
        QP::QS::rxPut(b);
    }

    QK_ARM_ERRATUM_838869();
}

} // extern "C"

#endif // Q_SPY

//============================================================================
// NOTE1:
// The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
// ISR priority that is disabled by the QF framework. The value is suitable
// for the NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
// with the numerical values of priorities equal or higher than
// QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
// are "QF-aware".
//
// Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
// level (i.e., with the numerical values of priorities less than
// QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
// Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
// by which a "QF-unaware" ISR can communicate with the QF framework is by
// triggering a "QF-aware" ISR, which can post/publish events.
//
// NOTE2:
// One of the LEDs is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invocations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//
