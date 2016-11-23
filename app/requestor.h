/*
* @file
* @brief Requestor active object
* @ingroup Requestor active object
* @cond
******************************************************************************
* Build date on  2016-11-1
* Last updated for version 1.0.0
* Last updated on  2016-11-1
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __REQUESTOR_H__
#define __REQUESTOR_H__

namespace ARCS {

class Requestor : public QP::QMsm {
private:
    /*! current request id */
    uint32_t curRequestId;
    /*! current ReqElem */
    TRequestElem *curReqElem;
public:
    Requestor();
protected:
    static QP::QMState const active_s;
    static QP::QMState const serving_s;
    static QP::QMState const idle_s;
    static QP::QState active(Requestor * const me,
        QP::QEvt const * const e);
    static QP::QState active_e(Requestor * const me);
    static QP::QState active_x(Requestor * const me);
    static QP::State idle(Requestor * const me,
        QP::QEvt const * const e);
    static QP::State idle_e(Requestor * const me);
    static QP::State initial(Requestor * const me,
        QP::QEvt const * const e);
    static QP::State serving(Requestor * const me,
        QP::QEvt const * const e);
};

} /*namespace ARCS*/

#endif /* __REQUESTOR_H__ */

