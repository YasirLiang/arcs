/*
* @file commander.cpp
* @brief Commander QMsm object
* @ingroup Commander QMsm object
* @cond
******************************************************************************
* Build date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-11-18
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#include "requestproccessor.h"

namespace ARCS {

static Commander l_commander;

QP::QMState const Commander::active_s = {
    &Commander::serving_s, /*superstae(serving_s)*/
    Q_STATE_CAST(&active), /* state handle active*/
    Q_ACTION_CAST(&active_e),/* entry action */
    Q_ACTION_CAST(&active_x),/* exit action */
    Q_ACTION_CAST(0) /* no initial action */
};

QP::QMState const Commander::serving_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top) */
    Q_STATE_CAST(&serving), /* state handle active*/
    Q_STATE_CAST(0), /* entry action */
    Q_STATE_CAST(0), /* exit action */
    Q_STATE_CAST(0) /* no initial action */
};

QP::QMState const Commander::idle_s = {
    &Commander::serving_s, /*superstae(serving_s)*/
    Q_STATE_CAST(&idle), /* state handle active*/
    Q_ACTION_CAST(&idle_e),/* entry action */
    Q_ACTION_CAST(&idle_x),/* exit action */
    Q_ACTION_CAST(0) /* no initial action */
};
/*get request instance......................................................*/
QP::QMsm *Commander_getMsm(void) {
    return &l_commander;
}
/*match current excuting command id.........................................*/
bool Commander_matchCmdId(uint32_t cmdId, TPCmdQueueNode *ppCurWk) {
    bool bret = (bool)0;
    if (l_commander.curWorkQnode != (TPCmdQueueNode)0) {
        if (l_commander.curWorkQnode->elem.id == cmdId) {
            *ppCurWk = l_commander.curWorkQnode;
            bret = (bool)1;
        }
    }
    return bret;
}
/*update request id for not realy running request...........................*/
void Commander_updateCurrentReq(uint32_t req) {
    l_commander.curEReqId = req;
}
/*Commander().......................................................*/
Commander::Commander()
  : QMsm(Q_STATE_CAST(&Commander::initial)),
    commandId((uint32_t)0),
    curECmdId((uint32_t)0),
    reqId((uint32_t)0),
    curEReqId((uint32_t)0)
{
    /* get commander msm */
    reQ = Requestor_getMsm();
    curWorkQnode = (TPCmdQueueNode)0;
    CmdQueue_init(&cmdQueue, (int)150);
}
/*initial().......................................................*/
QP::State Commander::initial(Commander * const me,
        QP::QEvt const * const e)
{
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &idle_s,
        {
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };
    /* initial msm */
    reQ->init();
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*active()..................................................................*/
QP::QState Commander::active(Commander * const me,
        QP::QEvt const * const e)
{
    QP::QState status_;
    TPCmdQueueNode cmdQNode = (TPCmdQueueNode)0;
    TPRequestNode pReq = (TPRequestNode)0;
    /* idle state table */
    static struct {
        QP::QMState const *target;
        QP::QActionHandler act[3];
   } const tatbl_ = { /* transition-action table */
        &idle_s,
        {
            Q_ACTION_CAST(&active_x), /* exit */
            Q_ACTION_CAST(&idle_e), /* entry */
            Q_ACTION_CAST(0)  /* zero terminator */
        }
    };
    switch (e->sig) {
        case COMMAND_SIG: {
            cmdQNode = (static_cast<CommandEvt*>e)->em;
            Q_ASSERT(cmdQNode != (TPCmdQueueNode)0);
            /* push to command queue */
            if (0 == CmdQueue_push(cmdQueue, cmdQNode)) {
                /* successful push */
                qDebug("[Command %d push Queue successful]",
                    cmdQNode->elem.id);
            }
            else {
                qDebug("[Command %d push Queue Failed]",
                    cmdQNode->elem.id);
            }
            status_ = QM_HANDLED();
            break;
        }
        case REQUEST_DONE_SIG: {
            /* proccessing other command will faild */
            Q_ASSERT(curWorkQnode != (TPCmdQueueNode)0
                && (static_cast<RequestDoneEvt*>e)->rId != curEReqId);
            RequestDoneEvt er(curEReqId);
            reQ->dispatch(&er);
            /* User will make sure current executable command
                whether is ended. If not, generate another 
                request locally and put it to current work queue
                node and dispatch it to 'reQ'.Otherwise, release
                all current work queue node space, including all
                request information. */
            pReqElem = (TRequestElem *)Cmd_run(curWorkQnode);
            if (pReqElem != (TRequestElem*)0) {
                /* command is not finishing, allot for request node */
                Q_ASSERT(curWorkQnode->elem.execStatus == REQUEST_EXECUTING);
                pReq = (TPRequestNode)malloc(sizeof(TPRequestNode));
                if (pReq != (TPRequestNode)0) {
                    /* initial request node */
                    RequestList_nodeInit(pReq, pReqElem);
                    /* push to link list */
                    RequestList_addTrail(&curWorkQnode->elem.requestHead,
                        pReq);
                    /* publish new request to requester */
                    RequestElemEvt ere(pReqElem);
                    reQ->dispatch(&ere);
                    /* update current executable request id */
                    curEReqId = pReqElem->id;
                    /* current request run done */
                    if (pReqElem->status == REQ_SUCCESS) {
                        /* run next request */
                        requestRun();
                    }
                    /* if command run successfully, change state machine
                         to idle */
                    if (curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                        /* run command in the local command working queue.
                             When current command be executed successfully, this
                             function will get next command and run it until all
                             command run completly or current command waiting
                             for running */
                        queueCmdRun();
                        if (qCmdExeStatus == CQE_SUCCESS) {
                            /* all command request run success */
                            /* change state machine to idle */
                            status_ = QM_TRAN(&tatbl_);
                        }
                        else {
                            status_ = QM_HANDLED();
                        }
                    }
                    else {
                        status_ = QM_HANDLED();
                    }
                }
                else {/* error space to for request */
                    Q_ASSERT(pReq != (TRequestElem*)0);
                }
            }
            else if (curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                /* run command in the local command working queue.
                     When current command be executed successfully, this
                     function will get next command and run it until all
                     command run completly or current command waiting
                     for running */
                queueCmdRun();
                if (qCmdExeStatus == CQE_SUCCESS) {
                    /* all command run success */
                    /* change state machine to idle */
                    status_ = QM_TRAN(&tatbl_);
                }
                else {
                    status_ = QM_HANDLED();
                }
            }
            else {
                /* other execStatus do nothing,
                     command not finished */
                status_ = QM_HANDLED();
            }
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*active_e()................................................................*/
QP::QState Commander::active_e(Commander * const me) {
}
/*active_x()................................................................*/
QP::QState Commander::active_x(Commander * const me) {
    return QM_EXIT(&active_s);
}
/*idle()....................................................................*/
QP::State Commander::idle(Commander * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    TPRequestNode pReq = (TPRequestNode)0;
    struct list_head *list_;
    switch (e->sig) {
        case COMMAND_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
           } const tatbl_ = { /* transition-action table */
                &active_s,
                {
                    Q_ACTION_CAST(&idle_x), /* exit */
                    Q_ACTION_CAST(&active_e), /* entry */
                    Q_ACTION_CAST(0)  /* zero terminator */
                }
            };
           /* get command node */
           curWorkQnode = (static_cast<CommandEvt*>e)->em;
           Q_ASSERT(curWorkQnode != (TPCmdQueueNode)0);
            /* directly excecut Command function and don't need
                to push to command working queue */
            pReqElem = (TRequestElem *)Cmd_run(curWorkQnode);
            if (pReqElem != (TRequestElem*)0) {/* local generate request */
                Q_ASSERT(curWorkQnode->elem.execStatus == REQUEST_EXECUTING);
                /* command is not finishing, allot for request node */
                pReq = (TPRequestNode)malloc(sizeof(TPRequestNode));
                if (pReq != (TPRequestNode)0) {
                    /* initial request node */
                    RequestList_nodeInit(pReq, pReqElem);
                    /* push to link list */
                    RequestList_addTrail(&curWorkQnode->elem.requestHead,
                        pReq);
                    /* publish new request to requester */
                    RequestElemEvt ere(pReqElem);
                    reQ->dispatch(&ere);
                    /* update current executable request id */
                    curEReqId = pReqElem->id;
                    /* current request run done */
                    if (pReqElem->status == REQ_SUCCESS) {
                        requestRun();
                    }
                    /* if current command node run successfully,
                         don't need to change state */
                    if (curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                        list_ = &curWorkQnode->elem.requestHead;
                        RequestList_destroy(&list_);
                        /* release commander status list */
                        list_ = &curWorkQnode->elem.statusList;
                        RequestList_statusListDestroy(list_);
                        /* curWorkQnode must be released at last */
                        CmdQueue_nodeDestroy(&curWorkQnode);
                        /* set handled status */
                        status_ = QM_HANDLED();
                    }
                    else {
                        /* set active state to waiting for command finished */
                        status_ = QM_TRAN(&tatbl_);
                    }
                }
                else {/* error space to for request */
                    Q_ASSERT(pReq != (TRequestElem*)0);
                }
            }
            else if (curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
            /* current command be finished, user make decision to
                execute success!*/
                list_ = &curWorkQnode->elem.requestHead;
                RequestList_destroy(&list_);
                /* release commander status list */
                list_ = &curWorkQnode->elem.statusList;
                RequestList_statusListDestroy(list_);
                /* curWorkQnode must be released at last */
                CmdQueue_nodeDestroy(&curWorkQnode);
                /* set status */
                status_ = QM_HANDLED();
            }
            else {
                /* other execStatus no need to do anything */
                status_ = QM_SUPER();
            }
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*idle_e()..................................................................*/
QP::State Commander::idle_e(Commander * const me) {

}
/*idle_x()..................................................................*/
QP::State Commander::idle_x(Commander * const me) {

}
/*serving().................................................................*/
QP::State Commander::serving(Commander * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = QM_HANDLED();
            break;
        }
    }
    return status_;
}
/*$ Commander::requestRun().................................................*/
void Commander::requestRun(void) {
    /* this function run request until request
         doing, output value is last request running
         'reqExeStatus' and transfered under prev
         request function successfully */
    TPRequestNode pReq = (TPRequestNode)0;
    RequestDoneEvt e(curEReqId);
    /* make sure requestor can run request */
    reQ->dispatch(&e);
    /* run command again */
    pReqElem = (TRequestElem *)Cmd_run(curWorkQnode);
    if (pReqElem != (TRequestElem*)0) {
        /* command is not finishing, allot for request node */
        Q_ASSERT(curWorkQnode->elem.execStatus == REQUEST_EXECUTING);
        pReq = (TPRequestNode)malloc(sizeof(TPRequestNode));
        if (pReq != (TPRequestNode)0) {
            /* initial request node */
            RequestList_nodeInit(pReq, pReqElem);
            /* push to link list */
            RequestList_addTrail(&curWorkQnode->elem.requestHead,
                pReq);
            /* publish new request to requester */
            RequestElemEvt e_(REQUEST_ELEM_SIG, pReqElem);
            reQ->dispatch(&e_);
            /* update current executable request id */
            curEReqId = pReqElem->id;
            /* current request run done */
            if (pReqElem->status == REQ_SUCCESS) {
                /* recursion request run until current request 
                     doing or there is no request generated by
                     Cmd_run() */
                requestRun();
            }
        }
        else {/* error space to for request */
            Q_ASSERT(pReq != (TRequestElem*)0);
        }
    }
}
/*$ Commander::queueCmdRun()................................................*/
void Commander::queueCmdRun(void) {
/* current command be finished, user make decision to
    execute success!*/
    TPRequestNode pReq = (TPRequestNode)0;
    /* destroy request head list */
    RequestList_destroy(&curWorkQnode->elem.requestHead);
    /* release commander status list */
    RequestList_statusListDestroy(&curWorkQnode->elem.statusList);
    /* curWorkQnode must be released at last */
    CmdQueue_nodeDestroy(&curWorkQnode);

    /* get another command work queue node and excute it.
        If there is cmd node in Commander Queue, get it from
        queue and excute it ending until last Command exectable
        status not EXEC_SUCCESS or there is no workinng node in the
        Commander Queue */
    curWorkQnode = CmdQueue_pop(&cmdQueue);
    if (curWorkQnode != (TPCmdQueueNode)0) {
        pReqElem = (TRequestElem *)Cmd_run(curWorkQnode);
        if (pReqElem != (TRequestElem*)0) {
            /* command is not finishing, allot for request node */
            Q_ASSERT(curWorkQnode->elem.execStatus == REQUEST_EXECUTING);
            pReq = (TPRequestNode)malloc(sizeof(TPRequestNode));
            if (pReq != (TPRequestNode)0) {
                /* initial request node */
                RequestList_nodeInit(pReq, pReqElem);
                /* push to link list */
                RequestList_addTrail(&curWorkQnode->elem.requestHead,
                    pReq);
                /* publish new request to requester */
                RequestElemEvt e(REQUEST_ELEM_SIG, pReqElem);
                reQ->dispatch(&e);
                /* update current executable request id */
                curEReqId = pReqElem->id;
                /* current request run done */
                if (pReqElem->status == REQ_SUCCESS) {
                    requestRun();
                }
                /* set queue command current node executing is in progress*/
                if (curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                    /* current command being finished and
                         execute next command */
                    queueCmdRun();
                }
                else {
                    qCmdExeStatus = CQE_DOING;
                }
            }
            else {/* error space to for request */
                Q_ASSERT(pReq != (TRequestElem*)0);
            }
        }
        else if (curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
            /* executable next command */
            queueCmdRun();
        }
        else { /* other excutable status */
            /* set queue command current node executing is in progress */
            qCmdExeStatus = CQE_DOING;
        }
    }
    else {
        /* set queue command execute successfully */
        qCmdExeStatus = CQE_SUCCESS;
    }
}

} /* namespace ARCS */

