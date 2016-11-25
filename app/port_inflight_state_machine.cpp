/*
* @file port_inflight_state_machine.cpp
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
#include "port_inflight_state_machine.h"

namespace ARCS {

/*$ Local variable decleration..............................................*/
static PortInflightStateMachine l_portStateMachine[EXTERN_PORT_NUM];

/*$ state machine get instance..............................................*/
QP::QMsm* PortInflightStateMachine_getIns(uint8_t id) {
    Q_ASSERT(id < EXTERN_PORT_NUM);
    return &l_portStateMachine[id];
}
/*$ state machine set new port..............................................*/
void PortInflightStateMachine_setPortVtbl(TExternPortVtbl const *ptr,
    uin8_t id)
{
    Q_ASSERT(id < EXTERN_PORT_NUM);
    l_portStateMachine[id].vptr = ptr;
}
/*$ state machine set ring buffer...........................................*/
void PortInflightStateMachine_setRingBuf(TCharRingBuf *pRingBuf,
    uin8_t id)
{
    Q_ASSERT(id < EXTERN_PORT_NUM);
    l_portStateMachine[id].pRingBuf = pRingBuf;
}
/*$ Local variable decleration..............................................*/
QP::QMState const PortInflightStateMachine::active_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top)*/
    Q_STATE_CAST(&active), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
QP::QMState const PortInflightStateMachine::server_s = {
    &PortInflightStateMachine::active_s, /* superstate (top) */
    Q_STATE_CAST(&server), /* state handle */
    Q_STATE_CAST(&server_e), /* entry action */
    Q_STATE_CAST(&server_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};

/*$ PortInflightStateMachine::PortInflightStateMachine()....................*/
PortInflightStateMachine::PortInflightStateMachine(void)
  :  QMsm(Q_STATE_CAST(&PortInflightStateMachine::initial))
{
    vptr = (TExternPortVtbl *)0;
    pRingBuf = (TCharRingBuf *)0;
}
/*$ PortInflightStateMachine::active()......................................*/
QP::QState PortInflightStateMachine::active(
    PortInflightStateMachine * const me,
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
/*$ PortInflightStateMachine::initial().....................................*/
QP::QState PortInflightStateMachine::initial(
    PortInflightStateMachine * const me,
    QP::QEvt const * const e)
{
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &server_s,
        {
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };
    /* Here intial port */
    ExternPort_init(me->vptr);
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*$ PortInflightStateMachine::sending().....................................*/
QP::QState PortInflightStateMachine::server(PortInflightStateMachine * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case TRANSMIT_SIG: {
            TransmitEvt *pe = static_cast<TransmitEvt*>(e);
            ExternPort_send(vptr, pe->buf, (int)pe->datalen);
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
/*$ PortInflightStateMachine::sending_e()...................................*/
QP::QState PortInflightStateMachine::server_e(
    PortInflightStateMachine * const me)
{

}
/*$ PortInflightStateMachine::sending_x()...................................*/
QP::QState PortInflightStateMachine::server_x(
    PortInflightStateMachine * const me)
{

}

}/* namespace ARCS */
