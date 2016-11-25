/*
* @file
* @brief system setting dialog
* @ingroup system setting dialog
* @cond
******************************************************************************
* Last updated for version 1.0.0
* Last updated on  2016-10-20
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __SYSTEMSET_H__
#define __SYSTEMSET_H__

#include <QTcpSocket> 
#include <QByteArray>
#include "user.h"
#include "ui_systemset.h"

/*! class SystemSetDialog */
class SystemSetDialog : public QDialog, public Ui_SysSetDialog {
    Q_OBJECT
public:
    SystemSetDialog(QWidget * parent = 0);
    TExternPortVtbl vTable;
    void setTcpSocket(char const * const pip,
        int _port);
    static void init_s(void);
    static int send_s(void const * const, int);
    static int recv_s(void * const, int);
    static int destroy_s(void);
    uint8_t serialPort;
    static QString tcpIp;
    static int32_t tcpPort;
    static QTcpSocket tcpClient;
    static uint8_t *datagram;
    static uint8_t avail;
    static ARCS::PortEvt qtCltRdEvt;
private slots:
    void changePass(void);
    void changePort(void);
    void clientConnected(void);
    void clientDisConnected(void);
    void qtPortReady(void);
    void displayError(QAbstractSocket::SocketError socketErr);
} ;

#endif /* __SYSTEMSET_H__ */

