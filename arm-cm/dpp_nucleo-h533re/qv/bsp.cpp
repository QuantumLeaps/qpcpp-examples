//============================================================================
// BSP for QV kernel, NUCLEO-H533RE board
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: LicenseRef-QL-commercial
//
// This software is licensed under the terms of the Quantum Leaps commercial
// licenses. Please contact Quantum Leaps for more information about the
// available licensing options.
//
// RESTRICTIONS
// You may NOT:
// (a) redistribute, encumber, sell, rent, lease, sublicense, or otherwise
//     transfer rights in this software,
// (b) remove or alter any trademark, logo, copyright or other proprietary
//     notices, legends, symbols or labels present in this software,
// (c) plagiarize this software to sidestep the licensing obligations.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"      // QP/C++ real-time event framework
#include "bsp.hpp"        // Board Support Package
#include "app.hpp"        // Application

#include "stm32h533xx.h"  // CMSIS-compliant header file for the MCU used
// add other drivers if necessary...

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_MODULE("bsp")  // define the name of this file for assertions

// ROM configuration for the MCU
constexpr std::uint32_t ROM_START { 0x08000000 };
constexpr std::uint32_t ROM_SIZE  { 512*1024 };

// RAM configuration for the MCU
// NOTE; The following RAM constants lump all SRAM blocks into one 272K block.
// STM323H533RE suuports: SRAM1 (128K), SRAM2 (80K with ECC), SRAM3 (64K).
constexpr std::uint32_t RAM_START { 0x20000000 };
constexpr std::uint32_t RAM_SIZE  { 272*1024 };

// HSI CPU clock (64MHz) divisor (allowed 1U, 2U, 4U, 8U)
#define HSI_CLK_DIV 2U  // 64MHz/8 == 32MHz

// LED pins available on the board (just one user LED LD2--Green on PA.5)
constexpr std::uint32_t LD2_PIN {5U};

// Button pins available on the board (just one user Button B1 on PC.13)
constexpr std::uint32_t B1_PIN  {13U};

// macros from STM32Cube LL:
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
    WRITE_REG((REG), ((READ_REG(REG) & (~(CLEARMASK))) | (SETMASK)))


// Local objects -------------------------------------------------------------
// random seed shared among the Philo[] AOs
static std::uint32_t l_rnd; // random seed

#ifdef Q_SPY
    enum AppRecords { // application-specific trace records
        PHILO_STAT = QP::QS_USER,
        PAUSED_STAT,
    };

    // QSpy source IDs
    static QP::QSpyId const l_SysTick_Handler    { QP::QS_ID_AP };
    static QP::QSpyId const l_TIM2_Handler       { QP::QS_ID_AP + 1U };
    static QP::QSpyId const l_EXTI0_1_IRQHandler { QP::QS_ID_AP + 2U };
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
    // light up the user LED
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LD2 on
    for (;;) { // for debugging, hang on in an endless loop...
    }
#else
    NVIC_SystemReset();
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

// ISRs ======================================================================
void SysTick_Handler(void); // prototype
void SysTick_Handler(void) {

    QP::QTimeEvt::TICK_X(0U, &l_SysTick_Handler); // time events at rate 0

    // Perform the debouncing of buttons. The algorithm for debouncing
    // adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    // and Michael Barr, page 71.
    static struct {
        std::uint32_t depressed;
        std::uint32_t previous;
    } buttons = { 0U, 0U };

    std::uint32_t current = GPIOC->IDR; // read Port C with state of Button B1
    std::uint32_t tmp = buttons.depressed; // save the depressed buttons
    buttons.depressed |= (buttons.previous & current); // set depressed
    buttons.depressed &= (buttons.previous | current); // clear released
    buttons.previous   = current; // update the history
    tmp ^= buttons.depressed;     // changed debounced depressed
    current = buttons.depressed;

    if ((tmp & (1U << B1_PIN)) != 0U) { // debounced B1 state changed?
        if ((current & (1U << B1_PIN)) != 0U) { // is B1 depressed?
            static QP::QEvt const pauseEvt { APP::PAUSE_SIG };
            QP::QActive::PUBLISH(&pauseEvt, &l_SysTick_Handler);
        }
        else { // the button is released
            static QP::QEvt const serveEvt { APP::SERVE_SIG };
            QP::QActive::PUBLISH(&serveEvt, &l_SysTick_Handler);
        }
    }

    QV_ARM_ERRATUM_838869();
}
//............................................................................
void TIM2_IRQHandler(void);
void TIM2_IRQHandler(void) {
    QF_INT_DISABLE();
    TIM2->SR &= ~(TIM_SR_UIF); // clear the interrupt
    QF_INT_ENABLE();

    QP::QTimeEvt::TICK_X(1U, &l_TIM2_Handler); // time events at rate 0

    QV_ARM_ERRATUM_838869();
}
//............................................................................
// interrupt handler for testing preemptions
void EXTI0_IRQHandler(void); // prototype
void EXTI0_IRQHandler(void) {
    // for testing..
    static QP::QEvt const testEvt { APP::TEST_SIG };
    APP::AO_Table->POST(&testEvt, &l_EXTI0_1_IRQHandler);

    QV_ARM_ERRATUM_838869();
}

