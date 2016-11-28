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
#include <QtWidgets>
#include "user.h"
#include "systemset.h"
/*$ Local variable decrelation..............................................*/
static SystemSetDialog *l_sysSetDialog;
/*$ Class Local decralation-------------------------------------------------*/
ARCS::PortEvt SystemSetDialog::qtCltRdEvt(ARCS::PORTREADABLE_SIG,
    ARCS::QT_PORT,
    ARCS::READABLE);
QString SystemSetDialog::tcpIp;
int32_t SystemSetDialog::tcpPort;
QTcpSocket SystemSetDialog::tcpClient;
uint8_t *SystemSetDialog::datagram;
uint8_t SystemSetDialog::avail;
/*$ QtPortTcpSocket_getVtbl()...............................................*/
TExternPortVtbl const * QtPortTcpSocket_getVtbl(void) {
    return &l_sysSetDialog->vTable;
}
/*$ QtPortTcpSocket::setTcpSocket()..........................................*/
void QtPortTcpSocket_setTcpSocket(char const * const pip,
    int _port)
{
    l_sysSetDialog->setTcpSocket(pip, _port);
}
/*$ SystemSetDialog::SystemSetDialog()......................................*/
SystemSetDialog::SystemSetDialog(QWidget * parent)
    : QDialog(parent)
{
    l_sysSetDialog = this;
    setupUi(this);
    
    vTable.init = &init_s;
    vTable.send = &send_s;
    vTable.recv = &recv_s;
    vTable.destroy = &destroy_s;

    avail = 0;
    connect(&tcpClient, SIGNAL(connected()),
        this, SLOT(clientConnected()));
    connect(&tcpClient, SIGNAL(disconnected()),
        this, SLOT(clientDisConnected()));
    connect(&tcpClient, SIGNAL(readyRead()),
        this, SLOT(qtPortReady()));
    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(displayError(QAbstractSocket::SocketError)));
}
/*$ SystemSetDialog::displayError().........................................*/
void SystemSetDialog::displayError(QAbstractSocket::SocketError socketErr) {
    if (socketErr == QTcpSocket::RemoteHostClosedError) {
        return;
    }

    QMessageBox::information(this,
        "NetWork Error:",
        tcpClient.errorString());
    tcpClient.close();
    avail = 0;
}
/*$ SystemSetDialog::clientConnected()......................................*/
void SystemSetDialog::clientConnected(void) {
    avail = 1;
    /* can set connect server successful */
    qDebug("Connect to Server success( %s-%d )\n",
        (char *)&tcpIp, tcpPort);
}
/*$ SystemSetDialog::clientDisConnected()...................................*/
void SystemSetDialog::clientDisConnected(void) {
    avail = 0;
    /* can set disconnect server successful */
    qDebug("Client DisConnect From Server( %s-%d )\n",
        (char *)&tcpIp, tcpPort);
}
/*$ SystemSetDialog::qtPortReady()..........................................*/
void SystemSetDialog::qtPortReady(void) {
    if (avail) {
        /* publish port readable signal */
        QP::QF::PUBLISH(&qtCltRdEvt, (void*)0);
    }
}
/*$ SystemSetDialog::setTcpSocket().........................................*/
void SystemSetDialog::setTcpSocket(char const * const pip,
    int _port)
{
    tcpIp.clear();
    tcpIp.sprintf("%s", pip);
    tcpPort = _port;
    avail = 0;
    datagram = (uint8_t *)0;
    tcpClient.connectToHost(tcpIp, tcpPort);
}
/*$ specific Extern port init function......................................*/
void SystemSetDialog::init_s(void) {
    tcpClient.connectToHost(tcpIp,
        tcpPort);
}
/*$ specific Extern port send function......................................*/
int SystemSetDialog::send_s(void const * const buf, int sendLen) {
    int ret = -1;
    if (avail) {
        datagram = (uint8_t *)malloc(sendLen);
        if (datagram != (uint8_t *)0) {
            memcpy(datagram,
                (uint8_t *)buf, sendLen);
            tcpClient.write((char *)datagram, sendLen);
            free(datagram);
        }
        ret = sendLen;
    }
    return ret;
}
/*$ specific Extern port recv function......................................*/
int SystemSetDialog::recv_s(void * const buf, int bufSize) {
    int reLen = -1;
    if (avail) {
        reLen = tcpClient.bytesAvailable();
        if (bufSize >= reLen) {
            datagram = (uint8_t *)malloc(reLen);
            if (datagram != (uint8_t *)0) {
                tcpClient.read((char *)datagram, reLen);
                memcpy((uint8_t * const)buf,
                    datagram, reLen);
                free(datagram);
            }
        }
        else {
            /* set error return */
            reLen = -1;
        }
        /* release buffer */
        datagram = (uint8_t *)0;
    }
    /* Error return */
    return reLen;
}
/*$ specific Extern port destroy function...................................*/
int SystemSetDialog::destroy_s(void) {
    tcpClient.close();
    avail = 0;
    datagram = (uint8_t *)0;
    return 0;
}
/*$ SystemSetDialog::changePass()...........................................*/
void SystemSetDialog::changePass(void) {

}
/*$ SystemSetDialog::changePort()...........................................*/
void SystemSetDialog::changePort(void) {

}

