/*
* @file port_state_machine.cpp
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
#include "port_state_machine.h"

namespace ARCS {

/*$ Local variable decleration..............................................*/
PortStateMachine l_portStateMachine[EXTERN_PORT_NUM];

/*$ state machine get instance..............................................*/
QP::QMsm* PortStateMachine_getIns(uint8_t id) {
    Q_ASSERT(id < EXTERN_PORT_NUM);
    return &l_portStateMachine[id];
}
/*$ state machine set new port..............................................*/
void PortStateMachine_setPortVtbl(TExternPortVtbl *ptr,
    uin8_t id)
{
    Q_ASSERT(id < EXTERN_PORT_NUM);
    l_portStateMachine[id].vptr = ptr;
}
/*$ Local variable decleration..............................................*/
QP::QMState const PortStateMachine::active_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top)*/
    Q_STATE_CAST(&active), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
QP::QMState const PortStateMachine::sending_s = {
    &PortStateMachine::active_s, /* superstate (top) */
    Q_STATE_CAST(&sending), /* state handle */
    Q_STATE_CAST(&sending_e), /* entry action */
    Q_STATE_CAST(&sending_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};
QP::QMState const PortStateMachine::idle_s = {
    &PortStateMachine::active_s,, /* superstate (top) */
    Q_STATE_CAST(&idle), /* state handle */
    Q_STATE_CAST(&idle_e), /* entry action */
    Q_STATE_CAST(&idle_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};
QP::QMState const PortStateMachine::wait_s = {
    &PortStateMachine::active_s,, /* superstate (top) */
    Q_STATE_CAST(&wait), /* state handle */
    Q_STATE_CAST(&wait_e), /* entry action */
    Q_STATE_CAST(&wait_x), /* exit action */
    Q_STATE_CAST(0) /* initial action */
};
/*$ PortStateMachine::PortStateMachine()....................................*/
PortStateMachine::PortStateMachine(void)
  :  QMsm(Q_STATE_CAST(&PortStateMachine::initial))
{
    vptr = (TExternPortVtbl *)0;
}
/*$ PortStateMachine::active()..............................................*/
QP::QState active(PortStateMachine * const me,
    QP::QEvt const * const e)
{
}
/*$ PortStateMachine::initial().............................................*/
QP::QState initial(PortStateMachine * const me,
    QP::QEvt const * const e)
{
    /* Here intial port */
    ExternPort_init(me->vptr);
}
/*$ PortStateMachine::sending().............................................*/
QP::QState sending(PortStateMachine * const me,
    QP::QEvt const * const e)
{
}
/*$ PortStateMachine::sending_e()...........................................*/
QP::QState sending_e(PortStateMachine * const me) {

}
/*$ PortStateMachine::sending_x()...........................................*/
QP::QState sending_x(PortStateMachine * const me) {

}
/*$ PortStateMachine::idle()................................................*/
QP::QState idle(PortStateMachine * const me,
    QP::QEvt const * const e)
{
}
/*$ PortStateMachine::idle_e()..............................................*/
QP::QState idle_e(PortStateMachine * const me) {

}
/*$ PortStateMachine::idle_x()..............................................*/
QP::QState idle_x(PortStateMachine *const me) {

}
/*$ PortStateMachine::wait()................................................*/
QP::QState wait(PortStateMachine * const me,
    QP::QEvt const * const me)
{

}
/*$ PortStateMachine::wait_e()..............................................*/
QP::QState wait_e(PortStateMachine * const me) {

}
/*$ PortStateMachine::wait_x()..............................................*/
QP::QState wait_x(PortStateMachine * const me) {

}

}/* namespace ARCS */

