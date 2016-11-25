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
/*Including file------------------------------------------------------------*/
#include <QMessageBox> 
#include "user.h"
#include "extern_port.h"
#include "extern_port_qt.h"
#include "systemset.h"
/*$ Local variable decrelation..............................................*/
static SystemSetDialog l_sysSetDialog;
/*$ QtPortTcpSocket_getVtbl()...............................................*/
TExternPortVtbl const * QtPortTcpSocket_getVtbl(void) {
    return &l_qtTcpSocket.vTable;
}
/*$ QtPortTcpSocket::setTcpSocket()..........................................*/
void QtPortTcpSocket_setTcpSocket(char const * const pip,
    int _port)
{
    l_qtTcpSocket.setTcpSocket(pip, _port);
}
/*$ QtPortTcpSocket::QtPortTcpSocket()......................................*/
QtPortTcpSocket::QtPortTcpSocket(char const * const pip,
    int _port)
{
    vTable.init = &init_s;
    vTable.send = &send_s;
    vTable.recv = &recv_s;
    vTable.destroy = &destroy_s;

    qtCltRdEvt.sig = ARCS::PORTREADABLE_SIG;
    qtCltRdEvt.eType = ARCS::READABLE;
    qtCltRdEvt.port = ARCS::QT_PORT;
    memset(ip, 0, sizeof(ip));
    memcpy(ip, pip, sizeof(ip));
    port = _port;
    avail = 0;
    datagram.resize(0);
    connect(&tcpClient, SIGNAL(connected()),
        this, SLOT(clientConnected()));
    connect(&tcpClient, SIGNAL(disconnected()),
        this, SLOT(clientDisConnected()));
    connect(&tcpClient, SIGNAL(readyRead()),
        this, SLOT(qtPortReady()));
    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(displayError(QAbstractSocket::SocketError)));
}
/*$ QtPortTcpSocket::displayError().........................................*/
void QtPortTcpSocket::displayError(QAbstractSocket::SocketError socketErr) {
    if (socketErr == QTcpSocket::RemoteHostClosedError) {
        return;
    }

    QMessageBox::information(this,
        "网络产生如下错误:",
        tcpClient.errorString());
    tcpClient.close();
    avail = 0;
}
/*$ QtPortTcpSocket::clientConnected().......................................*/
void QtPortTcpSocket::clientConnected(void) {
    avail = 1;
    /* can set connect server successful */
    qDebug("Connect to Server success( %s-%d )\n", ip, port);
}
/*$ QtPortTcpSocket::clientDisConnected()...................................*/
void QtPortTcpSocket::clientDisConnected(void) {
    avail = 0;
    /* can set disconnect server successful */
    qDebug("Client DisConnect From Server( %s-%d )\n", ip, port);
}
/*$ QtPortTcpSocket::qtPortReady()..........................................*/
void QtPortTcpSocket::qtPortReady(void) {
    if (avail) {
        /* publish port readable signal */
        QP::QF::PUBLISH(&this.qtCltRdEvt, (void*)0);
    }
}
/*$ QtPortTcpSocket::setTcpSocket().........................................*/
void QtPortTcpSocket::setTcpSocket(char const * const pip,
    int _port)
{
    memset(ip, 0, sizeof(ip));
    memcpy(ip, pip, sizeof(ip));
    port = _port;
    avail = 0;
    datagram.resize(0);
    tcpClient.connectToHost(ip, port);
}
/*$ specific Extern port init function......................................*/
void QtPortTcpSocket::init_s(void) {
    l_qtTcpSocket.tcpClient.connectToHost(l_qtTcpSocket.ip,
        l_qtTcpSocket.port);
}
/*$ specific Extern port send function......................................*/
int QtPortTcpSocket::send_s(void const * const buf, int sendLen) {
    int ret = -1;
    if (l_qtTcpSocket.avail) {
        l_qtTcpSocket.datagram.resize(sendLen);
        memcpy(l_qtTcpSocket.datagram.data(),
            (char const * const)buf, l_qtTcpSocket.datagram.size());
        l_qtTcpSocket.tcpClient.write(l_qtTcpSocket.datagram.data(),
            l_qtTcpSocket.datagram.size());
        /* release buffer */
        l_qtTcpSocket.datagram.resize(0);
        ret = sendLen;
    }
    return ret;
}
/*$ specific Extern port recv function......................................*/
int QtPortTcpSocket::recv_s(void * const buf, int bufSize) {
    int reLen = -1;
    if (l_qtTcpSocket.avail) {
        l_qtTcpSocket.datagram.resize(
            l_qtTcpSocket.tcpClient.bytesAvailable());
        reLen = (int)l_qtTcpSocket.datagram.size();
        if (bufSize < reLen) {
            l_qtTcpSocket.tcpClient.read(l_qtTcpSocket.datagram.data(),
                l_qtTcpSocket.datagram.size());
            memcpy(buf, l_qtTcpSocket.datagram.data, reLen);
        }
        else {
            /* set error return */
            reLen = -1;
        }
        /* release buffer */
        datagram.resize(0);
    }
    /* Error return */
    return reLen;
}
/*$ specific Extern port destroy function...................................*/
int QtPortTcpSocket::destroy_s(void) {
    l_qtTcpSocket.tcpClient.close();
    l_qtTcpSocket.avail = 0;
    l_qtTcpSocket.datagram.resize(0);
}