//............................................................................
#ifdef Q_SPY
// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QF-unaware" meaning that it does not interact with
// the QF/QK and is not disabled. Such ISRs don't need to call
// QK_ISR_ENTRY/QK_ISR_EXIT and they cannot post or publish events.

void USART2_IRQHandler(void); // prototype
void USART2_IRQHandler(void) { // used in QS-RX (kernel UNAWARE interrupt)
    // while Read Data Register or RX-FIFO Not Empty
    while ((USART2->ISR & USART_ISR_RXNE_Msk ) != 0U) {
        std::uint32_t const b = USART2->RDR;
        QP::QS::rxPut(b); // NOTE: allowed to run in kernel UNAWARE interrupt
    }

    QV_ARM_ERRATUM_838869();
}
#endif // Q_SPY

} // extern "C"

//============================================================================
namespace { // unnamed namespace

static void MPU_Config(); // prototype
static void MPU_Config() {
    // Set Attr 0
    ARM_MPU_SetMemAttr(0UL,
        ARM_MPU_ATTR(     // Normal memory
            // Outer Write-Through non-transient
            ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL),
            // Inner Write-Through non-transient
            ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL))
    );

    MPU->RNR = 0U; // region 0 (for ROM: read-only, can-execute)
    MPU->RBAR = ARM_MPU_RBAR(ROM_START,
        ARM_MPU_SH_NON,        // SH: Normal memory (not-shareable)
        1U,                    // RO: Normal memory, read-only
        0U,                    // NP: Normal memory, privileged access only
        0U);                   // XN: eXecute never (disabled)
    MPU->RLAR = ARM_MPU_RLAR(ROM_START + ROM_SIZE - 1U, 0U);

    MPU->RNR = 1U; // region 0 (for RAM1: read-write, execute-never)
    MPU->RBAR = ARM_MPU_RBAR(RAM_START,
        ARM_MPU_SH_OUTER,      // SH: Normal memory (outer shareable)
        0U,                    // RO: Normal memory, read/write
        0U,                    // NP: Normal memory, privileged access only
        1U);                   // XN: eXecute never
    MPU->RLAR = ARM_MPU_RLAR(RAM_START + RAM_SIZE - 1U, 0U);

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
static void SystemClock_Config(void); // prototype
static void SystemClock_Config(void) {
#if HSI_CLK_DIV == 1U
    uint32_t const rcc_cr_hisdiv = 0U;              // div by 1
    uint32_t const flash_latency = FLASH_ACR_LATENCY_3WS;
#elif HSI_CLK_DIV == 2U
    uint32_t const rcc_cr_hisdiv = RCC_CR_HSIDIV_0; // div by 2
    uint32_t const flash_latency = FLASH_ACR_LATENCY_1WS;
#elif HSI_CLK_DIV == 4U
    uint32_t const rcc_cr_hisdiv = RCC_CR_HSIDIV_1; // div by 4
    uint32_t const flash_latency = FLASH_ACR_LATENCY_0WS;
#elif HSI_CLK_DIV == 8U
    uint32_t const rcc_cr_hisdiv = RCC_CR_HSIDIV;   // div by 8
    uint32_t const flash_latency = FLASH_ACR_LATENCY_0WS;
#else
    error Unsupported HSI_CLK_DIV
#endif

    // Flash latency depends on the clock speed...
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, flash_latency);
    while (READ_BIT(FLASH->ACR, 0U)) {
    }

    MODIFY_REG(PWR->VOSCR, PWR_VOSCR_VOS, 0U);
    while (READ_BIT(PWR->VOSSR, PWR_VOSSR_VOSRDY) != PWR_VOSSR_VOSRDY) {
    }

    SET_BIT(RCC->CR, RCC_CR_HSION);
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) != RCC_CR_HSIRDY) {
    }

    MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSITRIM, 64U << RCC_HSICFGR_HSITRIM_Pos);
    MODIFY_REG(RCC->CR, RCC_CR_HSIDIV, rcc_cr_hisdiv);
    MODIFY_REG(RCC->CFGR1, RCC_CFGR1_SW, 0x0);
    while (READ_BIT(RCC->CFGR1, RCC_CFGR1_SWS) != 0U) {
    }

    MODIFY_REG(RCC->CFGR2, RCC_CFGR2_HPRE, 0U);
    MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE1, 0U);
    MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE2, 0U);
    MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE3, 0U);

    // update SystemCoreClock
    SystemCoreClockUpdate();
}
//..........................................................................
static void GPIOA_out(uint32_t const pin); // prototype
static void GPIOA_out(uint32_t const pin) {
    MODIFY_REG(GPIOA->OSPEEDR,
               GPIO_OSPEEDR_OSPEED0 << (pin * GPIO_OSPEEDR_OSPEED1_Pos),
               1U << (pin * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==1
    MODIFY_REG(GPIOA->OTYPER, 1U << pin,  0U << pin); // output
    MODIFY_REG(GPIOA->PUPDR,
               GPIO_PUPDR_PUPD0 << (pin * GPIO_PUPDR_PUPD1_Pos),
               0U << (pin * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL
    MODIFY_REG(GPIOA->MODER,
               GPIO_MODER_MODE0 << (pin * GPIO_MODER_MODE1_Pos),
               1U << (pin * GPIO_MODER_MODE1_Pos)); // MODE_1
}
//..........................................................................
static void GPIOC_inp(uint32_t const pin); // prototype
static void GPIOC_inp(uint32_t const pin) {
    uint32_t tmp;

    // configure IO Direction mode (input)
    tmp = GPIOC->MODER; //
    tmp &= ~(GPIO_MODER_MODE0 << (pin * GPIO_MODER_MODE1_Pos));
    GPIOC->MODER = tmp;

    // activate the Pull-up or Pull down resistor for the current IO
    tmp = GPIOC->PUPDR;
    tmp &= ~(GPIO_PUPDR_PUPD0 << (pin * GPIO_PUPDR_PUPD1_Pos));
    tmp |= (0x2U << (pin * GPIO_PUPDR_PUPD1_Pos));
    GPIOC->PUPDR = tmp;

    // EXTI mode configuration...
    tmp = EXTI->EXTICR[pin >> 2U];
    tmp &= ~((0x0FUL) << ((pin & 0x03U) * EXTI_EXTICR1_EXTI1_Pos));
    tmp |= (0x2 << ((pin & 0x03U) * EXTI_EXTICR1_EXTI1_Pos));
    EXTI->EXTICR[pin >> 2U] = tmp;

    // clear Rising Falling edge configuration
    tmp = EXTI->RTSR1;
    tmp &= ~((uint32_t)(1U << pin));
    tmp |= (1U << pin);
    EXTI->RTSR1 = tmp;

    tmp = EXTI->FTSR1;
    tmp &= ~((uint32_t)(1U << pin));
    EXTI->FTSR1 = tmp;

    // clear EXTI line configuration
    tmp = EXTI->EMR1;
    tmp &= ~((uint32_t)(1U << pin));
    EXTI->EMR1 = tmp;

    tmp = EXTI->IMR1;
    tmp &= ~((uint32_t)(1U << pin));
    tmp |= (1U << pin);
    EXTI->IMR1 = tmp;
}

} // unnamed namespace

namespace BSP {

//..........................................................................
void init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    // setup the MPU...
    MPU_Config();

    // initialize I-CACHE
    MODIFY_REG(ICACHE->CR, ICACHE_CR_WAYSEL, 0U); // 1-way
    SET_BIT(ICACHE->CR, ICACHE_CR_EN); // enable

    // flash prefetch buffer enable
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);

    // setup the clock
    SystemClock_Config();

    // NOTE: The VFP (hardware Floating Point) unit is configured by QK

    // enable GPIOA clock port for the LD2 LED
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);

    // set LD2 LED pin on GPIOA as output, no pull-up, pull-down
    GPIOA_out(LD2_PIN);

    // enable GPIOC clock port for the Button B1
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);

    // set Button B1 pin on GPIOC as input, no pull-up, pull-down
    GPIOC_inp(B1_PIN);

    // configure Timer2 for clock tick rate #1
    SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM2EN);

    // enable Timer2 for tick rate #1
    uint32_t tmp = TIM2->CR1;
    MODIFY_REG(tmp, TIM_CR1_DIR | TIM_CR1_CMS, 0U); // counter-mode=Up
    MODIFY_REG(tmp, TIM_CR1_CKD, 0U); // clock-division=1
    WRITE_REG(TIM2->CR1, tmp);

    WRITE_REG(TIM2->PSC, 0U); // prescaler=1
    SET_BIT(TIM2->EGR, TIM_EGR_UG); // update event to reload prescaler

    CLEAR_BIT(TIM2->CR1, TIM_CR1_ARPE); // disable ARR preload
    MODIFY_REG(TIM2->SMCR, TIM_SMCR_SMS | TIM_SMCR_ECE, 0U); // internal clock
    MODIFY_REG(TIM2->CR2, TIM_CR2_MMS, 0U); // reset timer synchronization
    CLEAR_BIT(TIM2->SMCR, TIM_SMCR_MSM); // disable master-slave

    // initialize QS software tracing...
    if (!QS_INIT(arg)) {
        Q_ERROR();
    }

    // QS dictionaries...
    QS_OBJ_DICTIONARY(&l_SysTick_Handler);
    QS_OBJ_DICTIONARY(&l_TIM2_Handler);
    QS_OBJ_DICTIONARY(&l_EXTI0_1_IRQHandler);
    QS_USR_DICTIONARY(PHILO_STAT);
    QS_USR_DICTIONARY(PAUSED_STAT);

    QS_ONLY(APP::produce_sig_dict());

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_GRP_ALL);  // enable all records
    QS_GLB_FILTER(-QP::QS_QF_TICK); // exclude the clock tick
    QS_LOC_FILTER(-(APP::N_PHILO + 3U)); // exclude prio. of AO_Ticker0

    // initialize event pools for mutable events
    static QF_MPOOL_EL(APP::TableEvt) smlPoolSto[2*APP::N_PHILO];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    randomSeed(1234U); // seed the random number generator

    // start AOs...
    static QP::QEvtPtr philoQueueSto[APP::N_PHILO][5];
    for (std::uint8_t n = 0U; n < APP::N_PHILO; ++n) {
        APP::AO_Philo[n]->start(
            n + 3U,                  // QF-prio.
            philoQueueSto[n],        // event queue storage
            Q_DIM(philoQueueSto[n]), // queue length [events]
            nullptr, 0U);            // no stack storage
    }

    static QP::QEvtPtr tableQueueSto[APP::N_PHILO];
    APP::AO_Table->start(
        APP::N_PHILO + 7U,       // QP prio. of the AO
        tableQueueSto,           // event queue storage
        Q_DIM(tableQueueSto),    // queue length [events]
        nullptr, 0U);            // no stack storage
}
//............................................................................
void terminate(std::int16_t const result) {
    Q_UNUSED_PAR(result);
    QP::QF::stop();
}
//............................................................................
void displayPhilStat(std::uint8_t const n, char const * const stat) {
    Q_UNUSED_PAR(n);

    if (stat[0] == 'e') {
        GPIOA->BSRR = (1U << LD2_PIN); // turn LED on
    }
    else {
        GPIOA->BRR  = (1U << LD2_PIN); // turn LED off
    }

    // application-specific trace record
    QS_BEGIN_ID(PHILO_STAT, APP::AO_Table->getPrio())
        QS_U8(1, n);  // Philosopher number
        QS_STR(stat); // Philosopher status
    QS_END()
}
//............................................................................
void displayPaused(std::uint8_t const paused) {
    // not enough LEDs to implement this feature
    if (paused != 0U) {
        //GPIOA->BSRR = (1U << LD2_PIN); // turn LED[n] on
    }
    else {
        //GPIOA->BRR  = (1U << LD2_PIN); // turn LED[n] off
    }

    // application-specific trace record
    QS_BEGIN_ID(PAUSED_STAT, APP::AO_Table->getPrio())
        QS_U8(1U, paused);  // Paused status
    QS_END()
}
//............................................................................
void randomSeed(std::uint32_t const seed) {
    l_rnd = seed;
}
//............................................................................
std::uint32_t random() { // a very cheap pseudo-random-number generator
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    std::uint32_t const rnd = l_rnd * (3U*7U*11U*13U*23U);
    l_rnd = rnd; // set for the next time

    return rnd >> 8U;
}
//............................................................................
void ledOn() {
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
}
//............................................................................
void ledOff() {
    GPIOA->BRR = (1U << LD2_PIN);  // turn LED off
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP::TICKS_PER_SEC);

    // set up the TIM2 timer to fire slighlty slower than BSP_TICKS_PER_SEC rate
    WRITE_REG(TIM2->ARR, SystemCoreClock / (BSP::TICKS_PER_SEC + 3U));
    SET_BIT(TIM2->DIER, TIM_DIER_UIE); // enable timer interrupts
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);

    // assign all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE1
    NVIC_SetPriority(USART2_IRQn,    0U); // kernel UNAWARE interrupt
    NVIC_SetPriority(EXTI0_IRQn,     QF_AWARE_ISR_CMSIS_PRI + 0U);
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 1U);
    NVIC_SetPriority(TIM2_IRQn,      QF_AWARE_ISR_CMSIS_PRI + 2U);
    // ...

    // enable IRQs...
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);

