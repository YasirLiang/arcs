/*
* @file command.c
* @brief system command
* @ingroup system command
* @cond
******************************************************************************
* Build Date on  2016-11-2
* Last updated for version 1.0.0
* Last updated on  2016-11-28
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h> /*! memset/memcpy declaration */
#include "list.h"
#include "request.h"
#include "command.h"

/*! CmdQueue_getValue.......................................................*/
TPCmdQueueNode CmdQueue_getValue(TPCmdQueue root) {
    return (TPCmdQueueNode)Queue_getValue((TPQueue)root);
}
/*! CmdQueue_pop............................................................*/
TPCmdQueueNode CmdQueue_pop(TPCmdQueue root) {
    TPCmdQueueNode p = (TPCmdQueueNode)0;
    if (root->queueCount > (int)0) {
        p = (TPCmdQueueNode)Queue_pop((TPQueue)root);
        if (p != (TPCmdQueueNode)0) {
            root->queueCount--;
        }
    }
    /* if p = NULL, failed pop */
    return p;
}
/*! CmdQueue_push...........................................................*/
int CmdQueue_push(TPCmdQueue root, TPCmdQueueNode node) {
    Queue_push((TPQueue)root, (TPQueueNode)node);
    if (root->queueCount < root->queueSize) {
        root->queueCount++;
        return root->queueCount;
    }
    return 0; /* zero is push failed */
}
/*! CmdQueue_isEmpty........................................................*/
int CmdQueue_isEmpty(TPCmdQueue root) {
    return Queue_isEmpty((TPQueue)root);
}
/*! CmdQueue_init...........................................................*/
void CmdQueue_init(TPCmdQueue root, int size) {
    Queue_init((TPQueue)root);
    root->queueCount = 0;
    root->queueSize= size;
}
/*! CmdQueue_getSize........................................................*/
int CmdQueue_getSize(TPCmdQueue root) {
    return root->queueSize;
}
/*! CmdQueue_getSize........................................................*/
int CmdQueue_getCount(TPCmdQueue root) {
    return root->queueCount;
}
/*! CmdQueue_elemInitial....................................................*/
void CmdQueue_elemInitial(TCmdQElem *elem, uint32_t cmdId,
    TECmdType type, TECmdUser user, uint16_t dataLen,
    uint8_t const * const buf, TPSpeCmdFunc const pSpeF)
{
    elem->id = cmdId;
    elem->type = type;
    elem->user = user;
    elem->execStatus = NO_START;
    INIT_LIST_HEAD(&elem->requestHead);
    INIT_LIST_HEAD(&elem->statusList);
    /* request data length set */
    elem->cmdBufLen = dataLen;
    memset(elem->cmdBuf, 0, USER_PROTOCAL_SIZE);
    if ((dataLen > 0)
          &&(buf != (uint8_t const * const)0))
    {
        /* set request data to cmd buffer */
        memcpy(elem->cmdBuf, buf, dataLen);
    }
    elem->run = pSpeF;
}
/*! CmdQueue_nodeDestroy....................................................*/
void CmdQueue_nodeDestroy(TPCmdQueueNode *tg) {
    if (*tg != (TPCmdQueueNode)0) {
        free(*tg);
        *tg = (TPCmdQueueNode)0;
    }
}
/*! Cmd_run.................................................................*/
void* Cmd_run(TPCmdQueueNode instance)  {
    return (void *)instance->elem.run(&instance->elem);
}

