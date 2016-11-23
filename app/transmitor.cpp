/*
* @file
* @brief transmitor active object
* @ingroup transmitor active object
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-11-21
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
    /* get port state machine msm */
    for (uin8_t i = 0; i < EXTERN_PORT_NUM; ++i) {
        /* set msm */
        port[i] = PortStateMachine_getIns(i);
    }
    /* set port state machine virtual table */
    PortStateMachine_setPortVtbl(QtTcpSocket_getVtbl(),
            QT_SV_EP);
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
/*$ Transmitor::serving_e()................................................*/
QP::QState Transmitor::serving_e(Transmitor * const me) {

}
/*$ Transmitor::serving_x().................................................*/
QP::QState Transmitor::serving_x(Transmitor * const me) {

}

}/* namespace ARCS */

