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

#include "request.h"
/*$ RequestList::addTrail().................................................*/
void RequestList_addTrail(struct list_head *list, TPRequestNode nw) {
    list_add_tail(&nw->list, list);
}
/*$ RequestList::addTrail().................................................*/
void RequestList_nodeInit(TPRequestNode n,TRequestElem *in) {
    LIST_HEAD_INIT(&in->statusList);
    n->in = in;
}
/*$ RequestList::statusListDestroy()........................................*/
void RequestList_statusListDestroy(struct list_head *list) {
    TPRStatusNode p1, p2;
    list_for_each_entry_safe(p1, p2, list, list) {
        __list_del_entry(p1);
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
         __list_del_entry(pos);
        /* release status node */
        RequestList_statusListDestroy(&pos->in->statusList);
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
        Q_ASSERT(pos->in != (TRequestElem *)0);
        if (pos->in->id == reqId) {
            work = pos;
            break;
        }
    }
    /* return value */
    return work;
}
/*$ RequestStatusList::nodeInsert().........................................*/
void RequestStatusList_nodeInsert(TPRequestNode reqNode,
    TRStatusNode const _staNode)
{
    list_add_tail(_staNode, &reqNode->in->statusList);
}
/*$ Request::saveStatusToList...............................................*/
void Request_saveStatusToList(TEHandledType hType, uint8_t type,
    uint16_t subtype, uint32_t reStatus, struct list_head *statusList)
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
            statusNode->hType = hType;
            statusNode->statusElem = status;
            list_add_tail(&statusNode->list, statusList);
        }
        else {
            free(statusNode);
        }
    }
}

