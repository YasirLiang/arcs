/*
* @file controller.h
* @brief controller
* @ingroup controller
* @cond
******************************************************************************
* Build Date on  2016-11-1
* Last updated for version 1.0.0
* Last updated on  2016-11-11
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
    QP::QMsm *commander;
    /*! local command id generate */
    uint32_t cmdId;
    /*! local request id generate */
    uint32_t requstId;
public:
    /*! Inflight double list array */
    struct list_head inflight[INFLIGHT_NUM];
    Controller();
protected:
    static QP::QMState const active_s; 
    static QP::QMState const serving_s;
    static QP::QState active(Controller * const me);
    static QP::QState initial(Controller * const me,
        QP::QEvt const * const e);
    static QP::QState serving(Controller * const me,
        QP::QEvt const * const e);
    static QP::QState serving_e(Controller * const me);
    static QP::QState serving_x(Controller * const me);
};

} /* namespace ARCS */

#endif /* __CONTROLLER_H__ */

