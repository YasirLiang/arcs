/*
* @file protocal_qt.c
* @brief protocal communicate with qt application
* @ingroup protocal with qt and server
* @cond
******************************************************************************
* Build Date on  2016-11-11
* Last updated for version 1.0.0
* Last updated on  2016-12-2
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
#include "usertimer.h"
#include "inflight.h"
#include "qt_user.h"
#include "mainwidget.h"
#include <QTextCodec> 
/*local variable of current command-----------------------------------------*/
static uint32_t l_curCmd;
static uint16_t l_seqNum;
/* define user buffer size of qt */
#define QT_USER_BUF_SIZE 512
/*$ qt mainsurface query id command function................................*/
void* QtCmd_queryId(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    int flLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    uint8_t qtBuf[QT_USER_BUF_SIZE];
    bool failed;
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        memset(&buf, 0, sizeof(TProtocalQt));
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = 0x00; /* query */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_QUEUE_ID;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        failed = (bool)1;
        flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
        if (flLen != -1) {
            reqId = ARCS::Controller_getNextReqId();
            pi = Inflight_nodeCreate(buf.seq, cmd,
                flLen, reqId, ARCS::NOTIFY_FLAG,
                (uint32_t)500, (uint32_t)3, qtBuf);
            if (pi != (TInflightCmd_pNode)0) {
                Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                    ARCS::QT_PORT, flLen, qtBuf), (void*)0);
                pElem->execStatus = COMMAND_EXECUTING;
                failed = (bool)0;
                /* when no real request node generate, must update
                     commander request id after get next request id */
                ARCS::Commander_updateCurrentReq(reqId);
                /* log executing command here */
                /* log cmd here */
                qDebug("[Qt User %d Cmd(Req = %d) executing]",
                        cmd, reqId);
            }
        }
        
        if (failed) {
            pElem->execStatus = EXEC_SUCCESS;
            /* log error here */
            /* set command running error code */
            /* set status to Finish */
            MainSurface::instance()->setStatus(LBSTATUS_FINISH);
            /* set result */
            MainSurface::instance()->setResult(LBRESULT_FAILED);
            /* unLock Ui */
            MainSurface::instance()->unLockUi();
        }
    }
    else {
        char const *pS = LBSTATUS;
        char const *pR = LBRESULT;
        bool ok = (bool)0;
        TUptc ptc;
        /* \ check first command run status */
        ptc.subType = 0;
        ptc.type = QT_PTC;
        ok = RequestList_statusCorrect(&pElem->statusList,
            INFLIGHT_HD, ptc,
            QT_QUEUE_ID, ARCS::QSUCCESS);
        if (ok) {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_SCS;
        }
        else {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_FAILED;
        }

        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        qDebug("[Qt User %d Cmd finish]",
                    cmd);
        /* set status to Finish */
        MainSurface::instance()->setStatus(pS);
        /* set result to success */
        MainSurface::instance()->setResult(pR);
        /* unlock ui */
        MainSurface::instance()->unLockUi();
    }
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* QtCmd_switchMatrix(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    bool failed;
    int flLen;
    uint8_t qtBuf[QT_USER_BUF_SIZE];
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        memset(&buf, 0, sizeof(TProtocalQt));
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_SWITCH_MATRIX;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        failed = (bool)1;
        flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
        if (flLen != -1) {
            reqId = ARCS::Controller_getNextReqId();
            pi = Inflight_nodeCreate(buf.seq, cmd,
                (uint16_t)flLen, reqId, ARCS::NOTIFY_FLAG,
                (uint32_t)500, (uint32_t)3, qtBuf);
            if (pi != (TInflightCmd_pNode)0) {
                Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                    ARCS::QT_PORT, (uint16_t)flLen, qtBuf), (void*)0);
                failed = (bool)0;
                pElem->execStatus = COMMAND_EXECUTING;
                /* when no real request node generate, must update
                     commander request id after get next request id */
                ARCS::Commander_updateCurrentReq(reqId);
                /* log executing command here */
                /* log cmd here */
            }
        }

        if (failed) {
            pElem->execStatus = EXEC_SUCCESS;
            /* log error here */
            /* set command running error code */
            /* set status to Finish */
            MainSurface::instance()->setStatus(LBSTATUS_FINISH);
            /* set result */
            MainSurface::instance()->setResult(LBRESULT_FAILED);
            /* unLock Ui */
            MainSurface::instance()->unLockUi();
        }
    }
    else {
        char const *pS = LBSTATUS;
        char const *pR = LBRESULT;
        bool ok = (bool)0;
        TUptc ptc;
        /* \ check first command run status */
        ptc.subType = 0;
        ptc.type = QT_PTC;
        ok = RequestList_statusCorrect(&pElem->statusList,
            INFLIGHT_HD, ptc,
            QT_SWITCH_MATRIX, ARCS::QSUCCESS);
        if (ok) {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_SCS;
        }
        else {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_FAILED;
        }
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(pS);
        /* set result to success */
        MainSurface::instance()->setResult(pR);
        /* unlock ui */
        MainSurface::instance()->unLockUi();
    }
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* QtCmd_optTerminal(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    bool failed;
    int flLen;
    uint8_t qtBuf[QT_USER_BUF_SIZE];
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        memset(&buf, 0, sizeof(TProtocalQt));
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_OPT_TMNL;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        failed = (bool)1;
        flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
        if (flLen != -1) {
            reqId = ARCS::Controller_getNextReqId();
            pi = Inflight_nodeCreate(buf.seq, cmd,
                (uint16_t)flLen, reqId, ARCS::NOTIFY_FLAG,
                (uint32_t)500, (uint32_t)3, qtBuf);
            if (pi != (TInflightCmd_pNode)0) {
                Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                    ARCS::QT_PORT, (uint16_t)flLen, qtBuf), (void*)0);
                failed = (bool)0;
                pElem->execStatus = COMMAND_EXECUTING;
                /* when no real request node generate, must update
                     commander request id after get next request id */
                ARCS::Commander_updateCurrentReq(reqId);
                /* log executing command here */
                /* log cmd here */
            }
        }

        if (failed) {
            pElem->execStatus = EXEC_SUCCESS;
            /* log error here */
            /* set command running error code */
            /* set status to Finish */
            MainSurface::instance()->setStatus(LBSTATUS_FINISH);
            /* set result */
            MainSurface::instance()->setResult(LBRESULT_FAILED);
            /* unLock Ui */
            MainSurface::instance()->unLockUi();
        }
    }
    else {
        char const *pS = LBSTATUS;
        char const *pR = LBRESULT;
        bool ok = (bool)0;
        TUptc ptc;
        /* \ check first command run status */
        ptc.subType = 0;
        ptc.type = QT_PTC;
        ok = RequestList_statusCorrect(&pElem->statusList,
            INFLIGHT_HD, ptc,
            QT_OPT_TMNL, ARCS::QSUCCESS);
        if (ok) {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_SCS;
        }
        else {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_FAILED;
        }
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(pS);
        /* set result to success */
        MainSurface::instance()->setResult(pR);
        /* unlock ui */
        MainSurface::instance()->unLockUi();
    }
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* QtCmd_setTerminalSys(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    bool failed;
    int flLen;
    uint8_t qtBuf[QT_USER_BUF_SIZE];
    void *pRet  = (void *)0;
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        memset(&buf, 0, sizeof(TProtocalQt));
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_SYS_SET;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        failed = (bool)1;
        flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
        if (flLen != -1) {
            reqId = ARCS::Controller_getNextReqId();
            pi = Inflight_nodeCreate(buf.seq, cmd,
                (uint16_t)flLen, reqId, ARCS::NOTIFY_FLAG,
                (uint32_t)500, (uint32_t)3, qtBuf);
            if (pi != (TInflightCmd_pNode)0) {
                Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                    ARCS::QT_PORT, (uint16_t)flLen, qtBuf), (void*)0);
                failed = (bool)0;
                pElem->execStatus = COMMAND_EXECUTING;
                /* when no real request node generate, must update
                     commander request id after get next request id */
                ARCS::Commander_updateCurrentReq(reqId);
                /* log executing command here */
                /* log cmd here */
            }
        }

        if (failed) {
            pElem->execStatus = EXEC_SUCCESS;
            /* log error here */
            /* set command running error code */
            /* set status to Finish */
            MainSurface::instance()->setStatus(LBSTATUS_FINISH);
            /* set result */
            MainSurface::instance()->setResult(LBRESULT_FAILED);
            /* unLock Ui */
            MainSurface::instance()->unLockUi();
        }
    }
    else {
        char const *pS = LBSTATUS;
        char const *pR = LBRESULT;
        uint8_t sysCmd = pElem->cmdBuf[0];
        sysCmd &= 0xfe; /* high 7 bit*/
        sysCmd >>= 1;
        switch (sysCmd) {
            case 0: { /* stop */
                bool ok = (bool)0;
                TUptc ptc;
                /* \ check first command run status */
                ptc.subType = 0;
                ptc.type = QT_PTC;
                pElem->execStatus = EXEC_SUCCESS;
                pS = LBSTATUS_FINISH;
                ok = RequestList_statusCorrect(&pElem->statusList,
                        INFLIGHT_HD, ptc,
                        QT_SYS_SET, ARCS::QSUCCESS);
                if (ok) {
                    pR = LBRESULT_SCS;
                }
                else {
                    pR = LBRESULT_FAILED;
                }
                /* unlock ui */
                MainSurface::instance()->unLockUi();
                pRet = (void *)0;
                break;
            }
            case 1: { /* start */
                bool ok = (bool)0;
                TUptc ptc;
                /* \ check first command run status */
                ptc.subType = 0;
                ptc.type = QT_PTC;
                pElem->execStatus = EXEC_SUCCESS;
                pS = LBSTATUS_FINISH;
                ok = RequestList_statusCorrect(&pElem->statusList,
                        INFLIGHT_HD, ptc,
                        QT_SYS_SET, ARCS::QSUCCESS);
                if (ok) {
                    pR = LBRESULT_SCS;
                }
                else {
                    pR = LBRESULT_FAILED;
                }
                pR = LBRESULT_SCS;
                /* unlock ui */
                MainSurface::instance()->unLockUi();
                pRet = (void *)0;
                break;
            }
            case 2: { /* update */
                /* check command excuted successfully and
                     whether update system done.if not, 
                     generate request locally and return to commander */
                static TRequestElem l_reQ;
                static bool l_fset = (bool)1; /* first system set */
                uint32_t curReqId;
                bool ok = (bool)0;
                TUptc ptc;
                /* \ check first command run status */
                ptc.subType = 0;
                ptc.type = QT_PTC;
                ok = RequestList_statusCorrect(&pElem->statusList,
                    INFLIGHT_HD, ptc,
                    QT_SYS_SET, ARCS::QSUCCESS);
                
                if (!ok) {
                    /* set system set command running successfully */
                    pElem->execStatus = EXEC_SUCCESS;
                    pS = LBSTATUS_FINISH;
                    pR = LBRESULT_FAILED;
                    pRet = (void *)0;
                    /* unlock ui */
                    MainSurface::instance()->unLockUi();
                }
                else {
                    TPRequestNode pReq = (TPRequestNode)0;
                    if (l_fset) { /* first set */
                        l_fset = (bool)0; /* not first set */
                        /* \check transmit file whether finish,
                            \generate request until file transmited
                            \ completly */
                        uint8_t filedata[REQUEST_BUF_SIZE] = {0};
                        /* remaining lenght and load lenght */
                        uint64_t lLen = 0;
                        /* data pos to protocal buffer 2*/
                        lLen = MainSurface::instance()->\
                            loadUpdateData(filedata,
                                    REQUEST_BUF_SIZE - 2);
                        if (lLen > 0) {
                            l_reQ.id = ARCS::Controller_getNextReqId();
                            l_reQ.cmdId = pElem->id;
                            l_reQ.status = REQ_NO_START;
                            l_reQ.run = &QtReq_updateSystem;
                            l_reQ.type = SET_SYS;
                            l_reQ.user = QT_REQUEST;
                            memcpy(&l_reQ.buf, filedata, lLen);
                            l_reQ.buflen = (uint16_t)lLen;
                            pRet = (void *)&l_reQ;
                            pElem->execStatus = REQUEST_EXECUTING;
                            pS = LBSTATUS_EXCUTE;
                            pR = LBRESULT;
                        }
                        else {/*can't get here is meaning err */
                            l_fset = (bool)1;
                            /* set system set command running successfully */
                            pElem->execStatus = EXEC_SUCCESS;
                            pS = LBSTATUS_FINISH;
                            pR = LBRESULT_FAILED;
                            pRet = (void *)0;
                            MainSurface::instance()->unLockUi();
                        }
                    }
                    else { /* not first set */
                        ok = (bool)0;
                        curReqId = pElem->curEReq;
                        memset(&l_reQ, 0, sizeof(TRequestElem));
                        pReq = RequestList_searchNode(&pElem->requestHead,
                            curReqId);
                        if (pReq != (TPRequestNode)0) {
                            ok = (bool)1;
                        }

                        if (!ok) {
                            l_fset = (bool)1;
                            /* set system set command running successfully */
                            pElem->execStatus = EXEC_SUCCESS;
                            pS = LBSTATUS_FINISH;
                            pR = LBRESULT_FAILED;
                            pRet = (void *)0;
                            MainSurface::instance()->unLockUi();
                        }
                        else {
                            /* qDebug status */
                            ok = RequestList_statusAllCorrect(
                                        &pReq->in->statusList,
                                        INFLIGHT_HD, ARCS::QSUCCESS);
                            if (ok) {
                                /* \check transmit file whether finish,
                                    \generate request until file transmited
                                    \ completly.
                                    \If can get data, not transmit completly*/
                                uint8_t filedata[REQUEST_BUF_SIZE] = {0};
                                /* remaining lenght and load lenght */
                                uint64_t lLen = 0;
                                /* update proccess bar
                                     and confirm still upload */
                                MainSurface::instance()->\
                                    updateBarProccess();
                                /* data pos to protocal buffer 2*/
                                lLen = MainSurface::instance()->\
                                            loadUpdateData(filedata,
                                                REQUEST_BUF_SIZE - 2);
                                if (lLen > 0) {
                                    l_reQ.id =
                                        ARCS::Controller_getNextReqId();
                                    l_reQ.cmdId = pElem->id;
                                    l_reQ.status = REQ_NO_START;
                                    l_reQ.run = &QtReq_updateSystem;
                                    l_reQ.type = SET_SYS;
                                    l_reQ.user = QT_REQUEST;
                                    memcpy(&l_reQ.buf, filedata, lLen);
                                    l_reQ.buflen = (uint16_t)lLen;
                                    pRet = (void *)&l_reQ;
                                    pElem->execStatus = REQUEST_EXECUTING;
                                    pS = LBSTATUS_EXCUTE;
                                    pR = LBRESULT;
                                }
                                else {
                                    /*update success */
                                    l_fset = (bool)1;
                                    /* set system set command
                                         running successfully */
                                    pElem->execStatus = EXEC_SUCCESS;
                                    pS = LBSTATUS_FINISH;
                                    if (MainSurface::instance()->\
                                        isUpdateCancel())
                                    {
                                        pR = LBRESULT_FAILED;
                                    }
                                    else {
                                        pR = LBRESULT_SCS;
                                    }
                                    pRet = (void *)0;
                                    MainSurface::instance()->unLockUi();
                                }
                            }
                            else {
                                /*debug error code */
                                TPRStatusNode sts = (TPRStatusNode)0;
                                list_for_each_entry(sts,
                                    &pReq->in->statusList, list)
                                {
                                    if (sts->hType == INFLIGHT_HD) {
                                        TRequestState *pstElmt =
                                            (TRequestState *)0;
                                        pstElmt =
                                            (TRequestState *)(sts->statusElem);
                                        if (pstElmt != (TRequestState*)0) {
                                            if (pstElmt->reStatus
                                                != ARCS::QSUCCESS)
                                            {
                                                qDebug(">: reStatus = %d-<",
                                                    pstElmt->reStatus);
                                            }
                                        }
                                    }
                                }
                                l_fset = (bool)1;
                                /* set system set command 
                                    running successfully */
                                pElem->execStatus = EXEC_SUCCESS;
                                pS = LBSTATUS_FINISH;
                                pR = LBRESULT_FAILED;
                                pRet = (void *)0;
                                MainSurface::instance()->unLockUi();
                            }
                        }
                    }
                }
                break;
            }
            default :{
                break; /* error */
            }
        }
        /* set status to Finish */
        MainSurface::instance()->setStatus(pS);
        /* set result to success */
        MainSurface::instance()->setResult(pR);
    }
    /* not generate request local default*/
    return pRet;
}
/*$ */
void* QtCmd_cameraControl(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    bool failed;
    int flLen;
    uint8_t qtBuf[QT_USER_BUF_SIZE];
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        memset(&buf, 0, sizeof(TProtocalQt));
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_CMR_CTL;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        failed = (bool)1;
        flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
        if (flLen != -1) {
            reqId = ARCS::Controller_getNextReqId();
            pi = Inflight_nodeCreate(buf.seq, cmd,
                (uint16_t)flLen, reqId, ARCS::NOTIFY_FLAG,
                (uint32_t)500, (uint32_t)3, qtBuf);
            if (pi != (TInflightCmd_pNode)0) {
                Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                    ARCS::QT_PORT, (uint16_t)flLen, qtBuf), (void*)0);
                failed = (bool)0;
                pElem->execStatus = COMMAND_EXECUTING;
                /* when no real request node generate, must update
                     commander request id after get next request id */
                ARCS::Commander_updateCurrentReq(reqId);
                /* log executing command here */
                /* log cmd here */
            }
        }

        if (failed) {
            pElem->execStatus = EXEC_SUCCESS;
            /* log error here */
            /* set command running error code */
            /* set status to Finish */
            MainSurface::instance()->setStatus(LBSTATUS_FINISH);
            /* set result */
            MainSurface::instance()->setResult(LBRESULT_FAILED);
            /* unLock Ui */
            MainSurface::instance()->unLockUi();
        }
    }
    else {
        char const *pS = LBSTATUS;
        char const *pR = LBRESULT;
        bool ok = (bool)0;
        TUptc ptc;
        /* \ check first command run status */
        ptc.subType = 0;
        ptc.type = QT_PTC;
        ok = RequestList_statusCorrect(&pElem->statusList,
            INFLIGHT_HD, ptc,
            QT_CMR_CTL, ARCS::QSUCCESS);
        if (ok) {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_SCS;
        }
        else {
            pS = LBSTATUS_FINISH;
            pR = LBRESULT_FAILED;
        }
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(pS);
        /* set result to success */
        MainSurface::instance()->setResult(pR);          
        /* unlock ui */
        MainSurface::instance()->unLockUi();
    }
    /* not generate request local default*/
    return (void *)0;
}

