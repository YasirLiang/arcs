/*
* @file
* @brief avdecc reception Control System define
* @ingroup avdecc funhdl native
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-10-24
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __ARCS_H__
#define __ARCS_H__

/*! Macro port buffer size--------------------------------------------------*/
#define PORT_BUF_SIZE 640

#include "request.h"

/*! avdecc reception control system namespace-------------------------------*/
namespace ARCS {

/*! avdecc recetion control system enumration signal------------------------*/
typedef enum ArcsSignal {
    /*! published terminate signal to all active object for QT */
    TERMINATE_SIG = QP::Q_USER_SIG,
    MAX_PUB_SIG, /*! max published signal number */
    REQUEST_SIG, /*! Request sig */
    TRANSMIT_SIG, /*! send to Requestor or Transmitor */
    PORTREADABLE_SIG, /*! send from QT to Transmitor */
    OPT_DATABASE_SIG,/*! send from RequestProccessor to DatabaseAct */
    TICK_1MS_SIG, /*! one Ms timeout signal */
    REQUEST_ELEM_SIG, /*! request element signal */
    COMMAND_SIG, /*! command signal */
    REQUEST_DONE_SIG, /*! Request node signal */
    PORT_CHANGE_SIG, /*! Qt port change  */
    MAX_SIG /*! Always at the end */
}ArcsSignal;

}/* namespace ARCS */

namespace ARCS {

/*! System extern port used */
typedef enum TExternPort {
    QT_PORT, /*! QT port used define */
}TExternPort;

/*! System extern port event type */
typedef enum TPortEvtType {
    READABLE /*! port readable event type */
}TPortEvtType;

/*! TProBaseStatus */
typedef enum TProBaseStatus {
    FAILED, /*! Failed proccess database status */
    SUCCESS /*! Success */
}TProBaseStatus;
/*! TProBaseType */
typedef enum TProBaseType {
    BASE_MODIFY, /*! Modify operation */
    BASE_QUERY /*! Query operation */
}TProBaseType;

/*! TProBaseFormat */
typedef enum TProBaseFormat {
    FORMAT_1, /*!data format 1 */
    FORMAT_2  /*!data format 2 */
}TProBaseFormat;

enum {
    WITHOUT_NOTIFY_FLAG = 0,
    NOTIFY_FLAG
};
/*! response protocal status */
enum TEnumQStatus {
    QSUCCESS,       /*! protocal response ok */
    QHOST_SHIELDED, /*! host is sheiled by  other client */
    QTIMEOUT = 1024 /*! protcal command timeout */
};

/*! enumration of system port */
enum {
    QT_SV_EP = 0, /*! qt server external port */
    EXTERN_PORT_NUM    /*! external port number */
};

}/* namespace ARCS */

