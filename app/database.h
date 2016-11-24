/*
* @file
* @brief DatabaseAct active object
* @ingroup DatabaseAct active object
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
#ifndef __DATABASE_H__
#define __DATABASE_H__

namespace ARCS {

class DatabaseAct : public QP::GuiQMActive {
protected:
    static QP::QState active(DatabaseAct * const me);
    static QP::QState initial(DatabaseAct * const me,
        QP::QEvt const * const e);
    static QP::QState serving(DatabaseAct * const me,
        QP::QEvt const * const e);
    static QP::QState serving_e(DatabaseAct * const me);
    static QP::QState serving_x(DatabaseAct * const me);
};

} /* namespace ARCS */

#endif /* __DATABASE_H__ */

