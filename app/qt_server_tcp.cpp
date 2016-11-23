/*
* @file qt_server_tcp.cpp
* @brief qt specific port
* @ingroup qt specific port
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
#include "qt_server_tcp.h"
/*$ Local variable decrelation..............................................*/
static QtTcpSocket l_qtTcpSocket;
/*$ QtTcpSocket_getVtbl()...................................................*/
TExternPortVtbl const * QtTcpSocket_getVtbl(void) {
    return l_qtTcpSocket.getVtbl();
}
/*$ QtTcpSocket::setTcpSocket()..............................................*/
void QtTcpSocket_setTcpSocket(char const * const pip,
    int _port)
{
    l_qtTcpSocket.setTcpSocket(pip, _port);
}
/*$ QtTcpSocket::QtTcpSocket()..............................................*/
QtTcpSocket::QtTcpSocket(char const * const pip,
    int _port)
  : ExternPortQt(&init_s,
        &send_s,
        &recv_s,
        &destroy_s)
{
    memset(ip, 0, sizeof(ip));
    memcpy(ip, pip, sizeof(ip));
    port = _port;
    avail = 0;
    datagram.resize(0);
    connect(&tcpClient, SIGNAL(connected(),
        this, SLOT(clientConnected())));
    connect(&tcpClient, SIGNAL(disconnected(),
        this, SLOT(clientDisConnected())));
    connect(&tcpClient, SIGNAL(readyRead()),
        this, SLOT(qtPortReady()));
    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(displayError(QAbstractSocket::SocketError)));
}
/*$ QtTcpSocket::displayError().............................................*/
void QtTcpSocket::displayError(QAbstractSocket::SocketError socketErr) {
    if (socketErr == QTcpSocket::RemoteHostClosedError) {
        return;
    }

    QMessageBox::information(this,
        tr("网络"),
        tr("产生如下错误:%1.")
        .arg(tcpClient.errorString()));
    tcpClient.close();
    avail = 0;
}
/*$ QtTcpSocket::clientConnected()..........................................*/
void QtTcpSocket::clientConnected() {
    avail = 1;
    /* can set connect server successful */
    qDebug("Connect to Server success( %s-%d )\n", ip, port);
}
/*$ QtTcpSocket::clientDisConnected().......................................*/
void QtTcpSocket::clientDisConnected() {
    avail = 0;
    /* can set disconnect server successful */
    qDebug("Client DisConnect From Server( %s-%d )\n", ip, port);
}
/*$ QtTcpSocket::qtPortReady()..............................................*/
void QtTcpSocket::qtPortReady() {
    if (avail) {
        /* publish port readable signal */
        QP::QF::PUBLISH(qtCltRdEvt, (void*)0);
    }
}
/*$ QtTcpSocket::setTcpSocket()..............................................*/
void QtTcpSocket::setTcpSocket(char const * const pip,
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
void QtTcpSocket::init_s(void) {
    tcpClient.connectToHost(ip, port);
}
/*$ specific Extern port send function......................................*/
int QtTcpSocket::send_s(void const * const buf, int sendLen) {
    if (avail) {
        datagram.resize(sendLen);
        memcpy(datagram.data(),
            (char const * const)buf, datagram.size());
        tcpClient.write(datagram.data(),
            datagram.size());
        /* release buffer */
        datagram.resize(0);
    }
}
/*$ specific Extern port recv function......................................*/
int QtTcpSocket::recv_s(void * const buf, int bufSize) {
    int reLen = -1;
    if (avail) {
        datagram.resize(tcpClient.bytesAvailable());
        reLen = (int)datagram.size();
        if (bufSize < reLen) {
            tcpClient.read(datagram.data(), datagram.size());
            memcpy((char * const)buf, datagram.data, reLen);
        }
        /* release buffer */
        datagram.resize(0);
    }
    /* Error return */
    return -1;
}
/*$ specific Extern port destroy function...................................*/
int QtTcpSocket::destroy_s(void) {
    tcpClient.close();
    avail = 0;
    datagram.resize(0);
}

