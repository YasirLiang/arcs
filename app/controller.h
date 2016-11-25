/*
* @file controller.h
* @brief controller
* @ingroup controller
* @cond
******************************************************************************
* Build Date on  2016-11-1
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
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

namespace ARCS {

enum {
        QT_INFLIGHT,
        INFLIGHT_NUM /*! Inflight type number */
};
/*! class controller */
class Controller : public QP::GuiQMActive {
private:
    /*! commander QMsm */
    static QP::QMsm *commander;
    /*! local command id generate */
    static uint32_t lcmdId;
    /* 1ms time out event */
    QP::QTimeEvt m_timeEvt;
    static void timeTick_(void);
    static uint32_t getNextCmd(void);
public:
    /*! local request id generate */
    uint32_t requstId;
    /*! Inflight double list array */
    static struct list_head inflight[INFLIGHT_NUM];
    Controller();
    static void tickQtInflight(void);
    static int serverDataHandle(uint8_t const * const rxBuf,
        uint16_t const rxLen, uint32_t &notifyId, uint32_t &cmdId);
    static void handleServerCmd(uint8_t const * const rxBuf,
        uint16_t const rxLen);
    static void callbackQt(uint32_t cmdId, uint32_t notifyId,
        uint32_t notifyFlag, uint8_t const * const rxBuf,
        uint16_t const rxLen);
    static void rxPacketEvent(TExternPort port,
        uint8_t const * const rxBuf, uint16_t const dataLen);
protected:
    static QP::QMState const active_s; 
    static QP::QMState const serving_s;
    static QP::QState active(Controller * const me,
        QP::QEvt const * const e);
    static QP::QState initial(Controller * const me,
        QP::QEvt const * const e);
    static QP::QState serving(Controller * const me,
        QP::QEvt const * const e);
    static QP::QState serving_e(Controller * const me);
    static QP::QState serving_x(Controller * const me);
};

} /* namespace ARCS */

#endif /* __CONTROLLER_H__ */

