/*
* @file extern_port_qt.h
* @brief qt specific exern port
* @ingroup qt specific exern port
* @cond
******************************************************************************
* Build Date on  2016-11-22
* Last updated for version 1.0.0
* Last updated on  2016-11-22
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __EXTERN_PORT_QT_H__
#define __EXTERN_PORT_QT_H__
/* class extern port of for qt port */
class ExternPortQt {
public:
    TExternPortVtbl vTable;
    ExternPortQt();
    void initial(void);
    int send(void const * const buf, int len);
    int recv(void * const buf, int len);
    int destroy(void);
    TExternPortVtbl const *getVtbl(void);
};

#endif /* __REQUEST_PORT_H__ */

