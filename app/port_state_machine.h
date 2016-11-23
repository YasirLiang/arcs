/*
* @file port_state_machine.h
* @brief port state machine
* @ingroup port state machine
* @cond
******************************************************************************
* Build Date on  2016-11-22
* Last updated for version 1.0.0
* Last updated on  2016-11-22
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __PORT_STATE_MACHINE_H__
#define __PORT_STATE_MACHINE_H__

namespace ARCS {

class PortStateMachine : public  QP::QMsm {
private:
    #define PORT_BUF_SIZE 1536
    /*! Extern port virtual table */
    TExternPortVtbl const *vptr;
    /*! Recieve Buffer */
    uint8_t recvBuf[PORT_BUF_SIZE];
    /*! ring buffer pointer of Port */
    TCharRingBuf *pRingBuf;
public:
    PortStateMachine();
protected:
    static QP::QMState const active_s;
    static QP::QMState const sending_s;
    static QP::QMState const idle_s;
    static QP::QMState const wait_s;
    static QP::QState active(PortStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState initial(PortStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState sending(PortStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState sending_e(PortStateMachine * const me);
    static QP::QState sending_x(PortStateMachine * const me);
    static QP::QState idle(PortStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState idle_e(PortStateMachine * const me);
    static QP::QState idle_x(PortStateMachine *const me);
    static QP::QState wait(PortStateMachine * const me,
        QP::QEvt const * const me);
    static QP::QState wait_e(PortStateMachine * const me);
    static QP::QState wait_x(PortStateMachine * const me);
};

} /*  namespace ARCS*/

#endif /* __REQUEST_PORT_H__ */

