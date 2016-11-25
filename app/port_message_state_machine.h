/*
* @file port_message_state_machine.h
* @brief port message state machine
* @ingroup port message state machine
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
#ifndef __PORT_MESSAGE_STATE_MACHINE_H__
#define __PORT_MESSAGE_STATE_MACHINE_H__

namespace ARCS {

class PortMsgStateMachine : public  QP::QMsm {
public:
    #define ETPORT_BUF_SIZE 1536
    /*! Extern port virtual table */
    TExternPortVtbl const *vptr;
    /*! Recieve Buffer */
    uint8_t recvBuf[ETPORT_BUF_SIZE];
    /*! ring buffer pointer of Port */
    TCharRingBuf *pRingBuf;
    PortMsgStateMachine();
protected:
    static QP::QMState const active_s;
    static QP::QMState const sending_s;
    static QP::QMState const idle_s;
    static QP::QMState const wait_s;
    static QP::QState active(PortMsgStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState initial(PortMsgStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState sending(PortMsgStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState sending_e(PortMsgStateMachine * const me);
    static QP::QState sending_x(PortMsgStateMachine * const me);
    static QP::QState idle(PortMsgStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState idle_e(PortMsgStateMachine * const me);
    static QP::QState idle_x(PortMsgStateMachine *const me);
    static QP::QState wait(PortMsgStateMachine * const me,
        QP::QEvt const * const e);
    static QP::QState wait_e(PortMsgStateMachine * const me);
    static QP::QState wait_x(PortMsgStateMachine * const me);
};

} /*  namespace ARCS*/

#endif /* __PORT_MESSAGE_STATE_MACHINE_H__ */

