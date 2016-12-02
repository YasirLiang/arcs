/*
* @file command.h
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
#ifndef __COMMAND_H__
#define __COMMAND_H__
#include "circular_link_queue.h"
/*! make a destinction between c and c++ file */
#ifdef __cplusplus /* only define in .cpp file */
    extern "C" {   /*! for c++ file transfer c function */
#endif /* __cplusplus */
/*! enumration commander type */
typedef enum TECmdType {
    USER_CMD, /*! user command type */
    SYSTEM_CMD /*! for system command type */
}TECmdType;
/*! enumration user type */
typedef enum TECmdUser {
    QT_USER,       /*! qt surface user Command */
    SERVER_USER,   /*! only using bewteen ARCS and server */
    MENU_USER,     /*! menu surface user Command */
    CMPT_USER,     /*! upper computer operation */
    CMD_USER,      /*! command line Command */
    TERMINAL_USER, /*! terminal Command */
    SYSTEM_USER    /*! system user */
}TECmdUser;
/*! enumration command status */
typedef enum TECmdStatus {
    EXEC_SUCCESS = 0,  /*! Only for all request excute success */
    NO_START,          /*! no start  */
    EXEC_PAUSE,        /*! pause by user */
    USER_TERMINATE,    /*! terminate by user */
    COMMAND_EXECUTING, /*! command excuting while no request */
    REQUEST_EXECUTING  /*! attribution request Executing */
}TECmdStatus;

#define USER_PROTOCAL_SIZE 320

/*! user cmd queue element */
typedef struct TCmdQElem {
    uint32_t id;                  /*! local generate id */
    uint32_t curEReq;             /*! current excute Req id */
    TECmdType type;               /*! type for command */
    TECmdUser user;               /*! user for command */
    TECmdStatus execStatus;       /*! command execute status */
    struct list_head requestHead; /*! requst double link list guard */
    /*! request executable status list, node type is TRStatusNode */
    struct list_head statusList;
    uint16_t cmdBufLen; /*! cmdBufLen for protocal data */
    uint8_t cmdBuf[USER_PROTOCAL_SIZE]; /*! cmd buffer for protocal */
    /*! command run specific function, return new request,
        zero mean return nothing */
    void* (*run)(struct TCmdQElem *pElem);
}TCmdQElem;

/*! typedef TCmdQueueNode */
typedef struct TCmdQueueNode {
    TPQueueNode next; /*! point to next node */
    TCmdQElem elem; /*! information cmd in queue node */
}TCmdQueueNode, *TPCmdQueueNode;
/*! cmd queue define as user using */
struct TCmdQueue {
    TQueue queue;   /*! queue element */
    int queueSize;  /*! queue max size */
    int queueCount; /*! queue count number */
    /* other queue element deration can be hear, such as control data */
};
/*! typedef TCmdQueue */
typedef struct TCmdQueue TCmdQueue, *TPCmdQueue;
/*! typedef command function */
typedef void* (*TPSpeCmdFunc)(struct TCmdQElem *);

/*! CmdQueue_getValue-------------------------------------------------------*/
TPCmdQueueNode CmdQueue_getValue(TPCmdQueue root);
/*! CmdQueue_pop------------------------------------------------------------*/
TPCmdQueueNode CmdQueue_pop(TPCmdQueue root);
/*! CmdQueue_push-----------------------------------------------------------*/
int CmdQueue_push(TPCmdQueue root, TPCmdQueueNode node);
/*! CmdQueue_isEmpty--------------------------------------------------------*/
int CmdQueue_isEmpty(TPCmdQueue root);
/*! CmdQueue_init-----------------------------------------------------------*/
void CmdQueue_init(TPCmdQueue root, int size);
/*! CmdQueue_getSize--------------------------------------------------------*/
int CmdQueue_getSize(TPCmdQueue root);
/*! CmdQueue_getCount-------------------------------------------------------*/
int CmdQueue_getCount(TPCmdQueue root);
/*! CmdQueue_elemInitial----------------------------------------------------*/
void CmdQueue_elemInitial(TCmdQElem *elem, uint32_t cmdId,
    TECmdType type, TECmdUser user, uint16_t dataLen,
    uint8_t const * const buf, TPSpeCmdFunc const pSpeF);
/*! CmdQueue_updateCurReq---------------------------------------------------*/
void CmdQueue_updateCurReq(TPCmdQueueNode const pIn,
    uint32_t const reqId);
/*! CmdQueue_nodeDestroy----------------------------------------------------*/
void CmdQueue_nodeDestroy(TPCmdQueueNode *tg);
/*! Cmd_run-----------------------------------------------------------------*/
void* Cmd_run(TPCmdQueueNode instance);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* __COMMAND_QUEUE_H__ */

