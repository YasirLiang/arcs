/*
* @file controller.cpp
* @brief controller
* @ingroup controller
* @cond
******************************************************************************
* Build Date on  2016-11-1
* Last updated for version 1.0.0
* Last updated on  2016-12-1
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
/*Including-----------------------------------------------------------------*/
#include "user.h"
#include "protocal_qt.h"
#include "qt_user.h"
#include "usertimer.h"
#include "inflight.h"
#include "controller.h"
#include "mainwidget.h"

namespace ARCS {

/*Local variable------------------------------------------------------------*/
QP::QMState const Controller::active_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top) */
    Q_STATE_CAST(&active), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};

/*Local variable------------------------------------------------------------*/
QP::QMState const Controller::serving_s = {
    &Controller::active_s, /* superstate (top) */
    Q_STATE_CAST(&serving), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
/*$ Class Local decralation-------------------------------------------------*/
/*! commander QMsm */
QP::QMsm *Controller::commander = Commander_getMsm();
/*! local command id generate */
uint32_t Controller::lcmdId;
/*! Inflight double list array */
struct list_head Controller::inflight[INFLIGHT_NUM];

/*Local variable------------------------------------------------------------*/
static Controller l_contoller;
/*Global variable-----------------------------------------------------------*/
QP::GuiQMActive *A0_Controller = &l_contoller;
/*$ Controller::getQtInflight().............................................*/
struct list_head * Controller_getQtInflight(void) {
    return &l_contoller.inflight[QT_INFLIGHT];
}
/*$ Controller::getNextReqId()..............................................*/
uint32_t Controller_getNextReqId(void) {
    return ++l_contoller.requstId;
}
/*$ Controller::Controller()................................................*/
Controller::Controller()
    : GuiQMActive(Q_STATE_CAST(&Controller::initial)),
      m_timeEvt(this, TICK_1MS_SIG, 0U)
{
    commander = Commander_getMsm();
    /* initial inflight double list head */
    for (int i = 0; i < INFLIGHT_NUM; i++) {
        INIT_LIST_HEAD(&inflight[i]);
    }
}
/*$ Controller::active()....................................................*/
QP::QState Controller::initial(Controller * const me,
        QP::QEvt const * const e)
{
    static struct {
            QP::QMState        const *target;
            QP::QActionHandler const act[2];
        } const table_ = {/* transition-action table */
            &serving_s,
            {
                Q_ACTION_CAST(&serving_e),
                Q_ACTION_CAST(0)/* zero terminator */
            }
    };
    /* initial msm */
    commander->init();
    /* subscribe terminate signal */
    me->subscribe(TERMINATE_SIG);
    /* avoid unused */
    (void)e;
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*$ Controller::active()....................................................*/
QP::QState Controller::active(Controller * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case TICK_1MS_SIG: {
            /*1\ looking for inflight list and proccess timeout
                  node */
            timeTick_();
            status_ = QM_HANDLED();
            break;
        }
        case TERMINATE_SIG: {
            qDebug("CONTROLLER EXIT");
            BSP_terminate(0);
            status_ = QM_HANDLED();
            break;
        }
        default: {
            break;
        }
    }
    /* avoid unused */
    (void)me;
    return status_;
}
/*$ Controller::serving()...................................................*/
QP::QState Controller::serving(Controller * const me,
        QP::QEvt const * const e)
{
    QP::QState status_;
    TPCmdQueueNode cmd;
    switch (e->sig) {
        case TICK_1MS_SIG: {
            me->m_timeEvt.postIn(me, (QP::QTimeEvtCtr)TICKS_PER_MS);
            status_ = QM_SUPER();
            break;
        }
        case TRANSMIT_SIG: {
            TransmitEvt const * const e_ =
                static_cast<TransmitEvt const * const>(e);
            /* analysis data and generate command */
            rxPacketEvent(e_->port,
                e_->buf, e_->datalen);
            status_ = QM_HANDLED();
            break;
        }
        case REQUEST_SIG: {
            qDebug("Recieve Requst Sig");
            RequestEvt const * const e_ =
                    static_cast<RequestEvt const * const>(e);
            if (e_->user == QT_REQUEST) {
                TPSpeCmdFunc pSpeFunc;
                switch(e_->type) {
                    case QUERY_ID: {
                        pSpeFunc = &QtCmd_queryId;
                        break;
                    }
                    case SWITCH_MATRIX: {
                        pSpeFunc = &QtCmd_switchMatrix;
                        break;
                    }
                    case OPT_TERMINAL: {
                        pSpeFunc = &QtCmd_optTerminal;
                        break;
                    }
                    case SET_SYS: {
                        pSpeFunc = &QtCmd_setTerminalSys;
                        break;
                    }
                    case CAMERA_CONTROL: {
                        pSpeFunc = &QtCmd_cameraControl;
                        break;
                    }
                    default: {
                        /* no other request type */
                        pSpeFunc = (TPSpeCmdFunc)0;
                        break;
                    }
                }
                if (pSpeFunc != (TPSpeCmdFunc)0) {
                    cmd = (TPCmdQueueNode)malloc(sizeof(TCmdQueueNode));
                    if (cmd != (TPCmdQueueNode)0) {
                        CmdQueue_elemInitial(&cmd->elem,
                            l_contoller.getNextCmd(), USER_CMD,
                            QT_USER, e_->buflen, e_->buf, pSpeFunc);
                        CommandEvt _e(cmd);
                        qDebug("Dispatch to Cmd(Type = %d) to Commander",
                            e_->type);
                        commander->dispatch(&_e);
                    }
                }
                status_ = QM_HANDLED();
            }
            else {
                /* log error message here */
                status_ = QM_UNHANDLED();
            }
            /* out of case */
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    /* avoid unused */
    (void)me;
    return status_;
}
/*$ Controller::serving_e()................................................*/
QP::QState Controller::serving_e(Controller * const me) {
    qDebug("Controller serving entry");
    me->m_timeEvt.postIn(me, (QP::QTimeEvtCtr)TICKS_PER_MS);
    return QM_ENTRY(&serving_s);
}
/*$ Controller::serving_x().................................................*/
QP::QState Controller::serving_x(Controller * const me) {
    qDebug("Controller serving exit");
    me->m_timeEvt.disarm();
    return QM_EXIT(&serving_s);
}
/*$ Controller::getNextCmd()................................................*/
uint32_t Controller::getNextCmd(void) {
    return ++lcmdId;
}
/*$ Controller::serverDataHandle()..........................................*/
int Controller::serverDataHandle(uint8_t const * const rxBuf,
    uint16_t const rxLen, uint32_t &notifyId, uint32_t &cmdId)
{
    int ret; /* return value */
    TInflightCmd_pNode p; /* pointer to inflight node */
    TPCmdQueueNode pCurCmd;
    TPRequestNode pCurReq;
    uint32_t reState;
    uint32_t notifyFlag = 0;
    bool matchCmd;
    bool reqDone;
    struct list_head *reList;
    TProtocalQt *buf = (TProtocalQt *)rxBuf;
    if (buf->head != PROTOCAL_QT_TYPE) {
        ret = -1; /* error handle */
    }
    else {
        matchCmd = (bool)0;
        if (buf->type & PRO_RESP_MASK) {
            p = Inflight_searchSeq(&inflight[QT_INFLIGHT],
                buf->seq);
            if (p != (TInflightCmd_pNode)0) { /* found ?*/
                cmdId = p->cmdId;
                notifyId = p->cmdNotificationId;
                notifyFlag = p->cmdNotificationFlag;
                callbackQt(cmdId, notifyId, notifyFlag, rxBuf, rxLen);
                Inflight_nodeDestroy(&p);
                /* match command id? */
                if (Commander_matchCmdId(cmdId, &pCurCmd)) {
                    matchCmd = (bool)1;
                }
            }
        }
        /* looking for request node,
             and set to its executing status */
        if (matchCmd) { /* match ?*/
            /* search executing request */
            pCurReq = (TPRequestNode)0;
            pCurReq = RequestList_searchNode(&pCurCmd->elem.requestHead,
                notifyId);
            /* current request executing ?*/
            if (pCurReq != (TPRequestNode)0) {
                /* set current running request status */
                reList = &pCurReq->in->statusList;
            }
            else {
                /* set current running command status */
                reList = &pCurCmd->elem.statusList;
            }
            /* set error */
            reState = buf->type & PRO_ERR_MASK;
            qDebug("reState =  0x%02x", (uint8_t)reState);
            /* save reponse to status to list (command or request) */
            Request_saveStatusToList(INFLIGHT_HD, QT_PTC,
                    0, reState, buf->cmd, reList);
            if (reState > 0) {/* error reponse */
                MainSurface::instance()->displayArcsErr(buf->cmd,
                    reState, 0);
            }
        }
        /* look for each inflight list, if no such inflight node,
             send request signal to */
        reqDone = (bool)1;
        for (int i = 0; i < INFLIGHT_NUM; i++) {
            p = Inflight_searchReq(&inflight[i],
                notifyId);
            if (p != (TInflightCmd_pNode)0) {/*found?*/
                reqDone = (bool)0;
                break;
            }
        }
        /* match cmd done? */
        if (matchCmd) {
            if (reqDone) {/* and request done? */
                /* send request done sig to commander */
                RequestDoneEvt e(notifyId);
                commander->dispatch(&e);
            }
        }
        else {
            if (!(buf->type & PRO_RESP_MASK)) {
                /* create new command and disptch commander */
                handleServerCmd(rxBuf, rxLen);
            }
        }
    }
    /* return value */
    return ret;
}
/*$ Controller::rxPacketEvent().............................................*/
void Controller::rxPacketEvent(TExternPort port,
    uint8_t const * const rxBuf, uint16_t const dataLen)
{
    uint32_t cmdId = 0;
    uint32_t notifyId = 0;
    Q_ASSERT((dataLen != 0) || (rxBuf != (uint8_t *)0));
    switch (port) {
        case QT_PORT: {/* handle data from qt port */
            serverDataHandle(rxBuf, dataLen, notifyId, cmdId);
            break;
        }
        default: {
            /* error type of system port, do nothing */
            break;
        }
    }
}
/*$ Controller::handleServerCmd.............................................*/
void Controller::handleServerCmd(uint8_t const * const rxBuf,
    uint16_t const rxLen)
{
    TPCmdQueueNode cmd;
    TProtocalQt const * const buf = (TProtocalQt *)rxBuf;
    TPSpeCmdFunc cmdFun;
    /* setting specific command func */
    switch (buf->cmd) {/* protocal command */
        case QT_QUEUE_ID: {
            cmdFun = &ServerCmd_queryId;
            break;
        }
        case QT_SWITCH_MATRIX: {
            cmdFun = &ServerCmd_switchMatrix;
            break;
        }
        case QT_OPT_TMNL: {
            cmdFun = &ServerCmd_optTerminal;
            break;
        }
        case QT_SYS_SET: {
            cmdFun = &ServerCmd_setTerminalSys;
            break;
        }
        case QT_CMR_CTL: {
            cmdFun = &ServerCmd_cameraControl;
            break;
        }
        default: {
            /* no such command */
            cmdFun = (TPSpeCmdFunc)0;
            /* log error message */
            break;
        }
    }
    /* match protocal command? */
    if (cmdFun != (TPSpeCmdFunc)0) {
        cmd = (TPCmdQueueNode)malloc(sizeof(TCmdQueueNode));
        if (cmd != (TPCmdQueueNode)0) {
            CmdQueue_elemInitial(&cmd->elem,
                l_contoller.getNextCmd(), USER_CMD,
                QT_USER, rxLen, rxBuf, cmdFun);
            CommandEvt e(cmd);
            qDebug("Qt cmd Handler Dispatch "
                    "Cmd(%d) to Commander",
                buf->cmd);
            commander->dispatch(&e);
        }
    }
}
/*$ Controller::tickQtInflight()............................................*/
void Controller::tickQtInflight(void) {
    TInflightCmd_pNode pos, n, p;
    TPCmdQueueNode pCurCmd;
    TPRequestNode pCurReq;
    uint32_t wkCmdId;
    uint32_t notifyId;
    bool matchCmd;
    bool reqDone;
    uint8_t ptCmd;
    struct list_head *reList;
    list_for_each_entry_safe(pos, n, &inflight[QT_INFLIGHT], list) {
        if (Inflight_timeout(pos)) {
            if (Inflight_retried(pos)) {
               ptCmd = ((TProtocalQt *)pos->pBuf)->cmd;
                /* get some information before destroy */
                wkCmdId = pos->cmdId;
                notifyId = pos->cmdNotificationId;
                /* message timeout log */
                qDebug("[TIMEOUT %d, %d, %d, %d, %d]",
                    QT_INFLIGHT, pos->cmdSeqId,
                    pos->cmdId, pos->cmdNotificationId,
                    ptCmd);
                /* destroy */
                Inflight_nodeDestroy(&pos);
                /* match command id? */
                if (Commander_matchCmdId(wkCmdId, &pCurCmd)) {
                    matchCmd = (bool)1;
                }
                /* looking for request node,
                     and set to its executing status */
                if (matchCmd) { /* match ?*/
                    /* search executing request */
                    pCurReq = (TPRequestNode)0;
                    pCurReq = RequestList_searchNode(&pCurCmd->elem.requestHead,
                        notifyId);
                    /* current request executing ?*/
                    if (pCurReq != (TPRequestNode)0) {
                        /* set current running request status */
                        reList = &pCurReq->in->statusList;
                    }
                    else {
                        /* set current running command status */
                        reList = &pCurCmd->elem.statusList;
                    }
                    /* save reponse to status to list (command or request) */
                    Request_saveStatusToList(INFLIGHT_HD, QT_PTC,
                            0, QTIMEOUT, ptCmd, reList);
                    MainSurface::instance()->displayArcsErr(ptCmd,
                        QTIMEOUT, (bool)1); /*\ */
                }
                /* look for each inflight list, if no such inflight node,
                     send request signal to */
                reqDone = (bool)1;
                for (int i = 0; i < INFLIGHT_NUM; i++) {
                    p = Inflight_searchReq(&inflight[i],
                        notifyId);
                    if (p != (TInflightCmd_pNode)0) {/*found?*/
                        reqDone = (bool)0;
                        break;
                    }
                }
                /* match cmd done? */
                if (matchCmd) {
                    if (reqDone) {/* and request done? */
                        /* send request done sig to commander */
                        RequestDoneEvt e(notifyId);
                        commander->dispatch(&e);
                    }
                }
                pCurReq = (TPRequestNode)0;
                pCurCmd = (TPCmdQueueNode)0;
            }
            else { /* resend data */
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                        ARCS::QT_PORT, pos->dataLen, pos->pBuf),
                    (void*)0);
                Inflight_increaseSendCnt(pos);
                Inflight_updateTimer(pos, pos->cmdTimeoutMs);
                qDebug("[ReSend %d, %d, %d, %d, %d]",
                    QT_INFLIGHT, pos->cmdSeqId,
                    pos->cmdId, pos->cmdNotificationId,
                    ((TProtocalQt*)pos->pBuf)->cmd);
            }
        }
    }
}
/*$ Controller::timeTick_().................................................*/
void Controller::timeTick_(void) {
    tickQtInflight();
}
/*$ Controller::callbackQt()................................................*/
void Controller::callbackQt(uint32_t cmdId, uint32_t notifyId,
    uint32_t notifyFlag, uint8_t const * const rxBuf, uint16_t const rxLen)
{
    TProtocalQt const * const buf =
            (TProtocalQt *)rxBuf;
    uint32_t reState;
    /* log callback */
    if (notifyFlag == NOTIFY_FLAG) {
        /* set reponse state */
        if (buf->type & PRO_ERR_MASK) {
            reState = QHOST_SHIELDED;
        }
        else {
            reState = QSUCCESS;
        }
        qDebug("[QT RESPONSE %d(cmd), %d(seq),"
            "%d(datalen), %d(respflag)]",
            buf->cmd, buf->seq,
            buf->dataLen, reState);
    }
    /* aviod unused */
    (void)cmdId;
    (void)notifyId;
    (void)rxLen;
}

}

