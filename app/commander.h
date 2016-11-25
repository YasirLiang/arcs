/*
* @file
* @brief Commander QMsm object
* @ingroup RequestProccessor active object
* @cond
******************************************************************************
* Last updated for version 1.0.0
* Last updated on  2016-10-20
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __COMMANDER_H__
#define __COMMANDER_H__

namespace ARCS {

class Commander : public QP::QMsm {
private:
    static QP::QMsm *reQ; /*! requster Qmsm */
    static enum TCmdQExeState {
        CQE_SUCCESS, /*! all command excute success and done */
        CQE_DOING /*! last command executing is in progress */
    }qCmdExeStatus; /*!queue command execute status */
public:
    static uint32_t commandId; /*! allot for user or system command */
    static uint32_t curECmdId; /*! current excutable command id */
    static uint32_t reqId; /*! current request id */
    static uint32_t curEReqId; /*! current excutable request id */
    static TPCmdQueueNode curWorkQnode; /*! curWorkQnode */
    static TCmdQueue cmdQueue; /*! command queue */
    static TRequestElem *pReqElem;
    Commander();
protected:
    static QP::QMState const active_s;
    static QP::QMState const serving_s;
    static QP::QMState const idle_s;
    static QP::QState active(Commander * const me,
        QP::QEvt const * const e);
    static QP::QState active_e(Commander * const me);
    static QP::QState active_x(Commander * const me);
    static QP::QState idle(Commander * const me,
        QP::QEvt const * const e);
    static QP::QState idle_e(Commander * const me);
    static QP::QState idle_x(Commander * const me);
    static QP::QState initial(Commander * const me,
        QP::QEvt const * const e);
    static QP::QState serving(Commander * const me,
        QP::QEvt const * const e);
private:
    static void queueCmdRun(void);
    static void requestRun(void);
};

} /*namespace ARCS*/

#endif /* __COMMANDER_H__ */


