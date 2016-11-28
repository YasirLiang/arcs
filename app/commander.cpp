/*
* @file commander.cpp
* @brief Commander QMsm object
* @ingroup Commander QMsm object
* @cond
******************************************************************************
* Build date on  2016-10-20
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
#include "user.h"
#include "commander.h"

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
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* no initial action */
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
  : QMsm(Q_STATE_CAST(&Commander::initial))
{
    /* get commander msm */
    reQ = Requestor_getMsm();
    curWorkQnode = (TPCmdQueueNode)0;
    CmdQueue_init(&cmdQueue, (int)150);
}
/*initial().......................................................*/
QP::QState Commander::initial(Commander * const me,
        QP::QEvt const * const e)
{
    qDebug("Commander initial");
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &idle_s,
        {
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };
    /* initial msm */
    me->reQ->init();
    /* avoid not used */
    (void)e;
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
            qDebug("Commander(active) recieve COMMAND_SIG");
            cmdQNode = (static_cast<CommandEvt const * const>(e))->em;
            Q_ASSERT((TPCmdQueueNode)cmdQNode != (TPCmdQueueNode)0);
            /* push to command queue */
            if (0 != CmdQueue_push(&me->cmdQueue,
                    cmdQNode))
            {
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
            qDebug("[Commander(active) recieve  REQUEST_DONE_SIG(%d)]",
                me->curEReqId);
            /* proccessing other command will faild */
            Q_ASSERT(me->curWorkQnode != (TPCmdQueueNode)0
                && (static_cast<RequestDoneEvt const * const >(e))->rId\
                    == me->curEReqId);
            RequestDoneEvt er(me->curEReqId);
            me->reQ->dispatch(&er);
            /* User will make sure current executable command
                whether is ended. If not, generate another 
                request locally and put it to current work queue
                node and dispatch it to 'reQ'.Otherwise, release
                all current work queue node space, including all
                request information. */
            me->pReqElem = (TRequestElem *)Cmd_run(me->curWorkQnode);
            if ((me->pReqElem != (TRequestElem*)0)
                  && (me->curWorkQnode->elem.execStatus
                        == REQUEST_EXECUTING)) {
                /* command is not finishing, allot for request node */
                Q_ASSERT(me->curWorkQnode->elem.execStatus
                        == REQUEST_EXECUTING);
                pReq = (TPRequestNode)malloc(sizeof(TRequestNode));
                if (pReq != (TPRequestNode)0) {
                    /* initial request node */
                    RequestList_nodeInit(pReq, me->pReqElem);
                    /* push to link list */
                    RequestList_addTrail(&me->curWorkQnode->elem.requestHead,
                        pReq);
                    /* publish new request to requester */
                    RequestElemEvt ere(me->pReqElem);
                    me->reQ->dispatch(&ere);
                    /* update current executable request id */
                    me->curEReqId = me->pReqElem->id;
                    /* current request run done */
                    if (me->pReqElem->status == REQ_SUCCESS) {
                        /* run next request */
                        requestRun(me);
                    }
                    /* if command run successfully, change state machine
                         to idle */
                    if (me->curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                        /* run command in the local command working queue.
                             When current command be executed successfully, this
                             function will get next command and run it until all
                             command run completly or current command waiting
                             for running */
                        queueCmdRun(me);
                        if (me->qCmdExeStatus == CQE_SUCCESS) {
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
                    Q_ASSERT(pReq != (TPRequestNode)0);
                }
            }
            else if (me->curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                /* run command in the local command working queue.
                     When current command be executed successfully, this
                     function will get next command and run it until all
                     command run completly or current command waiting
                     for running */
                queueCmdRun(me);
                if (me->qCmdExeStatus == CQE_SUCCESS) {
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
    /* avoid unused */
    (void)me;
    return QM_ENTRY(&active_s);
}
/*active_x()................................................................*/
QP::QState Commander::active_x(Commander * const me) {
    /* avoid unused */
    (void)me;
    return QM_EXIT(&active_s);
}
/*idle()....................................................................*/
QP::QState Commander::idle(Commander * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    TPRequestNode pReq = (TPRequestNode)0;
    struct list_head *list_;
    switch (e->sig) {
        case COMMAND_SIG: {
            qDebug("[Commander(idle) recieve COMMAND_SIG]");
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
            me->curWorkQnode = (static_cast<CommandEvt const * const>(e))->em;
            Q_ASSERT(me->curWorkQnode != (TPCmdQueueNode)0);
            /* directly excecut Command function and don't need
                 to push to command working queue */
            me->pReqElem = (TRequestElem *)Cmd_run(me->curWorkQnode);
            if ((me->pReqElem != (TRequestElem*)0)
                  && (me->curWorkQnode->elem.execStatus
                            == REQUEST_EXECUTING))
            {
                /* local generate request */
                qDebug("[Command Requst Executing %d, %d]",
                    me->curWorkQnode->elem.id, me->curEReqId);
                /* command is not finishing, allot for request node */
                pReq = (TPRequestNode)malloc(sizeof(TRequestNode));
                if (pReq != (TPRequestNode)0) {
                    /* initial request node */
                    RequestList_nodeInit(pReq, me->pReqElem);
                    /* push to link list */
                    RequestList_addTrail(&me->curWorkQnode->elem.requestHead,
                        pReq);
                    /* publish new request to requester */
                    RequestElemEvt ere(me->pReqElem);
                    me->reQ->dispatch(&ere);
                    /* update current executable request id */
                    me->curEReqId = me->pReqElem->id;
                    /* current request run done */
                    if (me->pReqElem->status == REQ_SUCCESS) {
                        requestRun(me);
                    }
                    /* if current command node run successfully,
                         don't need to change state */
                    if (me->curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                        list_ = &me->curWorkQnode->elem.requestHead;
                        RequestList_destroy(list_);
                        /* release commander status list */
                        list_ = &me->curWorkQnode->elem.statusList;
                        RequestList_statusListDestroy(list_);
                        /* curWorkQnode must be released at last */
                        CmdQueue_nodeDestroy(&me->curWorkQnode);
                        /* set handled status */
                        status_ = QM_HANDLED();
                    }
                    else {
                        /* set active state to waiting for command finished */
                        status_ = QM_TRAN(&tatbl_);
                    }
                }
                else {/* error space to for request */
                    Q_ASSERT(pReq != (TPRequestNode)0);
                }
            }
            else if (me->curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
            /* current command be finished, user make decision to
                execute success!*/
                qDebug("[Command Finish %d, %d]",
                    me->curWorkQnode->elem.id, me->curEReqId);
                list_ = &me->curWorkQnode->elem.requestHead;
                RequestList_destroy(list_);
                /* release commander status list */
                list_ = &me->curWorkQnode->elem.statusList;
                RequestList_statusListDestroy(list_);
                /* curWorkQnode must be released at last */
                CmdQueue_nodeDestroy(&me->curWorkQnode);
                /* set status */
                status_ = QM_HANDLED();
            }
            else {
                /* other execStatus no need to do anything */
                qDebug("[%d Cmd(Req = %d) executing]",
                    me->curWorkQnode->elem.id, me->curEReqId);
                /* set active state to waiting for command finished */
                status_ = QM_TRAN(&tatbl_);
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
QP::QState Commander::idle_e(Commander * const me) {
    /* avoid unused */
    (void)me;
    return QM_ENTRY(&idle_s);
}
/*idle_x()..................................................................*/
QP::QState Commander::idle_x(Commander * const me) {
    /* avoid unused */
    (void)me;
    return QM_EXIT(&idle_s);
}
/*serving().................................................................*/
QP::QState Commander::serving(Commander * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = QM_HANDLED();
            break;
        }
    }
    /* avoid unused */
    (void)me;
    return status_;
}
/*$ Commander::requestRun().................................................*/
void Commander::requestRun(Commander * const me) {
    /* this function run request until request
         doing, output value is last request running
         'reqExeStatus' and transfered under prev
         request function successfully */
    TPRequestNode pReq = (TPRequestNode)0;
    RequestDoneEvt e(me->curEReqId);
    /* make sure requestor can run request */
    me->reQ->dispatch(&e);
    /* run command again */
    me->pReqElem = (TRequestElem *)Cmd_run(me->curWorkQnode);
    if (me->pReqElem != (TRequestElem*)0) {
        /* command is not finishing, allot for request node */
        Q_ASSERT(me->curWorkQnode->elem.execStatus == REQUEST_EXECUTING);
        pReq = (TPRequestNode)malloc(sizeof(TRequestNode));
        if (pReq != (TPRequestNode)0) {
            /* initial request node */
            RequestList_nodeInit(pReq, me->pReqElem);
            /* push to link list */
            RequestList_addTrail(&me->curWorkQnode->elem.requestHead,
                pReq);
            /* publish new request to requester */
            RequestElemEvt e_(me->pReqElem);
            me->reQ->dispatch(&e_);
            /* update current executable request id */
            me->curEReqId = me->pReqElem->id;
            /* current request run done */
            if (me->pReqElem->status == REQ_SUCCESS) {
                /* recursion request run until current request 
                     doing or there is no request generated by
                     Cmd_run() */
                requestRun(me);
            }
        }
        else {/* error space to for request */
            Q_ASSERT(pReq != (TPRequestNode)0);
        }
    }
}
/*$ Commander::queueCmdRun()................................................*/
void Commander::queueCmdRun(Commander * const me) {
/* current command be finished, user make decision to
    execute success!*/
    TPRequestNode pReq = (TPRequestNode)0;
    /* destroy request head list */
    RequestList_destroy(&me->curWorkQnode->elem.requestHead);
    /* release commander status list */
    RequestList_statusListDestroy(&me->curWorkQnode->elem.statusList);
    /* curWorkQnode must be released at last */
    CmdQueue_nodeDestroy(&me->curWorkQnode);

    /* get another command work queue node and excute it.
        If there is cmd node in Commander Queue, get it from
        queue and excute it ending until last Command exectable
        status not EXEC_SUCCESS or there is no workinng node in the
        Commander Queue */
    me->curWorkQnode = CmdQueue_pop(&me->cmdQueue);
    if (me->curWorkQnode != (TPCmdQueueNode)0) {
        me->pReqElem = (TRequestElem *)Cmd_run(me->curWorkQnode);
        if (me->pReqElem != (TRequestElem*)0) {
            /* command is not finishing, allot for request node */
            Q_ASSERT(me->curWorkQnode->elem.execStatus == REQUEST_EXECUTING);
            pReq = (TPRequestNode)malloc(sizeof(TRequestNode));
            if (pReq != (TPRequestNode)0) {
                /* initial request node */
                RequestList_nodeInit(pReq, me->pReqElem);
                /* push to link list */
                RequestList_addTrail(&me->curWorkQnode->elem.requestHead,
                    pReq);
                /* publish new request to requester */
                RequestElemEvt e(me->pReqElem);
                me->reQ->dispatch(&e);
                /* update current executable request id */
                me->curEReqId = me->pReqElem->id;
                /* current request run done */
                if (me->pReqElem->status == REQ_SUCCESS) {
                    requestRun(me);
                }
                /* set queue command current node executing is in progress*/
                if (me->curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
                    /* current command being finished and
                         execute next command */
                    me->queueCmdRun(me);
                }
                else {
                    me->qCmdExeStatus = CQE_DOING;
                }
            }
            else {/* error space to for request */
                Q_ASSERT(pReq != (TPRequestNode)0);
            }
        }
        else if (me->curWorkQnode->elem.execStatus == EXEC_SUCCESS) {
            /* executable next command */
            queueCmdRun(me);
        }
        else { /* other excutable status */
            /* set queue command current node executing is in progress */
            me->qCmdExeStatus = CQE_DOING;
        }
    }
    else {
        /* set queue command execute successfully */
        me->qCmdExeStatus = CQE_SUCCESS;
    }
}

} /* namespace ARCS */

