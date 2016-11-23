/*
* @file
* @brief transmitor active object
* @ingroup transmitor active object
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-11-21
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __TRANSMITOR_H__
#define __TRANSMITOR_H__

namespace ARCS {

class Transmitor : public QP::GuiQMActive {
private:
    QP::QMsm *port[EXTERN_PORT_NUM]; /*! port state machine */
    /* recv buffer 1...n */
protected:
    static QP::QState active(Transmitor * const me);
    static QP::QState initial(Transmitor * const me,
        QP::QEvt const * const e);
    static QP::QState serving(Transmitor * const me,
        QP::QEvt const * const e);
    static QP::QState serving_e(Transmitor * const me);
    static QP::QState serving_x(Transmitor * const me);
};

} /* namespace ARCS */

#endif /* __TRANSMITOR_H__ */

