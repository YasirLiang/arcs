/*
* @file
* @brief Requestor active object
* @ingroup Requestor active object
* @cond
******************************************************************************
* Build date on  2016-11-1
* Last updated for version 1.0.0
* Last updated on  2016-11-24
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#include "user.h"
#include "requestor.h"

namespace ARCS {

/*$ Local varailable--------------------------------------------------------*/
static Requestor l_rAct;

QP::QMState const Requestor::serving_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top) */
    Q_STATE_CAST(&serving), /* state handle active*/
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* no initial action */
};

QP::QMState const Requestor::active_s = {
    &Requestor::serving_s, /*superstae(serving_s)*/
    Q_STATE_CAST(&active), /* state handle active*/
    Q_ACTION_CAST(&active_e),/* entry action */
    Q_ACTION_CAST(&active_x),/* exit action */
    Q_ACTION_CAST(0) /* no initial action */
};

QP::QMState const Requestor::idle_s = {
    &Requestor::serving_s, /*superstae(serving_s)*/
    Q_STATE_CAST(&idle), /* state handle active*/
    Q_ACTION_CAST(&idle_e),/* entry action */
    Q_ACTION_CAST(&idle_x),/* exit action */
    Q_ACTION_CAST(0) /* no initial action */
};
/*get request instance......................................................*/
QP::QMsm *Requestor_getMsm(void) {
    return &l_rAct;
}
/*$ Requestor().............................................................*/
Requestor::Requestor()
  : QMsm(Q_STATE_CAST(&Requestor::initial))
{
    curReqElem = (TRequestElem *)0;
}
/*$ initial()...............................................................*/
QP::QState Requestor::initial(Requestor * const me,
        QP::QEvt const * const e)
{
    static QP::QMTranActTable const tatbl_ = { /* transition-action table */
        &active_s,
        {
            Q_ACTION_CAST(0)  /* zero terminator */
        }
    };
    /* avoid unused */
    (void)me;
    (void)e;
    return QM_TRAN_INIT(&tatbl_);
}
/*$ active()................................................................*/
QP::QState Requestor::active(Requestor * const me,
        QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case REQUEST_DONE_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const table_ = { /* transition-action table */
                &idle_s,
                {
                    Q_ACTION_CAST(&active_x), /* exit */
                    Q_ACTION_CAST(&idle_e), /* entry */
                    Q_ACTION_CAST(0)/* zero terminate */
                }
            };
            Q_ASSERT(curRequestId ==
                     static_cast<RequestDoneEvt const * const>(e)->rId);
            curReqElem = (TRequestElem *)0;
            /* change state to idle */
            status_ = QM_TRAN(&table_);
            break;
        }
        case REQUEST_ELEM_SIG: {
            status_ = QM_UNHANDLED();
            break;
        }
        default: {
            break;
        }
    }
    return status_;
}
/*$ active_e()..............................................................*/
QP::QState Requestor::active_e(Requestor * const me) {
    /* avoid unused */
    (void)me;
    return QM_ENTRY(&idle_s);
}
/*$ active_x()..............................................................*/
QP::QState Requestor::active_x(Requestor * const me) {
    /* avoid unused */
    (void)me;
    return QM_EXIT(&idle_s);
}
/*$ idle()..................................................................*/
QP::QState Requestor::idle(Requestor * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case REQUEST_ELEM_SIG: {
            /* get request elememt */
            curReqElem =
                (static_cast<RequestElemEvt const * const>(e))->pElem;
            Q_ASSERT(curReqElem != (TRequestElem *)0);
            curRequestId = curReqElem->id;
            /* run request function */
            Request_run(curReqElem);
            status_ = QM_HANDLED();
            break;
        }
        case REQUEST_DONE_SIG: {
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    /* avoid unused */
    (void)me;
    return status_;
}
/*$ idle_e()................................................................*/
QP::QState Requestor::idle_e(Requestor * const me) {
    /* avoid unused */
    (void)me;
    return QM_ENTRY(&idle_s);
}
/*$ idle_x()................................................................*/
QP::QState Requestor::idle_x(Requestor * const me) {
    /* avoid unused */
    (void)me;
    return QM_EXIT(&idle_s);
}
/*$ serving()...............................................................*/
QP::QState Requestor::serving(Requestor * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    /* avoid unused */
    (void)me;
    return status_;
}

} /* namespace ARCS */

