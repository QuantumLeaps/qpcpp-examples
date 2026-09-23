#include "qpcpp.hpp"
#include "bsp.hpp"
#include "qhsmtst.hpp"

//Q_DEFINE_THIS_FILE

//............................................................................
void BSP::init(void const * const arg) {
    Q_UNUSED_PAR(arg);
    //QP::Qube::setAO(APP::AO_QHsmTst); // register the AO with Qube
}
//............................................................................
void BSP::exit() {
    QP::QF::stop();
}
//............................................................................
void BSP::display(char const * const msg) {
    // application-specific record
    QS_BEGIN_ID(QP::QS_USER, 0U)
        QS_STR(msg);
    QS_END()
}

//............................................................................
QP::QEvt const * QP::Qube::onGenEvt(QP::QSignal const sig,
                                    char const * const params)
{
    Q_UNUSED_PAR(params); // QHsmTst does not use params

    static QP::QEvt evt {0U};
    evt = QP::QEvt(sig); // immutable event with the given signal
    return &evt;
}
