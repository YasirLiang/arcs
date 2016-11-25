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
#ifndef __REQUEST_H__
#define __REQUEST_H__
/*! make a destinction between c and c++ file */
#ifdef __cplusplus /* only define in .cpp file */
    extern "C" { /*! for c++ file transfer c function */
#endif /* __cplusplus */

/*! Macro request buffer size-----------------------------------------------*/
#define REQUEST_BUF_SIZE 256

/*! User Request type */
typedef enum TRequestType {
    QUERY_ID, /*! query terminal from server */
    SWITCH_MATRIX, /*! switch matrix */
    OPT_TERMINAL, /*! terminal operation */
    SET_SYS, /*! system setting including local and server */
    CAMERA_CONTROL, /*! control server camera */
    QT_MAX_PUB /*! QT request command max number */
}TRequestType;
/*! the identify of user Request */
typedef enum TRequestUser {
    QT_REQUEST, /*! qt surface user request */
    MENU_REQUEST, /*! menu surface user request */
    CMPT_REQUEST, /*! upper computer operation */
    CMD_REQUEST, /*! command line request */
    TERMINAL_REQUEST /*! terminal request */
}TRequestUser;
/*! the execute status of Request */
typedef enum TReqExeStatus {
    REQ_SUCCESS,   /*! executed successful */
    REQ_DOING,     /*! request doing */
    REQ_TERMINATE, /*! request terminate */
}TReqExeStatus;
/*! Request information struct define */
typedef struct TRequestElem {
    uint32_t id; /*! allot by commander QMsm */
    uint32_t cmdId; /*! the attribution of request for command id */
    TRequestType type; /*! type of request command */
    TRequestUser user; /*! the user of request executing */
    TReqExeStatus status; /*!executable status for current request */
    /*! request executable status list, node type is TRStatusNode */
    struct list_head statusList;
    uint16_t buflen; /*! request buffer length */
    uint8_t buf[REQUEST_BUF_SIZE]; /*! buffer of request */
    /* specific request run function */
    int (*run)(struct TRequestElem *elem);
}TRequestElem;
/*! Handled Request  type */
typedef enum TEHandledType {
    INVALID_HD, /* invalid handle type */
    INFLIGHT_HD, /*! inflight handled */
    LOFLIGHT_HD, /*! local inflight handled */
    HANDLED_TYPE_MAX
}TEHandledType;
/*! request type and subtype define as follow */
#define QT_PTC 0x01
#define TMNL_PTC 0x02
#define CMPT_PTC 0x03
#define PTC_1722_1 0x04
/*! adp subtye 0x0100 not including 1722.1 */
#define PTC_1722_1_ADP 0x0104
/*! adp subtye 0x0200 not including 1722.1 */
#define PTC_1722_1_ACMP 0x0204
/*! adp subtye 0x0300 not including 1722.1 */
#define PTC_1722_1_AECP 0x0304
/*! request status */
typedef struct TRequestState {
    union {
        uint16_t subType; /*! subtype of transfer protocal */
        uint8_t type;     /*! type of transfer protocal */
    }ptc;
    uint32_t reStatus;    /*! response status */
}TRequestState;
/*! Request executable status */
typedef struct TRStatusNode {
    TEHandledType hType; /*! handled type */
    /*! support for all status of request execute,
        pointer to each specific request  status, get it
        basing on handled type and it space must be 
        alloted by malloc() function */
    void *statusElem;
    struct list_head list;/*! double link list node */
}TRStatusNode, *TPRStatusNode;
/*! Requst double link list node define */
typedef struct TRequestNode {
    TRequestElem *in;            /*! point to request node information */
    struct list_head list;       /*! double link list node */
}TRequestNode, *TPRequestNode;
/*! RequestList_addTrail----------------------------------------------------*/
void RequestList_addTrail(struct list_head *list, TPRequestNode nw);
/*! RequestList_nodeInit----------------------------------------------------*/
void RequestList_nodeInit(TPRequestNode n,TRequestElem *in);
/*! RequestList_statusListDestroy-------------------------------------------*/
void RequestList_statusListDestroy(struct list_head *list);
/*! RequestList_destroy-----------------------------------------------------*/
void RequestList_destroy(struct list_head *head);
/*! Request_run-------------------------------------------------------------*/
int Request_run(TRequestElem * const pelem);
/*! RequestList_searchNode--------------------------------------------------*/
TPRequestNode RequestList_searchNode(struct list_head *list, uint32_t reqId);
/*! RequestStatusList_nodeInsert--------------------------------------------*/
void RequestStatusList_nodeInsert(TPRequestNode reqNode,
    TPRStatusNode const _staNode);
/*! Request_saveStatusToList------------------------------------------------*/
void Request_saveStatusToList(TEHandledType hType, uint8_t type,
    uint16_t subtype, uint32_t reStatus, struct list_head *statusList);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* __REQUEST_H__ */

