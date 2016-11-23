/*
* @file controller.cpp
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
/*Including-----------------------------------------------------------------*/
#include "controller.h"

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
/*Local variable------------------------------------------------------------*/
static Controller l_contoller;
/*Global variable-----------------------------------------------------------*/
QP::GuiQMActive A0_Controller = &l_contoller;
/*$ Controller::Controller()................................................*/
Controller::Controller()
    : GuiQMActive(Q_STATE_CAST(&Controller::initial))
{
    /* get commander msm */
    commander = Commander_getMsm();

    /* initial inflight double list head */
    for (int i = 0; i < INFLIGHT_NUM; i++) {
        INIT_LIST_HEAD(&inflight[i]);
    }
}
/*$ Controller::active()....................................................*/
QP::QState Controller::active(Controller * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case TICK_1MS_SIG: {
            status_ = QM_HANDLE();
            break;
        }
        default: {
            break;
        }
    }
}
struct list_head * Controller_getQtInflight(void) {
    return &l_contoller.inflight[QT_INFLIGHT];
}
uint32_t Controller_getNextReqId(void) {
    return ++l_contoller.requstId;
}
uint32_t Controller::getNextCmd(void) {
    return ++cmdId;
}
/*$ Controller::active()....................................................*/
QP::QState Controller::initial(Controller * const me,
        QP::QEvt const * const e)
{
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &serving_s,
        {
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };
    /* initial msm */
    commander->init();
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*$ Controller::serving()...................................................*/
QP::QState Controller::serving(Controller * const me,
        QP::QEvt const * const e)
{
    QP::QState status_;
    TPCmdQueueNode cmd;
    switch (e->sig) {
        case TICK_1MS_SIG: {
            status_ = QM_SUPER();
            break;
        }
        case TRANSMIT_SIG: {
            TransmitEvt *e_ = static_cast<TransmitEvt *>(e);
            /* analysis data and generate command */
            rxPacketEvent(e_->port,
                e_->buf, e_->datalen);
            break;
        }
        case REQUEST_SIG: {
            RequestEvt *e_ = static_cast<RequestEvt *>(e);
            if (e_->user == QT_REQUEST) {
                cmd = (TPCmdQueueNode)malloc(sizeof(TCmdQueueNode));
                if (cmd != (TPCmdQueueNode)) {
                    cmd->elem.type = USER_CMD;
                    cmd->elem.user = QT_USER;
                    cmd->elem.execStatus = NO_START;
                    cmd->elem.id = getNextCmd();
                    /* request data length set */
                    cmd->elem.cmdBufLen = e_->buflen;
                    if (e_->buflen > 0) {
                        /* set request data to cmd buffer */
                        memcpy(cmd->elem.cmdBuf, e_->buf, e_->buflen);
                    }
                    switch(e_->type) {
                        case QUERY_ID: {
                            cmd->elem.run = &QtCmd_queryId;
                            CommandEvt _e(cmd);
                            commander->dispatch(&_e);
                            break;
                        }
                        case SWITCH_MATRIX: {
                            cmd->elem.run = &QtCmd_switchMatrix;
                            CommandEvt _e(cmd);
                            commander->dispatch(&_e);
                            break;
                        }
                        case OPT_TERMINAL: {
                            cmd->elem.run = &QtCmd_optTerminal;
                            CommandEvt _e(cmd);
                            commander->dispatch(&_e);
                            break;
                        }
                        case SET_SYS: {
                            if (!(e_->buf[0] & 0x01)) {
                                /* local system set */
                                cmd->elem.run = &QtCmd_setLocalSys;
                            }
                            else {
                                cmd->elem.run = &QtCmd_setTerminalSys;
                            }
                            CommandEvt _e(cmd);
                            commander->dispatch(&_e);
                            break;
                        }
                        case CAMERA_CONTROL: {
                            cmd->elem.run = &QtCmd_cameraControl;
                            CommandEvt _e(cmd);
                            commander->dispatch(&_e);
                            break;
                        }
                        default: {
                            /* no other request type */
                            free(cmd);
                            cmd = (TPCmdQueueNode)0;
                            break;
                        }
                    }
                    status_ = QM_HANDLED();
                }
                else {
                    /* log error message here */
                    status_ = QM_UNHANDLED();
                }
            }
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
}
/*$ Controller::serving_e()................................................*/
QP::QState Controller::serving_e(Controller * const me) {

}
/*$ Controller::serving_x().................................................*/
QP::QState Controller::serving_x(Controller * const me) {

}
/*$ Controller::serverDataHandle()..........................................*/
int Controller::serverDataHandle(uint8_t const * const rxBuf,
    uint16_t const rxLen, uint32_t &notifyId, uint32_t &cmdId)
{
    int ret; /* return value */
    TInflightCmd_pNode p; /* pointer to inflight node */
    TPCmdQueueNode pCurCmd;
    TPRequestNode pCurReq;
    uint32_t wkCmdId, reState;
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
            /* set reponse state */
            if (buf->type & PRO_ERR_MASK) {
                reState = QHOST_SHIELDED;
            }
            else {
                reState = QSUCCESS;
            }
            /* save reponse to status to list (command or request) */
            Request_saveStatusToList(INFLIGHT_HD, QT_PTC,
                    0, reState, reList);
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
            /* create new command and disptch commander */
            handleServerCmd(rxBuf, rxLen);
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
    bool reqDone;
    TInflightCmd_pNode p; /* pointer to inflight node */
    TPRequestNode pCurReq;
    TPCmdQueueNode pCurCmd;
    bool matchCmd = (bool)0;
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
/**/
void Controller::handleServerCmd(uint8_t const * const rxBuf,
    uint16_t const rxLen)
{
    TCmdQueueNode cmd;
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
        if (cmd != (TPCmdQueueNode)) {
            cmd->elem.type = USER_CMD;
            cmd->elem.user = SERVER_USER;
            cmd->elem.execStatus = NO_START;
            cmd->elem.id = getNextCmd();
            /* request data length set */
            cmd->elem.cmdBufLen = rxLen;
            /* set command data to cmd buffer */
            memcpy(cmd->elem.cmdBuf, rxBuf, rxLen);
            /* set specific function */
            cmd->elem.run = cmdFun;
            CommandEvt e(cmd);
            commander->dispatch(&e);
        }
    }
}
/*$ Controller::callbackQt()................................................*/
void Controller::callbackQt(uint32_t cmdId, uint32_t notifyId,
    uint32_t notifyFlag, uint8_t const * const rxBuf, uint16_t const rxLen)
{
    TProtocalQt *buf = (TProtocalQt *)rxBuf;
    /* log callback */
}

}

