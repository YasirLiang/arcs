/*
* @file
* @brief system setting dialog
* @ingroup system setting dialog
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-12-1
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
    uint8_t serialPort;
    QString passWord;
    static QString tcpIp;
    static QString tcpPort;
    static QTcpSocket tcpClient;
    static uint8_t *datagram;
    static uint8_t avail;
    static ARCS::PortEvt qtCltRdEvt;
    TExternPortVtbl vTable;
    SystemSetDialog(QWidget * parent = 0);
    void closeEvent(QCloseEvent *event);
    void setTcpSocket(char const * const pip,
        int _port);
    void setTcpSocket(void);
    static SystemSetDialog *instance(void);
    static void init_s(void);
    static int send_s(void const * const, int);
    static int recv_s(void * const, int);
    static int destroy_s(void);
private slots:
    void changePort(void);
    void changePass(void);
    void clientConnected(void);
    void clientDisConnected(void);
    void qtPortReady(void);
    void displayError(QAbstractSocket::SocketError socketErr);
    void tcpPortLineEditChange(const QString &txt);
    void tcpIpLineEditChange(const QString &txt);
    void passLineEditChange(const QString &txt);
} ;

#endif /* __SYSTEMSET_H__ */