/*$ Protocal command between server and ARCS */
void* ServerCmd_queryId(struct TCmdQElem *pElem) {
    uint32_t cmd;
    TProtocalQt buf, *pBuf;
    int row, i, tnum;
    uint16_t dataLen, pos;
    uint16_t bufLen;
    int flLen;
    int qAddr = -1, countCnnt = 0;
    uint8_t qtBuf[QT_USER_BUF_SIZE];
    TProtocalQtQueryData *data;
    QString cnntStr, permisionStr;
    QString voteStr, signStr, avbStr;
    QString micStr, rgstStr;
    QString nameStr, selStr, gradeStr;
    QTableWidgetItem *idItem, *avbItem, *cnntItem,
        *cnntCountItem, *perItem, *micItem, *rgstItem,
        *signItem, *voteItem, *selItem, *gradeItem,
        *nameItem;
    QTableWidget *tableWidget;
    /* first run this command? */
    cmd = pElem->id;
    pBuf = (TProtocalQt *)pElem->cmdBuf;
    if (l_curCmd != cmd) {
        /* set current running command */
        l_curCmd = cmd;
        /* response for server request */
        if (!(pBuf->type & PRO_RESP_MASK)) {
            memset(&buf, 0, sizeof(TProtocalQt));
            buf.head = PROTOCAL_QT_TYPE;
            buf.type = (0x00 | PRO_RESP_MASK);
            qDebug("query id seq response = %d", pBuf->seq);
            buf.seq = pBuf->seq;
            buf.cmd = QT_QUEUE_ID;
            buf.dataLen = 0;
            bufLen = buf.dataLen + PRO_COMMON_LEN;
            flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
            if (flLen != -1) {
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                    ARCS::QT_PORT, flLen, qtBuf), (void*)0);
            }
        }
        /* parser data */
        if (pBuf->type & PRO_REPORT_TYPE) {
        /* only proccess report type */
            dataLen = pBuf->dataLen;
            if (!(dataLen % sizeof(TProtocalQtQueryData))) {
                tableWidget = MainSurface::instance()->qresultTableWidget;
                tnum = (int)(dataLen / sizeof(TProtocalQtQueryData));
                for (i = 0; i < tnum; i++) {
                    pos = i*sizeof(TProtocalQtQueryData);
                    /* must not out of range */
                    Q_ASSERT(pos <= (PRO_QT_MAX - 78));
                    data = (TProtocalQtQueryData *)\
                        &pBuf->dataBuf[pos];
                    QString tempStr;
                    avbStr.sprintf("0x%016llx", data->id_1722);
                    nameStr.sprintf("%s", data->name);
                    if (data->online) {
                        cnntStr.sprintf("%s", "C");/* Online */
                    }
                    else {
                        cnntStr.sprintf("%s", "D"); /* offline */
                    }
                    countCnnt = data->cnntNum;
                    if ((data->avbIdentity == 1)
                         || (data->avbIdentity == 2))
                    {/* host unit */
                        qAddr = -1;
                        if (data->avbIdentity == 1) {
                            permisionStr.sprintf("%s", "Muticastor");
                        }
                        else {
                            permisionStr.sprintf("%s", "Common host unit");
                        }
                        
                        micStr.sprintf("%s", " ");
                        rgstStr.sprintf("%s", " ");
                        signStr.sprintf("%s", " ");
                        voteStr.sprintf("%s", " ");
                        selStr.sprintf("%s", " ");
                        gradeStr.sprintf("%s", " ");

                        micItem = new QTableWidgetItem(micStr);
                        rgstItem = new QTableWidgetItem(rgstStr);
                        signItem = new QTableWidgetItem(signStr);
                        voteItem = new QTableWidgetItem(voteStr);
                        selItem = new QTableWidgetItem(selStr);
                        gradeItem = new QTableWidgetItem(gradeStr);
                    }
                    else if (data->avbIdentity == 3) {
                        qAddr = data->id;
                        if (qAddr == 0xffff) {
                                qAddr = -1;
                        }
                        permisionStr.sprintf("%d", data->permision);
                        if (data->micStatus == MIC_CLOSE) {
                            micStr.sprintf("%s", "Close");
                        }
                        else if (data->micStatus == MIC_OPEN) {
                            micStr.sprintf("%s", "\u2713");
                        }
                        else if (data->micStatus == MIC_FIRST_APPLY) {
                            micStr.sprintf("%s", "First Apply");
                        }
                        else if (data->micStatus == MIC_APPLY) {
                            micStr.sprintf("%s", "Other Apply");
                        }
                        else {
                            micStr.sprintf("%s", "wrong status");
                        }

                        if (data->rgst) {
                            rgstStr.sprintf("%s", "\u2713");
                        }
                        else {
                            rgstStr.sprintf("%s", "unregister");
                        }

                        if (data->sign) {                            
                            signStr.sprintf("%s", "\u2713");
                        }
                        else {
                            signStr.sprintf("%s", "unsigned");
                        }
                        if (data->vote) {
                            voteStr.sprintf("%s", "\u2713");
                        }
                        else {
                            voteStr.sprintf("%s", "unvote");
                        }
                        if (data->select) {
                            selStr.sprintf("%s", "\u2713");
                        }
                        else {
                            selStr.sprintf("%s", "unselect");
                        }
                        if (data->grade) {
                            gradeStr.sprintf("%s", "\u2713");
                        }
                        else {
                            gradeStr.sprintf("%s", "ungrade");
                        }

                        micItem = new QTableWidgetItem(micStr);
                        rgstItem = new QTableWidgetItem(rgstStr);
                        signItem = new QTableWidgetItem(signStr);
                        voteItem = new QTableWidgetItem(voteStr);
                        selItem = new QTableWidgetItem(selStr);
                        gradeItem = new QTableWidgetItem(gradeStr);
                    }
                    else {
                        /* no other case */
                        continue;
                    }
                    if (qAddr == -1) {
                        idItem = new QTableWidgetItem("None");
                    }
                    else {
                        idItem = new QTableWidgetItem();
                        idItem->setData(Qt::DisplayRole, qAddr);
                    }
                    avbItem = new QTableWidgetItem(avbStr);
                    nameItem = new QTableWidgetItem(nameStr);
                    cnntItem = new QTableWidgetItem(cnntStr);
                    cnntCountItem = new QTableWidgetItem();
                    cnntCountItem->setData(Qt::DisplayRole, countCnnt);
                    perItem = new QTableWidgetItem(permisionStr);
                    row = tableWidget->rowCount();
                    tableWidget->insertRow(row);
                    tableWidget->setItem(row, 0, idItem);
                    tableWidget->setItem(row, 1, avbItem);
                    tableWidget->setItem(row, 2, nameItem);
                    tableWidget->setItem(row, 3, cnntItem);
                    tableWidget->setItem(row, 4, cnntCountItem);
                    tableWidget->setItem(row, 5, perItem);
                    tableWidget->setItem(row, 6, micItem);
                    tableWidget->setItem(row, 7, rgstItem);
                    tableWidget->setItem(row, 8, signItem);
                    tableWidget->setItem(row, 9, voteItem);
                    tableWidget->setItem(row, 10, selItem);
                    tableWidget->setItem(row, 11, gradeItem);
                    tableWidget->sortByColumn(0);
                }
            }
        }
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
    }
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* ServerCmd_switchMatrix(struct TCmdQElem *pElem) {
    /* set command running successfully */
    pElem->execStatus = EXEC_SUCCESS;
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* ServerCmd_optTerminal(struct TCmdQElem *pElem) {
    /* set command running successfully */
    pElem->execStatus = EXEC_SUCCESS;
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* ServerCmd_setTerminalSys(struct TCmdQElem *pElem) {
    /* set command running successfully */
    pElem->execStatus = EXEC_SUCCESS;
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* ServerCmd_cameraControl(struct TCmdQElem *pElem) {
    /* set command running successfully */
    pElem->execStatus = EXEC_SUCCESS;
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
int QtReq_updateSystem(struct TRequestElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    bool failed;
    int flLen;
    uint8_t qtBuf[QT_USER_BUF_SIZE] = {0};
    Q_ASSERT(pElem != (struct TRequestElem *)0);
    Q_ASSERT(pElem->buflen <= (PRO_QT_MAX - 2));
    memset(&buf, 0, sizeof(TProtocalQt));
    buf.head = PROTOCAL_QT_TYPE;
    buf.type = PRO_REPORT_TYPE; /* set */
    buf.seq = ++l_seqNum;
    buf.cmd = QT_SYS_SET;
    buf.dataBuf[0] = 0x05; /* update system */
    buf.dataBuf[1] = pElem->buflen;
    memcpy(buf.dataBuf+2, pElem->buf, pElem->buflen);
    buf.dataLen = pElem->buflen + 2;
    bufLen = buf.dataLen + PRO_COMMON_LEN;
    failed = (bool)1;
    flLen = ProtocalQt_Fill(&buf, bufLen, qtBuf, QT_USER_BUF_SIZE);
    if (flLen != -1) {
        /* set current request id */
        reqId = pElem->id;
        cmd = pElem->cmdId;
        pi = Inflight_nodeCreate(buf.seq, cmd,
            (uint16_t)flLen, reqId, ARCS::NOTIFY_FLAG,
            (uint32_t)500, (uint32_t)3, qtBuf);
        if (pi != (TInflightCmd_pNode)0) {
            Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, (uint16_t)flLen, qtBuf), (void*)0);
            failed = (bool)0;
            pElem->status = REQ_EXECUTING;
        }
    }
    
    if (failed) {
        /* end request */
        pElem->status = REQ_SUCCESS;
    }
    /* return default */
    return 0;
}

