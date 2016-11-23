/*
* @file circular_link_queue.h
* @brief circular link queue
* @ingroup circular link queue
* @cond
******************************************************************************
* Build Date on  2016-11-1
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
#ifndef __CIRCULAR_LINK_QUEUE_H__
#define __CIRCULAR_LINK_QUEUE_H__
/*! make a destinction between c and c++ file */
#ifdef __cplusplus /* only define in .cpp file */
    extern "C" { /*! for c++ file transfer c function */
#endif /* __cplusplus */
/*! Queue Node type */
typedef struct TQueueNode {
	struct TQueueNode *next;
}TQueueNode, *TPQueueNode;
/*! Queue type */
typedef struct TQueue {
	TPQueueNode head;
	TPQueueNode trail;
}TQueue, *TPQueue;
/*! Queue_getValue----------------------------------------------------------*/
extern TPQueueNode Queue_getValue(TPQueue root);
/*! Queue_pop---------------------------------------------------------------*/
extern TPQueueNode Queue_pop(TPQueue root);
/*! Queue_push--------------------------------------------------------------*/
extern void Queue_push(TPQueue root, TPQueueNode node);
/*! Queue_isEmpty-----------------------------------------------------------*/
extern int Queue_isEmpty(TPQueue root);
/*! Queue_init--------------------------------------------------------------*/
extern void Queue_init(TPQueue root);
/*! Queue_getSize-----------------------------------------------------------*/
extern int Queue_getSize(TPQueue root);
#ifdef __cplusplus
} /*end extern "C" */
#endif /* __cplusplus */

#endif /* __CIRCULAR_LINK_QUEUE_H__ */

