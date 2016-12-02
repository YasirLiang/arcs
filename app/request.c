/*
* @file request.h
* @brief system request
* @ingroup system request
* @cond
******************************************************************************
* Build Date on  2016-11-2
* Last updated for version 1.0.0
* Last updated on  2016-11-2
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
/*Including file------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "request.h"
/*$ RequestList::addTrail().................................................*/
void RequestList_addTrail(struct list_head *list, TPRequestNode nw) {
    list_add_tail(&nw->list, list);
}
/*$ RequestList::addTrail().................................................*/
void RequestList_nodeInit(TPRequestNode n,TRequestElem *in) {
    INIT_LIST_HEAD(&in->statusList);
    n->in = in;
}
/*$ RequestList::statusCorrect()............................................*/
bool RequestList_statusCorrect(struct list_head *list,
    TEHandledType hT, TUptc ptc, uint32_t selfCmd, uint32_t reStatus)
{
    bool ok = (bool)0;
    TPRStatusNode sts = (TPRStatusNode)0;
    list_for_each_entry(sts, list, list) {
        if ((hT == INFLIGHT_HD)
              && (sts->hType == INFLIGHT_HD))
        {
            TRequestState *pstElmt = (TRequestState *)0;
            pstElmt = (TRequestState*)(sts->statusElem);
            if (pstElmt != (TRequestState*)0) {
                if ((pstElmt->ptc.subType == ptc.subType)
                      && (pstElmt->ptc.type == ptc.type)
                      && (pstElmt->selfCmd == selfCmd)
                      && (pstElmt->reStatus == reStatus))
                {
                    ok = (bool)1;
                    break;
                }
            }
        }
        else {
            /* when other status type define,
                must add corresponding code here(2016-12-1)*/
        }
    }
    return ok;
}
/*$ RequestList::statusAllCorrect().........................................*/
bool RequestList_statusAllCorrect(struct list_head *list,
    TEHandledType hT, uint32_t reStatus)
{
    bool noOk = (bool)0;
    TPRStatusNode sts = (TPRStatusNode)0;
    list_for_each_entry(sts, list, list) {
        if (sts->hType == INFLIGHT_HD) {
            TRequestState *pstElmt = (TRequestState *)0;
            pstElmt = (TRequestState *)(sts->statusElem);
            if (pstElmt != (TRequestState*)0) {
                if (pstElmt->reStatus != reStatus)
                {
                    noOk = (bool)1;
                    break;
                }
            }
        }
        else {
            /* when other status type define,
                must add corresponding code here(2016-12-1)*/
        }
    }
    /* include empty list, if list empty, return 0*/
    return (!noOk);
}
/*$ RequestList::statusListDestroy()........................................*/
void RequestList_statusListDestroy(struct list_head *list) {
    TPRStatusNode p1, p2;
    list_for_each_entry_safe(p1, p2, list, list) {
        __list_del_entry(&p1->list);
        /* release status element */
        if (p1->statusElem != (void *)0) {
            free(p1->statusElem);
            p1->statusElem = (void *)0;
        }
        free(p1);
        p1 = (TPRStatusNode)0;
    }
}
/*$ RequestList::addTrail().................................................*/
void RequestList_destroy(struct list_head *head) {
    TPRequestNode pos, n;
    /*!release each */
    list_for_each_entry_safe(pos, n, head, list) {
        /* Delect node from list */
         __list_del_entry(&pos->list);
        /* release status node */
        RequestList_statusListDestroy(&pos->in->statusList);
        if (pos->in != (TRequestElem *)0) {
            free(pos->in); /* for framework free */
        }
        pos->in = (TRequestElem*)0;
        /* Free space */
        free(pos);
        pos = (TPRequestNode)0;
    }
}
/*$ Request::run()..........................................................*/
int Request_run(TRequestElem * const pelem) {
    return pelem->run(pelem);
}

/*$ RequestList::searchNode()...............................................*/
TPRequestNode RequestList_searchNode(struct list_head *list, uint32_t reqId) {
    TPRequestNode pos, work = (TPRequestNode)0;
    list_for_each_entry(pos, list, list) {
        /* in must pointer request node elememt */
        if (pos->in != (TRequestElem *)0) {
            if (pos->in->id == reqId) {
                work = pos;
                break;
            }
        }
    }
    /* return value */
    return work;
}
/*$ RequestStatusList::nodeInsert().........................................*/
void RequestStatusList_nodeInsert(TPRequestNode reqNode,
    TPRStatusNode const _staNode)
{
    list_add_tail(&_staNode->list, &reqNode->in->statusList);
}
/*$ Request::saveStatusToList...............................................*/
void Request_saveStatusToList(TEHandledType hType, uint8_t type,
    uint16_t subtype, uint32_t reStatus,
    uint32_t selfCmd, struct list_head *statusList)
{
    TRequestState *status;
    TPRStatusNode statusNode;
    statusNode = (TPRStatusNode)malloc(sizeof(TRStatusNode));
    if (statusNode != (TPRStatusNode)0) {
        status = (TRequestState *)malloc(sizeof(TRequestState));
        if (status != (TRequestState *)0) {
            status->ptc.subType = subtype;
            status->ptc.type = type;
            status->reStatus = reStatus;
            status->selfCmd = selfCmd;
            statusNode->hType = hType;
            statusNode->statusElem = status;
            list_add_tail(&statusNode->list, statusList);
        }
        else {
            free(statusNode);
        }
    }
}

/*$ Request::Request_elemtUpdate............................................*/
void Request_elemtUpdate(TRequestElem const * const pIn,
    TRequestElem * const pT)
{
    pT->id = pIn->id;
    pT->cmdId = pIn->cmdId;
    pT->type = pIn->type;
    pT->user = pIn->user;
    pT->status = pIn->status;
    INIT_LIST_HEAD(&pT->statusList);
    pT->buflen = pIn->buflen;
    memset(pT->buf, 0, REQUEST_BUF_SIZE);
    memcpy(pT->buf, pIn->buf, pT->buflen);
    pT->run = pIn->run;
}

