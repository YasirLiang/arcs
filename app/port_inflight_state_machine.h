/*
* @file port_inflight_state_machine.h
* @brief port inflight state machine
* @ingroup port state machine
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
#ifndef __PORT_INFLIGHT_STATE_MACHINE_H__
#define __PORT_INFLIGHT_STATE_MACHINE_H__

namespace ARCS {

class PortInflightStateMachine : public  QP::QMsm {
public:
    /*! port inflight state machine buffer size */
    #define PORT_ISM_BUF_SIZE 1536
    /*! Recieve Buffer */
    uint8_t recvBuf[PORT_ISM_BUF_SIZE];
    /*! ring buffer pointer of Port */
    TCharRingBuf *pRingBuf;
    /*! Extern port virtual table */
    TExternPortVtbl const *vptr;
public:
    PortInflightStateMachine();
protected:
    static QP::QMState const active_s;
    static QP::QMState const server_s;
    static QP::QState active(PortInflightStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState initial(PortInflightStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState server(PortInflightStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState server_e(PortInflightStateMachine * const me);
    static QP::QState server_x(PortInflightStateMachine * const me);
};

} /*  namespace ARCS*/

#endif /* __PORT_INFLIGHT_STATE_MACHINE_H__ */