#ifdef Q_SPY
    NVIC_EnableIRQ(USART2_IRQn); // UART1 interrupt used for QS-RX
#endif
}
//............................................................................
void QF::onCleanup() {
    QS_EXIT();
}

//............................................................................
void QV::onIdle() { // CAUTION: called with interrupts DISABLED, see NOTE0
    // toggle an LED on and then off (not enough LEDs, see NOTE2)
    //GPIOA->BSRR = (1U << LD2_PIN); // turn LED[n] on
    //GPIOA->BRR  = (1U << LD2_PIN); // turn LED[n] off

#ifdef Q_SPY
    QF_INT_ENABLE();

    // Some floating point code is to exercise the VFP...
    float volatile x = 1.73205F;
    x = x * 1.73205F;
    Q_ASSERT_ID(300, 2.999F < x);

    QS::rxParse();  // parse all the received bytes

    // while Transmit Data Register Empty or TX-FIFO Not Full
    while ((USART2->ISR & USART_ISR_TXE_TXFNF_Msk) != 0U) {
        QF_INT_DISABLE();
        std::uint16_t const b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {   // not End-Of-Data?
            USART2->TDR = b; // put into the DR register
        }
        else {
            break; // done for now
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    QV_CPU_SLEEP(); // atomically go to sleep and enable interrupts
#else
    QF_INT_ENABLE(); // just enable interrupts
#endif
}

//============================================================================
// QS callbacks...
#ifdef Q_SPY

//............................................................................
static constexpr std::uint16_t QS_UARTPrescTable[12] = {
    1U, 2U, 4U, 6U, 8U, 10U, 12U, 16U, 32U, 64U, 128U, 256U
};

#define __LL_USART_DIV_SAMPLING16(__PERIPHCLK__, __PRESCALER__, __BAUDRATE__) \
  ((((__PERIPHCLK__)/(USART_PRESCALER_TAB[(__PRESCALER__)]))\
    + ((__BAUDRATE__)/2U))/(__BAUDRATE__))

#define QS_UART_DIV_SAMPLING16(__PCLK__, __BAUD__, __CLOCKPRESCALER__) \
  ((((__PCLK__)/QS_UARTPrescTable[(__CLOCKPRESCALER__)]) \
  + ((__BAUD__)/2U)) / (__BAUD__))

// USART1 pins PA.2 and PA.3
constexpr std::uint32_t USART2_TX_PIN {2U};
constexpr std::uint32_t USART2_RX_PIN {3U};

//............................................................................
bool QS::onStartup(void const *arg) {
    Q_UNUSED_PAR(arg);

    static std::uint8_t qsTxBuf[2*1024]; // buffer for QS-TX channel
    initBuf(qsTxBuf, sizeof(qsTxBuf));

    static std::uint8_t qsRxBuf[100];    // buffer for QS-RX channel
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    // enable peripheral clock for USART2 and its pins
    SET_BIT(RCC->APB1LENR, RCC_APB1LENR_USART2EN); // USART2 clock
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN); // GPIOA clock for pins

    // Configure GPIOA to USART2_TX and USART2_RX pins.........................
    std::uint32_t tmp;

    // configure GPIOA to USART2_TX ...........................................
    // configure alternate function
    tmp = GPIOA->AFR[USART2_TX_PIN >> 3U];
    tmp &= ~(0x0FUL << ((USART2_TX_PIN & 0x07U) * GPIO_AFRL_AFSEL1_Pos));
    tmp |= (7U << ((USART2_TX_PIN & 0x07U) * GPIO_AFRL_AFSEL1_Pos));
    GPIOA->AFR[USART2_TX_PIN >> 3U] = tmp; // 0x700

    // configure IO Direction mode (Input, Output, Alternate or Analog)
    tmp = GPIOA->MODER;
    tmp &= ~(GPIO_MODER_MODE0 << (USART2_TX_PIN * GPIO_MODER_MODE1_Pos));
    tmp |= (2U << (USART2_TX_PIN * GPIO_MODER_MODE1_Pos));
    GPIOA->MODER = tmp;

    // configure the IO Speed
    tmp = GPIOA->OSPEEDR;
    tmp &= ~(GPIO_OSPEEDR_OSPEED0 << (USART2_TX_PIN * GPIO_OSPEEDR_OSPEED1_Pos));
    tmp |= (2U << (USART2_TX_PIN * GPIO_OSPEEDR_OSPEED1_Pos));
    GPIOA->OSPEEDR = tmp;

    // configure the IO Output Type
    tmp = GPIOA->OTYPER;
    tmp &= ~(GPIO_OTYPER_OT0 << USART2_TX_PIN) ;
    GPIOA->OTYPER = tmp;

    // activate the Pull-up or Pull down resistor for the current IO
    tmp = GPIOA->PUPDR;
    tmp &= ~(GPIO_PUPDR_PUPD0 << (USART2_TX_PIN * GPIO_PUPDR_PUPD1_Pos));
    tmp |= (1U << (USART2_TX_PIN * GPIO_PUPDR_PUPD1_Pos));
    GPIOA->PUPDR = tmp;

    // configure GPIOA to USART2_RX ...........................................

    // configure alternate function
    tmp = GPIOA->AFR[USART2_RX_PIN >> 3U];
    tmp &= ~(0xFU << ((USART2_RX_PIN & 0x07U) * GPIO_AFRL_AFSEL1_Pos));
    tmp |= (7U << ((USART2_RX_PIN & 0x7U) * GPIO_AFRL_AFSEL1_Pos));
    GPIOA->AFR[USART2_RX_PIN >> 3U] = tmp;

    // configure IO Direction mode (Input, Output, Alternate or Analog)
    tmp = GPIOA->MODER;
    tmp &= ~(GPIO_MODER_MODE0 << (USART2_RX_PIN * GPIO_MODER_MODE1_Pos));
    tmp |= (2U << (USART2_RX_PIN * GPIO_MODER_MODE1_Pos));
    GPIOA->MODER = tmp;

    // configure the IO Speed
    tmp = GPIOA->OSPEEDR;
    tmp &= ~(GPIO_OSPEEDR_OSPEED0 << (USART2_RX_PIN * GPIO_OSPEEDR_OSPEED1_Pos));
    tmp |= (2U << (USART2_RX_PIN * GPIO_OSPEEDR_OSPEED1_Pos));
    GPIOA->OSPEEDR = tmp;

    // configure the IO Output Type
    tmp = GPIOA->OTYPER;
    tmp &= ~(GPIO_OTYPER_OT0 << USART2_RX_PIN) ;
    GPIOA->OTYPER = tmp;

    // activate the Pull-up or Pull down resistor for the current IO
    tmp = GPIOA->PUPDR;
    tmp &= ~(GPIO_PUPDR_PUPD0 << (USART2_RX_PIN * GPIO_PUPDR_PUPD1_Pos));
    tmp |= (1U << (USART2_RX_PIN * GPIO_PUPDR_PUPD1_Pos));
    GPIOA->PUPDR = tmp;


    // configure USART2..............................................
    MODIFY_REG(USART2->CR1,
        (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS |
         USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
        0U  | // data==8bits
        0U  | // parity==none
        12U | // direction==TX & RX
        0U);  // oversampling==16
    MODIFY_REG(USART2->CR2,
        USART_CR2_STOP,
        0U);  // stop-bits==1
    MODIFY_REG(USART2->CR3,
        USART_CR3_RTSE | USART_CR3_CTSE,
        0U);  // hardware-flow=NO

    // baud rate
    USART2->BRR = QS_UART_DIV_SAMPLING16(
                       SystemCoreClock, // USART2 clock
                       115200U,         // baud rate
                       0U);             // prescaler

    MODIFY_REG(USART2->PRESC,
        USART_PRESC_PRESCALER,
        0U); // prescaler=DIV1

    // FIFO thresholds
    MODIFY_REG(USART2->CR3,
        USART_CR3_TXFTCFG,
        0U << USART_CR3_TXFTCFG_Pos); // TX-FIFO=threshold-1
    MODIFY_REG(USART2->CR3,
        USART_CR3_RXFTCFG,
        0U << USART_CR3_RXFTCFG_Pos); // RX-FIFO=threshold-1

    SET_BIT(USART2->CR1, USART_CR1_FIFOEN);  // enable FIFO

    // asynch mode
    CLEAR_BIT(USART2->CR2, USART_CR2_LINEN | USART_CR2_CLKEN);
    CLEAR_BIT(USART2->CR3, USART_CR3_SCEN | USART_CR3_IREN | USART_CR3_HDSEL);

    // RXNE and RX FIFO Not Empty Interrupt Enable
    SET_BIT(USART2->CR1, USART_CR1_RXNEIE_Msk);

    SET_BIT(USART2->CR1, USART_CR1_UE); // enable USART

    // configure Timer5 for delivering QS time-stamp...........................
    SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM5EN);

    tmp = TIM5->CR1;
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
void QS::onCleanup() {
}
//............................................................................
QSTimeCtr QS::onGetTime() { // NOTE: invoked with interrupts DISABLED
    return TIM5->CNT; // 32-bit Timer5 count
}
//............................................................................
// NOTE:
// No critical section in QS::onFlush() to avoid nesting of critical sections
// in case QS::onFlush() is called from Q_onError().
void QS::onFlush() {
    for (;;) {
        std::uint16_t const b = getByte();
        if (b != QS_EOD) {
            while ((USART2->ISR & USART_ISR_TXE_Msk) == 0U) { // TXE not empty
            }
            USART2->TDR = b;
        }
        else {
            break;
        }
    }
}
//............................................................................
void QS::onReset() {
    NVIC_SystemReset();
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
// NOTE1:
// The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
// ISR priority that is disabled by the QF framework. The value is suitable
// for the NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
// with the numerical values of priorities equal or higher than
// QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/
// QK_ISR_ENTRY macros or any other QF/QK services. These ISRs are
// "QF-aware".
//
// Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
// level (i.e., with the numerical values of priorities less than
// QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
// Such "QF-unaware" ISRs cannot call ANY QF/QK services. In particular they
// can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
// by which a "QF-unaware" ISR can communicate with the QF framework is by
// triggering a "QF-aware" ISR, which can post/publish events.
//
// NOTE2:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//
