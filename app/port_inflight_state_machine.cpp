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
#include "user.h"
#include "port_inflight_state_machine.h"

namespace ARCS {

/*$ Local variable decleration..............................................*/
static PortInflightStateMachine l_portStateMachine[EXTERN_PORT_NUM];
/* helper Function for self Number of me */
static inline uint8_t PORT_ID(PortInflightStateMachine const * const me) {
    return static_cast<uint8_t>(me - l_portStateMachine);
}
/*$ state machine get instance..............................................*/
QP::QMsm* PortInflightStateMachine_getIns(uint8_t id) {
    Q_ASSERT(id < EXTERN_PORT_NUM);
    return &l_portStateMachine[id];
}
/*$ state machine set new port..............................................*/
void PortInflightStateMachine_setPortVtbl(TExternPortVtbl const *ptr,
    uint8_t id)
{
    Q_ASSERT((ptr != (TExternPortVtbl const *)0)
                        && (id < EXTERN_PORT_NUM));
    l_portStateMachine[id].vptr = ptr;
    qDebug("PortStateMachine[%d] Set Vtable success", id);
}
/*$ state machine set new port..............................................*/
void PortInflightStateMachine_setMePortVtbl(TExternPortVtbl const *ptr,
    QP::QMsm * const me)
{
    uint8_t n;
    Q_ASSERT((ptr != (TExternPortVtbl const *)0)
                        && (me != (QP::QMsm * const)0));
    n = PORT_ID(static_cast<PortInflightStateMachine * const>(me));
    Q_ASSERT(n < EXTERN_PORT_NUM);
    l_portStateMachine[n].vptr = ptr;
    qDebug("PortStateMachine[%d] Set Me Vtable success", n);
}
/*$ state machine set ring buffer...........................................*/
void PortInflightStateMachine_setRingBuf(TCharRingBuf *pRingBuf,
    uint8_t id)
{
    Q_ASSERT((pRingBuf != (TCharRingBuf *)0)
                        && (id < EXTERN_PORT_NUM));
    l_portStateMachine[id].pRingBuf = pRingBuf;
    qDebug("PortStateMachine[%d] setRingBuf success", id);
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
    Q_ACTION_CAST(&server_e), /* entry action */
    Q_ACTION_CAST(&server_x), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
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
            MZR(me->recvBuf, PORT_ISM_BUF_SIZE);
            /* read data from readable port */
            recvLen = ExternPort_recv(me->vptr,
                me->recvBuf, PORT_ISM_BUF_SIZE);
            if (recvLen != -1) {/* No Error recv? */
                /* save messabe to ring buf */
                pos = 0;
                while (pos < recvLen) {
                    RingBuffer_saveChar(me->pRingBuf,
                        me->recvBuf[pos++]);
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
    Q_ASSERT(me->vptr != (TExternPortVtbl const *)0);
    ExternPort_init(me->vptr);
    /* avoid unused */
    (void)e;
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
            TransmitEvt const * const pe =
                static_cast<TransmitEvt const * const>(e);
            ExternPort_send(me->vptr,
                pe->buf, (int)pe->datalen);
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
    /* avoid unused */
    (void)me;
    return QM_ENTRY(&server_s);
}
/*$ PortInflightStateMachine::sending_x()...................................*/
QP::QState PortInflightStateMachine::server_x(
    PortInflightStateMachine * const me)
{
    /* avoid unused */
    (void)me;
    return QM_EXIT(&server_s);
}

}/* namespace ARCS */

