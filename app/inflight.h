/*
* @file
* @brief inflight command
* @ingroup inflight command
* @cond
******************************************************************************
* Build Date on  2016-10-25
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
#ifndef __INFLIGHT_H__
#define __INFLIGHT_H__
/*! c++ function define */
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
/*! Including list.h */
#include "list.h"

/*! struct TInflightCmdNode for linked list node */
typedef struct TInflightCmdNode {
    uint16_t cmdSeqId; /*! seq id */
    uint16_t dataLen; /*! data lenght */
    uint32_t cmdId; /*! command id */
    uint32_t cmdNotificationId;/*! notification id */
    uint32_t cmdNotificationFlag;/*! notification flag */
    uint32_t cmdTimeoutMs;/*! timeout ms */
    uint32_t timerCnt; /*! count for  timeout = send count */
    uint32_t cntTime; /*! count times */
    uint8_t *pBuf;/*! send buffer lenght */
    TUserTimer timer; /*! timer for inflight node */
    struct list_head list; /*! double linked list node */
}TInflightCmdNode, *TInflightCmd_pNode;
/*! Inflight_nodeCreate-----------------------------------------------------*/
TInflightCmdNode * Inflight_nodeCreate(uint16_t seqId,uint32_t cmdId,
    uint16_t dataLen, uint32_t notifyId, uint32_t notifyFlag,
    uint32_t timeoutMs, uint32_t cntTime, uint8_t *pBuf);
/*! Inflight_nodeInsert-----------------------------------------------------*/
void Inflight_nodeInsertTail(TInflightCmd_pNode pInNode,
    struct list_head *head);
/*! Inflight_nodeDelect-----------------------------------------------------*/
void Inflight_nodeDelect(TInflightCmd_pNode pDelNode);
/*! Inflight_nodeDestroy----------------------------------------------------*/
void Inflight_nodeDestroy(TInflightCmd_pNode *pDtyNode);
/*! Inflight_startTimer-----------------------------------------------------*/
void Inflight_startTimer(TInflightCmd_pNode const pNode);
/*! Inflight_frame----------------------------------------------------------*/
uint8_t* Inflight_frame(TInflightCmd_pNode const pNode);
/*! Inflight_notificationFlag-----------------------------------------------*/
uint32_t Inflight_notificationFlag(TInflightCmd_pNode const pNode);
/*! Inflight_timeout-------------------------------------------------------*/
bool Inflight_timeout(TInflightCmd_pNode const pNode);
/*! Inflight_retried--------------------------------------------------------*/
bool Inflight_retried(TInflightCmd_pNode const pNode);
/*! Inflight_searchSeq------------------------------------------------------*/
TInflightCmd_pNode Inflight_searchSeq(struct list_head *head, uint16_t seq);
/*! Inflight_increaseSendCnt------------------------------------------------*/
bool Inflight_increaseSendCnt(TInflightCmd_pNode const pNode);
/*! Inflight_searchReq------------------------------------------------------*/
TInflightCmd_pNode Inflight_searchReq(struct list_head *head, uint32_t req);

#ifdef __cplusplus
} /*end extern "C" */
#endif /* __cplusplus */

#endif /* __INFLIGHT_H__ */

