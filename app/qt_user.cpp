/*
* @file protocal_qt.c
* @brief protocal communicate with qt application
* @ingroup protocal with qt and server
* @cond
******************************************************************************
* Build Date on  2016-11-11
* Last updated for version 1.0.0
* Last updated on  2016-11-29
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
/*local variable of current command-----------------------------------------*/
static uint32_t l_curCmd;
static uint16_t l_seqNum;
/*$ qt mainsurface query id command function................................*/
void* QtCmd_queryId(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = 0x00; /* query */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_QUEUE_ID;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        reqId = ARCS::Controller_getNextReqId();
        pi = Inflight_nodeCreate(buf.seq, cmd,
            bufLen, reqId, ARCS::NOTIFY_FLAG,
            (uint32_t)100, (uint32_t)3, (uint8_t *)&buf);
        if (pi != (TInflightCmd_pNode)0) {
            Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, bufLen, (uint8_t *)&buf), (void*)0);
            pElem->execStatus = COMMAND_EXECUTING;
            /* when no real request node generate, must update
                 commander request id after get next request id */
            ARCS::Commander_updateCurrentReq(reqId);
            /* log executing command here */
            /* log cmd here */
            qDebug("[Qt User %d Cmd(Req = %d) executing]",
                    cmd, reqId);
        }
        else {
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
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        qDebug("[Qt User %d Cmd finish]",
                    cmd);
        /* set status to Finish */
        MainSurface::instance()->setStatus(LBSTATUS_FINISH);
        /* set result to success */
        MainSurface::instance()->setResult(LBRESULT_SCS);
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
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_SWITCH_MATRIX;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        reqId = ARCS::Controller_getNextReqId();
        pi = Inflight_nodeCreate(buf.seq, cmd,
            bufLen, reqId, ARCS::NOTIFY_FLAG,
            (uint32_t)100, (uint32_t)3, (uint8_t *)&buf);
        if (pi != (TInflightCmd_pNode)0) {
            Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, bufLen, (uint8_t *)&buf), (void*)0);
            pElem->execStatus = COMMAND_EXECUTING;
            /* when no real request node generate, must update
                 commander request id after get next request id */
            ARCS::Commander_updateCurrentReq(reqId);
            /* log executing command here */
            /* log cmd here */
        }
        else {
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
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(LBSTATUS_FINISH);
        /* set result to success */
        MainSurface::instance()->setResult(LBRESULT_SCS);
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
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_OPT_TMNL;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        reqId = ARCS::Controller_getNextReqId();
        pi = Inflight_nodeCreate(buf.seq, cmd,
            bufLen, reqId, ARCS::NOTIFY_FLAG,
            (uint32_t)100, (uint32_t)3, (uint8_t *)&buf);
        if (pi != (TInflightCmd_pNode)0) {
            Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, bufLen, (uint8_t *)&buf), (void*)0);
            pElem->execStatus = COMMAND_EXECUTING;
            /* when no real request node generate, must update
                 commander request id after get next request id */
            ARCS::Commander_updateCurrentReq(reqId);
            /* log executing command here */
            /* log cmd here */
        }
        else {
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
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(LBSTATUS_FINISH);
        /* set result to success */
        MainSurface::instance()->setResult(LBRESULT_SCS);
        /* unlock ui */
        MainSurface::instance()->unLockUi();
    }
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* QtCmd_setLocalSys(struct TCmdQElem *pElem) {
    uint32_t cmd;
    uint16_t bufLen;
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        if ((pElem->cmdBuf[0] & 0x01) == 0) { /* for local */
            if ((pElem->cmdBuf[0] & 0x7f) == 3) {/* change pwd */
                /* change password, this is not finished(20161121)*/
            }
            else if ((pElem->cmdBuf[0] & 0x7f) == 4) {/* change port */
                bufLen = pElem->cmdBuf[1];
                ARCS::A0_Transmitor->POST(Q_NEW(ARCS::PortChangeEvt,
                ARCS::QT_PORT, bufLen, pElem->cmdBuf + 2), (void*)0);
            }
            else {
                /* no need */
            }
            pElem->execStatus = EXEC_SUCCESS;
        }
        else {
            pElem->execStatus = EXEC_SUCCESS;
        }
    }
    else {
        pElem->execStatus = EXEC_SUCCESS;
    }
    /* set status to Finish */
    MainSurface::instance()->setStatus(LBSTATUS_FINISH);
    /* set result to success */
    MainSurface::instance()->setResult(LBRESULT_SCS);
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* QtCmd_setTerminalSys(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_SYS_SET;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        reqId = ARCS::Controller_getNextReqId();
        pi = Inflight_nodeCreate(buf.seq, cmd,
            bufLen, reqId, ARCS::NOTIFY_FLAG,
            (uint32_t)100, (uint32_t)3, (uint8_t *)&buf);
        if (pi != (TInflightCmd_pNode)0) {
            Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, bufLen, (uint8_t *)&buf), (void*)0);
            pElem->execStatus = COMMAND_EXECUTING;
            /* when no real request node generate, must update
                 commander request id after get next request id */
            ARCS::Commander_updateCurrentReq(reqId);
            /* log executing command here */
            /* log cmd here */
        }
        else {
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
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(LBSTATUS_FINISH);
        /* set result to success */
        MainSurface::instance()->setResult(LBRESULT_SCS);
        /* unlock ui */
        MainSurface::instance()->unLockUi();
    }
    /* not generate request local default*/
    return (void *)0;
}
/*$ */
void* QtCmd_cameraControl(struct TCmdQElem *pElem) {
    uint32_t cmd, reqId;
    uint16_t bufLen;
    TProtocalQt buf;
    TInflightCmd_pNode pi;
    /* assert 'NULL' pointer error */
    Q_ASSERT(pElem != (struct TCmdQElem *)0);
    cmd = pElem->id;
    if (l_curCmd != cmd) {/* first run this command */
        l_curCmd = cmd;
        buf.head = PROTOCAL_QT_TYPE;
        buf.type = PRO_SET_TYPE; /* set */
        buf.seq = ++l_seqNum;
        buf.cmd = QT_CMR_CTL;
        buf.dataLen = pElem->cmdBufLen;
        memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
        bufLen = buf.dataLen + PRO_COMMON_LEN;
        reqId = ARCS::Controller_getNextReqId();
        pi = Inflight_nodeCreate(buf.seq, cmd,
            bufLen, reqId, ARCS::NOTIFY_FLAG,
            (uint32_t)100, (uint32_t)3, (uint8_t *)&buf);
        if (pi != (TInflightCmd_pNode)0) {
            Inflight_nodeInsertTail(pi, ARCS::Controller_getQtInflight());
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, bufLen, (uint8_t *)&buf), (void*)0);
            pElem->execStatus = COMMAND_EXECUTING;
            /* when no real request node generate, must update
                 commander request id after get next request id */
            ARCS::Commander_updateCurrentReq(reqId);
            /* log executing command here */
            /* log cmd here */
        }
        else {
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
        /* get */
        /* set query command running successfully */
        pElem->execStatus = EXEC_SUCCESS;
        /* set status to Finish */
        MainSurface::instance()->setStatus(LBSTATUS_FINISH);
        /* set result to success */
        MainSurface::instance()->setResult(LBRESULT_SCS);          
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
    TProtocalQtQueryData *data;
    QString cnntStr, cnntCountStr, permisionStr;
    QString voteStr, signStr, avbStr;
    QString micStr, rgstStr, idStr;
    QString nameStr, selStr, gradeStr;
    QTableWidgetItem *idItem, *avbItem, *cnntItem,
        *cnntCountItem, *perItem, *micItem, *rgstItem,
        *signItem, *voteItem, *selItem, *gradeItem,
        *nameItem;
    QTableWidget *tableWidget;
    /* first run this command? */
    cmd = pElem->id;
    if (l_curCmd != cmd) {
        /* set current running command */
        l_curCmd = cmd;
        pBuf = (TProtocalQt *)pElem->cmdBuf;
        /* response for server request */
        if (!(pBuf->type & PRO_RESP_MASK)) {
            buf.head = PROTOCAL_QT_TYPE;
            buf.type = (0x00 & PRO_RESP_MASK);
            buf.seq = pBuf->seq;
            buf.cmd = QT_QUEUE_ID;
            buf.dataLen = 0;
            memcpy(buf.dataBuf, pElem->cmdBuf, buf.dataLen);
            bufLen = buf.dataLen + PRO_COMMON_LEN;
            ARCS::A0_Transmitor->POST(Q_NEW(ARCS::TransmitEvt,
                ARCS::QT_PORT, bufLen, (uint8_t *)&buf), (void*)0);
        }
        /* parser data */
        if (pBuf->type & PRO_REPORT_TYPE) {
        /* only proccess report type */
            dataLen = pBuf->dataLen;
            if (!(dataLen % sizeof(TProtocalQtQueryData))) {
                tableWidget = MainSurface::instance()->qresultTableWidget;
                tnum = (int)(dataLen / sizeof(TProtocalQtQueryData));
                for (i = 0; i < tnum; i++) {
                    pos = tnum*sizeof(TProtocalQtQueryData);
                    /* must not out of range */
                    Q_ASSERT(pos <= (PRO_QT_MAX - 78));
                    data = (TProtocalQtQueryData *)\
                        &pBuf->dataBuf[pos];
                    
                    avbStr.sprintf("0x%016llx", data->id_1722);
                    nameStr.sprintf("%s", data->name);
                    if (data->online) {
                        cnntStr.sprintf("%s", "C");/* Online */
                    }
                    else {
                        cnntStr.sprintf("%s", "D"); /* offline */
                    }
                    cnntCountStr.sprintf("%d", data->cnntNum);
                    row = tableWidget->rowCount();        /* insert new row */
                    tableWidget->insertRow(row);
                    if (data->avbIdentity) {/* host unit */
                        idStr.sprintf("%s", "无");
                        permisionStr.sprintf("%s", "host unit");
                    }
                    else {
                        idStr.sprintf("%d", data->id);
                        permisionStr.sprintf("%d", data->permision);
                        if (data->micStatus == MIC_CLOSE) {
                            micStr.sprintf("%s", "关闭");
                        }
                        else if (data->micStatus == MIC_OPEN) {
                            micStr.sprintf("%s", "打开");
                        }
                        else if (data->micStatus == MIC_FIRST_APPLY) {
                            micStr.sprintf("%s", "首位申请");
                        }
                        else if (data->micStatus == MIC_APPLY) {
                            micStr.sprintf("%s", "其他申请");
                        }
                        else {
                            micStr.sprintf("%s", "wrong status");
                        }

                        if (data->rgst) {
                            rgstStr.sprintf("%s", "已报到");
                        }
                        else {
                            rgstStr.sprintf("%s", "未报到");
                        }

                        if (data->sign) {
                            signStr.sprintf("%s", "已签到");
                        }
                        else {
                            signStr.sprintf("%s", "未签到");
                        }
                        if (data->vote) {
                            voteStr.sprintf("%s", "已表决");
                        }
                        else {
                            voteStr.sprintf("%s", "未表决");
                        }
                        if (data->select) {
                            selStr.sprintf("%s", "已投票");
                        }
                        else {
                            selStr.sprintf("%s", "未投票");
                        }
                        if (data->grade) {
                            gradeStr.sprintf("%s", "已评分");
                        }
                        else {
                            gradeStr.sprintf("%s", "未评分");
                        }

                        micItem = new QTableWidgetItem(micStr);
                        rgstItem = new QTableWidgetItem(rgstStr);
                        signItem = new QTableWidgetItem(signStr);
                        voteItem = new QTableWidgetItem(voteStr);
                        selItem = new QTableWidgetItem(selStr);
                        gradeItem = new QTableWidgetItem(gradeStr);
                    }

                    idItem = new QTableWidgetItem(idStr);
                    avbItem = new QTableWidgetItem(avbStr);
                    nameItem = new QTableWidgetItem(nameStr);
                    cnntItem = new QTableWidgetItem(cnntStr);
                    cnntCountItem = new QTableWidgetItem(cnntCountStr);
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

