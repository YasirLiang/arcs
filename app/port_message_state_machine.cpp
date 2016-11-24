/*
* @file port_state_machine.cpp
* @brief port state machine
* @ingroup port state machine basing on send queue
* @cond
******************************************************************************
* Build Date on  2016-11-22
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
#include "port_state_machine.h"

namespace ARCS {

/*$ Local variable decleration..............................................*/
static PortMsgStateMachine l_portStateMachine[EXTERN_PORT_NUM];

/*$ state machine get instance..............................................*/
QP::QMsm* PortMsgStateMachine_getIns(uint8_t id) {
    Q_ASSERT(id < EXTERN_PORT_NUM);
    return &l_portStateMachine[id];
}
/*$ state machine set new port..............................................*/
void PortMsgStateMachine_setPortVtbl(TExternPortVtbl *ptr,
    uin8_t id)
{
    Q_ASSERT(id < EXTERN_PORT_NUM);
    l_portStateMachine[id].vptr = ptr;
}
/*$ state machine set ring buffer...........................................*/
void PortMsgStateMachine_setRingBuf(TCharRingBuf *pRingBuf,
    uin8_t id)
{
    Q_ASSERT(id < EXTERN_PORT_NUM);
    l_portStateMachine[id].pRingBuf = pRingBuf;
}
/*$ Local variable decleration..............................................*/
QP::QMState const PortMsgStateMachine::active_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top)*/
    Q_STATE_CAST(&active), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
QP::QMState const PortMsgStateMachine::sending_s = {
    &PortMsgStateMachine::active_s, /* superstate (top) */
    Q_STATE_CAST(&sending), /* state handle */
    Q_STATE_CAST(&sending_e), /* entry action */
    Q_STATE_CAST(&sending_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};
QP::QMState const PortMsgStateMachine::idle_s = {
    &PortMsgStateMachine::active_s,, /* superstate (top) */
    Q_STATE_CAST(&idle), /* state handle */
    Q_STATE_CAST(&idle_e), /* entry action */
    Q_STATE_CAST(&idle_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};
QP::QMState const PortMsgStateMachine::wait_s = {
    &PortMsgStateMachine::active_s,, /* superstate (top) */
    Q_STATE_CAST(&wait), /* state handle */
    Q_STATE_CAST(&wait_e), /* entry action */
    Q_STATE_CAST(&wait_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};
/*$ PortMsgStateMachine::PortMsgStateMachine()..............................*/
PortMsgStateMachine::PortMsgStateMachine(void)
  :  QMsm(Q_STATE_CAST(&PortMsgStateMachine::initial))
{
    vptr = (TExternPortVtbl *)0;
    pRingBuf = (TCharRingBuf *)0;
}
/*$ PortMsgStateMachine::active()...........................................*/
QP::QState PortMsgStateMachine::active(PortMsgStateMachine * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    int recvLen, pos; /* return varailable */
    switch (e->sig) {
        case PORTREADABLE_SIG: {
            MZR(recvBuf, PORT_BUF_SIZE);
            /* read data from readable port */
            recvLen = ExternPort_recv(vptr, recvBuf, PORT_BUF_SIZE);
            if (recvLen != -1) {/* No Error recv? */
                /* save messabe to ring buf */
                pos = 0;
                while (pos < recvLen) {
                    RingBuffer_saveChar(pRingBuf, recvBuf[pos++]);
                }
            }
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    /* return status */
    return status_;
}
/*$ PortMsgStateMachine::initial()..........................................*/
QP::QState PortMsgStateMachine::initial(PortMsgStateMachine * const me,
    QP::QEvt const * const e)
{
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &idle_s,
        {
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };
    /* Here intial port */
    ExternPort_init(me->vptr);
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*$ PortMsgStateMachine::sending()..........................................*/
QP::QState PortMsgStateMachine::sending(PortMsgStateMachine * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case : {
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }  
    }
    /* return status */
    return status_;
}
/*$ PortMsgStateMachine::sending_e()........................................*/
QP::QState PortMsgStateMachine::sending_e(PortMsgStateMachine * const me) {

}
/*$ PortMsgStateMachine::sending_x()........................................*/
QP::QState PortMsgStateMachine::sending_x(PortMsgStateMachine * const me) {

}
/*$ PortMsgStateMachine::idle().............................................*/
QP::QState PortMsgStateMachine::idle(PortMsgStateMachine * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case : {
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }  
    }
    /* return status */
    return status_;
}
/*$ PortMsgStateMachine::idle_e()...........................................*/
QP::QState PortMsgStateMachine::idle_e(PortMsgStateMachine * const me) {

}
/*$ PortMsgStateMachine::idle_x()...........................................*/
QP::QState PortMsgStateMachine::idle_x(PortMsgStateMachine *const me) {

}
/*$ PortMsgStateMachine::wait().............................................*/
QP::QState PortMsgStateMachine::wait(PortMsgStateMachine * const me,
    QP::QEvt const * const me)
{
    QP::QState status_;
    switch (e->sig) {
        case : {
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }  
    }
    /* return status */
    return status_;
}
/*$ PortMsgStateMachine::wait_e()...........................................*/
QP::QState PortMsgStateMachine::wait_e(PortMsgStateMachine * const me) {

}
/*$ PortMsgStateMachine::wait_x()...........................................*/
QP::QState PortMsgStateMachine::wait_x(PortMsgStateMachine * const me) {

}

}/* namespace ARCS */

