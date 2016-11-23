/*
* @file
* @brief circular link TQueue
* @ingroup circular link TQueue
* @cond
******************************************************************************
* Build Date on  2016-11-1
* Last updated for version 1.0.0
* Last updated on  2016-11-1
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
#include "circular_link_queue.h"
/*Local function decralation------------------------------------------------*/
static int Queue_getLenght(TPQueue root);
/*! Queue_getValue..........................................................*/
TPQueueNode Queue_getValue(TPQueue root) {
    TPQueueNode p;
    if (root == (TPQueue)0) {
        return (TPQueueNode)0;
    }
    p = root->head;
    return p;
}
/*! Queue_pop...............................................................*/
TPQueueNode Queue_pop(TPQueue root) {
    TPQueueNode pNode;
    if (root == (TPQueue)0) {
        return (TPQueueNode)0;
    }
    
    pNode = root->head;
    if (!Queue_isEmpty(root)) {
        root->head = root->head->next;
    }
    else {
        /* queue empty is trail and head are zero */
        if (root->trail != (TPQueueNode)0) {
            root->trail = (TPQueueNode)0;
        }
    }

    return pNode;
}
/*! Queue_push..............................................................*/
void Queue_push(TPQueue root, TPQueueNode node) {
    if (root == (TPQueue)0) {
        return;
    }
    
    if (Queue_isEmpty(root)) {
        /* when queue is empty, trail is zero */
        if (root->trail != (TPQueueNode)0) {
            root->trail = (TPQueueNode)0;
        }
    }

    node->next = (TPQueueNode)0;
    if ((TPQueueNode)0 != root->trail) {
        /* new node inserted to trail */
        root->trail->next = node;
    }

    root->trail = node;
    if ((TPQueueNode)0 == root->head) {
        root->head = node;
    }
}
/*! Queue_isEmpty...........................................................*/
int Queue_isEmpty(TPQueue root) {
    if (root == (TPQueueNode)0) {
        return 1;
    }
    return ((root->head == (TPQueueNode)0)?(int)1:(int)0);
}
/*! Queue_getLenght.........................................................*/
static int Queue_getLenght(TPQueue root) {
    int l;
    TPQueueNode qNode; /* queue Node */
    if (root == (TPQueue)0) {
        return 1;
    }
    
    l = 0;
    if (!Queue_isEmpty(root)) {
        qNode = root->head;
        do {
            l++;
            qNode = qNode->next;
        }while (qNode != (TPQueueNode)0);	
    }

    return l;
}
/*! Queue_init..............................................................*/
void Queue_init(TPQueue root) {
    root->head = (TPQueueNode)0;
    root->trail = (TPQueueNode)0;
}
/*! Queue_getSize...........................................................*/
int Queue_getSize(TPQueue root) {
    if (root == (TPQueue)0) {
        return 0;
    }
    return Queue_getLenght(root);
}