namespace ARCS {

/*! class request event */
class RequestEvt : public QP::QEvt {
public:
    uint32_t id; /*! allot by commander QMsm */
    TRequestType type;
    TRequestUser user;
    uint16_t buflen;
    uin8_t buf[REQUEST_BUF_SIZE];
public:
    RequestEvt(QP::QSignal sig,
        uint32_t id_p,
        TRequestType t,
        uint8_t* p,
        uint16_t len,
        TRequestUser _u)
      : QEvt(sig),
        id(id_p),
        type(t),
        buflen(len),
        user(_u);
    {
        memset(buf, 0, REQUEST_BUF_SIZE);/* reset memery */
        Q_ASSERT(buflen <= REQUEST_BUF_SIZE);
        if ((buflen != 0U) &&
             (p != (uint8_t *)0))
        {
            memcpy(buf, p, buflen); /* copy data */
        }
    }
};
/*! class request element event */
class RequestElemEvt : public QP::QEvt {
public:
    TRequestElem *pElem;
public:
    RequestElemEvt(TRequestElem *pE)
      : QEvt(REQUEST_ELEM_SIG),
        pElem(pE);
    {
    }
};
/*! class Transmit event */
class TransmitEvt : public QP::QEvt {
public:
    TExternPort port;
    uint16_t datalen;
    uint8_t buf[PORT_BUF_SIZE];
public:
    TransmitEvt(TExternPort po,
        uin16_t len,
        uin8_t *p)
      : QEvt(TRANSMIT_SIG),
        port(po),
        datalen(len)
    {
        memset(buf, 0, PORT_BUF_SIZE); /* reset memery */
        Q_ASSERT((datalen <= PORT_BUF_SIZE)
            && (p != (uint8_t *)0));
        if ((datalen <= PORT_BUF_SIZE)
            && (p != (uint8_t *)0))
        {
            memcpy(buf, p, datalen); /* copy data */
        }
    }
};
/*! class Transmit event */
class PortChangeEvt : public QP::QEvt {
public:
    TExternPort port;
    uint16_t datalen;
    uint8_t buf[64];
public:
    PortChangeEvt(TExternPort po,
        uin16_t len,
        uin8_t *p)
      : QEvt(PORT_CHANGE_SIG),
        port(po),
        datalen(len)
    {
        memset(buf, 0, 64); /* reset memery */
        Q_ASSERT((datalen <= 64)
            && (p != (uint8_t *)0));
        if ((datalen <= 64)
            && (p != (uint8_t *)0))
        {
            memcpy(buf, p, datalen); /* copy data */
        }
    }
};
/*! class Database operation event */
class DatabaseEvt : public QP::QEvt {
public:
    uin16_t id;
    TProBaseStatus status;
    TProBaseType type;
    TProBaseFormat format;
    uin16_t buflen;
    uin8_t buf[REQUEST_BUF_SIZE];
public:
    DatabaseEvt(QP::Signal sig,
        uin16_t id_p,
        TProBaseStatus _s,
        TProBaseType _t,
        TProBaseFormat ft,
        uint16_t len,
        uin8_t *p)
      : QEvt(sig),
        id(id_p),
        status(_s),
        type(_t),
        buflen(len),
        format(ft)
    {
        memset(buf, 0, REQUEST_BUF_SIZE); /* reset memery */
        Q_ASSERT(buflen <= REQUEST_BUF_SIZE);
        if ((buflen != 0u) &&
             (p != (uint8_t *)0))
        {
            memcpy(buf, p, len); /* copy data */
        }
    }
};
/*! port Event */
class PortEvt : public QP::QEvt {
public:
    TExternPort port;
    TPortEvtType eType;
public:
    PortEvt(QP::Signal sig,
        TExternPort po,
        TPortEvtType eT)
      : QEvt(sig),
        port(po),
        eType(eT)
    {
    }
};
/*! CommandEvt */
class CommandEvt : public QP::QEvt {
public:
    TPCmdQueueNode em; /*! command event element */
public:
    CommandEvt(TPCmdQueueNode p)
      : QEvt(REQUEST_DONE_SIG),
        em(p)
    {
    }
};

class RequestDoneEvt : public QP::QEvt {
public:
    uint32_t rId; /*! request id */
public:
    RequestDoneEvt(uin32_t _id)
      : QEvt(REQUEST_DONE_SIG),
        rId(_id)
    {
    }
};

QP::QMsm *Requestor_getMsm(void);
QP::QMsm *Commander_getMsm(void);
struct list_head * Controller_getQtInflight(void);
uint32_t Controller_getNextReqId(void);
/*! match current excuting command id---------------------------------------*/
bool Commander_matchCmdId(uint32_t cmdId);
/*! Commander_updateCurrentReq----------------------------------------------*/
void Commander_updateCurrentReq(uint32_t req);
QP::QMsm* PortStateMachine_getIns(uint8_t id);
void PortStateMachine_setPortVtbl(TExternPortVtbl *ptr,
    uin8_t id);
}/* namespace ARCS */

#endif /*__ARCS_H__*/

