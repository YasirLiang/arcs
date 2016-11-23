/*
* @file
* @brief transmitor active object
* @ingroup transmitor active object
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-11-23
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
/*Including file------------------------------------------------------------*/
#include "transmitor.h"
/*$ Qt Tcp socket get Vtbl..................................................*/
TExternPortVtbl const * QtTcpSocket_getVtbl(void);
namespace ARCS {

/*Local variable------------------------------------------------------------*/
QP::QMState const Transmitor::active_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top) */
    Q_STATE_CAST(&active), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};

/*Local variable------------------------------------------------------------*/
QP::QMState const Transmitor::serving_s = {
    &Transmitor::active_s, /* superstate (top) */
    Q_STATE_CAST(&serving), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
/*Local variable------------------------------------------------------------*/
static Transmitor l_transmitor;
/*Global variable-----------------------------------------------------------*/
QP::GuiQMActive A0_Transmitor = &l_transmitor;
/*$ Transmitor::Transmitor()................................................*/
Transmitor::Transmitor()
    : GuiQMActive(Q_STATE_CAST(&Transmitor::initial))
{
    if (!initialRecvBufList()) {
        Q_ASSERT(0); /* assert failed */
    }
    /* set information */
    for (uin8_t i = 0; i < EXTERN_PORT_NUM; ++i) {
        /* initial ring buffer */
        RingBuffer_initial(&ringBuf[i],
            recvBuf[i].bufSize, recvBuf[i].pBuf);
        /* set StateMachine ring buffer,
            ring buffer must be initial */
        PortStateMachine_setRingBuf(&ringBuf[i], i);
        /* get port state machine msm */
        port[i] = PortStateMachine_getIns(i);
    }
    
    /* set port state machine virtual table */
    PortStateMachine_setPortVtbl(QtTcpSocket_getVtbl(),
            QT_SV_EP);
}
/*$ Transmitor::active()....................................................*/
bool Transmitor::initialRecvBufList(void) {
    /* new space and initial
        for transmitor port recv buffer list */
    uint8_t *ptr;
    bool bret = (bool)0;
    /* for qt port transmitor recv buffer */
    ptr = (uint8_t *)malloc(QT_ET_RECV_BUF_SIZE);
    if (ptr != (uint8_t *0)) {
        recvBuf[QT_SV_EP].pBuf = ptr;
        recvBuf[QT_SV_EP].bufSize = QT_ET_RECV_BUF_SIZE;
        bret = (bool)1;
    }
    /* initial other recv buffer */
    if (bret) {

    }
    /* return successfully flag */
    return bret;
}
/*$ Transmitor::active()....................................................*/
QP::QState Transmitor::active(Transmitor * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case TICK_1MS_SIG: {
            status_ = QM_HANDLE();
            break;
        }
        default: {
            break;
        }
    }
}
/*$ Transmitor::active()....................................................*/
QP::QState Transmitor::initial(Transmitor * const me,
        QP::QEvt const * const e)
{
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &serving_s,
        {
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };

    /* initial port state machine msm */
    for (uin8_t i = 0; i < EXTERN_PORT_NUM; ++i) {
        port[i]->init();
    }
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*$ Transmitor::serving()...................................................*/
QP::QState Transmitor::serving(Transmitor * const me,
        QP::QEvt const * const e)
{
}
/*$ Transmitor::serving_e().................................................*/
QP::QState Transmitor::serving_e(Transmitor * const me) {

}
/*$ Transmitor::serving_x().................................................*/
QP::QState Transmitor::serving_x(Transmitor * const me) {

}

}/* namespace ARCS */

