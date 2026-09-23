#include "qpcpp.hpp"
#include "bsp.hpp"
#include "qhsmtst.hpp"

//Q_DEFINE_THIS_FILE

//............................................................................
int main() {
    using namespace APP; // for the plain names in the QS dictionaries

    QP::QF::init(); // initialize the framework and the underlying RT kernel
    BSP::init(nullptr); // initialize the Board Support Package

    // QS dictionaries...
    QS_SIG_DICTIONARY(A_SIG, nullptr);
    QS_SIG_DICTIONARY(B_SIG, nullptr);
    QS_SIG_DICTIONARY(C_SIG, nullptr);
    QS_SIG_DICTIONARY(D_SIG, nullptr);
    QS_SIG_DICTIONARY(E_SIG, nullptr);
    QS_SIG_DICTIONARY(F_SIG, nullptr);
    QS_SIG_DICTIONARY(G_SIG, nullptr);
    QS_SIG_DICTIONARY(H_SIG, nullptr);
    QS_SIG_DICTIONARY(I_SIG, nullptr);
    QS_SIG_DICTIONARY(X_SIG, nullptr);
    QS_OBJ_DICTIONARY(AO_QHsmTst);

    // QS filters...
    QS_GLB_FILTER(QP::QS_GRP_ALL);

    // publish-subscribe not used, no call to QActive::psInit()
    // dynamic event allocation not used, no call to QF::poolInit()

    // instantiate and start the active objects...
    static QP::QEvtPtr qhsmtstQSto[10]; // event queue storage
    AO_QHsmTst->start(
        2U,                  // QF-priority
        qhsmtstQSto,         // storage for the AO's queue
        Q_DIM(qhsmtstQSto),  // length of the queue [entries]
        nullptr,             // stack storage (not used)
        0U,                  // stack size [bytes] (not used)
        nullptr);            // initial param (or 0)

    return QP::QF::run(); // run the QF application
}
