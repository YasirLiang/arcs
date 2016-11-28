/*
* @file
* @brief inflight command
* @ingroup inflight command
* @cond
******************************************************************************
* Build Date on  2016-10-25
* Last updated for version 1.0.0
* Last updated on  2016-10-25
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "usertimer.h"
#include "inflight.h"
/*! Inflight_nodeCreate.....................................................*/
TInflightCmdNode * Inflight_nodeCreate(uint16_t seqId, uint32_t cmdId,
        uint16_t dataLen, uint32_t notifyId, uint32_t notifyFlag,
        uint32_t timeoutMs, uint32_t cntTime, uint8_t *pBuf)
{
    /*after transfer, user must judgment 
        whether the value is not NULL.And data buffer space must release
        after inflight node not used */
    TInflightCmd_pNode pNode = (TInflightCmd_pNode)0;
    /* malloc for TInflightCmdNode*/
    pNode = (TInflightCmd_pNode)malloc(sizeof(TInflightCmdNode));
    if (pNode != (TInflightCmd_pNode)0) { /* allot successfule */
        pNode->cmdNotificationFlag = notifyFlag;
        pNode->cmdNotificationId = notifyId;
        pNode->cmdId = cmdId;
        pNode->cmdSeqId = seqId;
        pNode->cmdTimeoutMs = timeoutMs;
        pNode->cntTime = cntTime;
        pNode->dataLen = dataLen;
        pNode->timerCnt = (uint32_t)1; /* send count */
        pNode->pBuf = (uint8_t *)malloc(dataLen);
        if (pNode->pBuf != (uint8_t *)0) {
            /* copy data */
            memcpy(pNode->pBuf, pBuf, dataLen);
            userTimerStart(timeoutMs, &pNode->timer);
        }
        else {
            /* buffer malloc not success */
            free(pNode);
            pNode = (TInflightCmd_pNode)0;
        }
    }
        
    /* return pNode, NULL is create not success */
    return pNode;
}
/*! Inflight_nodeInsert.....................................................*/
void Inflight_nodeInsertTail(TInflightCmd_pNode pInNode,
    struct list_head *head)
{
    list_add_tail(&pInNode->list, head);
}
/*! Inflight_nodeDelect.....................................................*/
void Inflight_nodeDelect(TInflightCmd_pNode pDelNode) {
    __list_del(pDelNode->list.prev, pDelNode->list.next);
}
/*! Inflight_nodeDestroy....................................................*/
void Inflight_nodeDestroy(TInflightCmd_pNode *pDtyNode) {
    if (*pDtyNode != (TInflightCmd_pNode)0) {
        Inflight_nodeDelect(*pDtyNode);
        if ((*pDtyNode)->pBuf != (uint8_t *)0) {
            free((*pDtyNode)->pBuf);
            (*pDtyNode)->pBuf = (uint8_t *)0;
        }
        free(*pDtyNode);
        *pDtyNode = (TInflightCmd_pNode)0;
    }
}
/*! Inflight_startTimer.....................................................*/
void Inflight_startTimer(TInflightCmd_pNode const pNode) {
    userTimerStart(pNode->cmdTimeoutMs, &pNode->timer);
}
/*! Inflight_updateTimer....................................................*/
void Inflight_updateTimer(TInflightCmd_pNode const pNode,
    uint32_t timeMs)
{
    pNode->cmdTimeoutMs = timeMs;
    userTimerStart(timeMs, &pNode->timer);
}
/*! Inflight_frame..........................................................*/
uint8_t* Inflight_frame(TInflightCmd_pNode const pNode) {
    return pNode->pBuf;
}
/*! Inflight_notificationFlag...............................................*/
uint32_t Inflight_notificationFlag(TInflightCmd_pNode const pNode) {
    return pNode->cmdNotificationFlag;
}
/*! Inflight_timeout........................................................*/
bool Inflight_timeout(TInflightCmd_pNode const pNode) {
    return userTimerTimeout(&pNode->timer);
}
/*! Inflight_retried........................................................*/
bool Inflight_retried(TInflightCmd_pNode const pNode) {
    return (pNode->timerCnt >= pNode->cntTime);
}
/*! Inflight_increaseSendCnt................................................*/
bool Inflight_increaseSendCnt(TInflightCmd_pNode const pNode) {
    return pNode->timerCnt++;
}
/*! Inflight_searchSeq......................................................*/
TInflightCmd_pNode Inflight_searchSeq(struct list_head *head, uint16_t seq) {
    TInflightCmd_pNode pos;
    list_for_each_entry(pos, head, list) {
        if (pos->cmdSeqId == seq) {
            return pos;
        }
    }
    return (TInflightCmd_pNode)0;
}

/*! Inflight_searchReq......................................................*/
TInflightCmd_pNode Inflight_searchReq(struct list_head *head, uint32_t req) {
    TInflightCmd_pNode pos;
    list_for_each_entry(pos, head, list) {
        if (pos->cmdNotificationId == req) {
            return pos;
        }
    }
    return (TInflightCmd_pNode)0;
}

