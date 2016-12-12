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
QString SystemSetDialog::tcpIp("192.168.0.243");
QString SystemSetDialog::tcpPort = "5002";
QTcpSocket SystemSetDialog::tcpClient;
uint8_t *SystemSetDialog::datagram;
uint8_t SystemSetDialog::avail;
/*$ QtPortTcpSocket_getVtbl()...............................................*/
TExternPortVtbl const * QtPortTcpSocket_getVtbl(void) {
    TExternPortVtbl const *pE = (TExternPortVtbl const *)0;
    if (l_sysSetDialog != (SystemSetDialog *)0) {
        pE = &l_sysSetDialog->vTable;
    }
    /* return vTable pointer */
    return pE;
}
/*$ QtPortTcpSocket::setTcpSocket()..........................................*/
void QtPortTcpSocket_setTcpSocket(char const * const pip,
    int _port)
{
    if (l_sysSetDialog != (SystemSetDialog *)0) {
        l_sysSetDialog->setTcpSocket(pip, _port);
    }
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
    /* set ip */
    tcpPortLineEdit->setText(tcpPort);
    ipLineEdit->setText(tcpIp);
    serialPortComboBox->setEnabled(0);
    connect(changePortPushBtn, SIGNAL(clicked()),
        this, SLOT(changePort()));
    connect(changePassPushBtn, SIGNAL(clicked()),
        this, SLOT(changePass()));
    connect(&tcpClient, SIGNAL(connected()),
        this, SLOT(clientConnected()));
    connect(&tcpClient, SIGNAL(disconnected()),
        this, SLOT(clientDisConnected()));
    connect(&tcpClient, SIGNAL(readyRead()),
        this, SLOT(qtPortReady()));
    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(tcpPortLineEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(tcpPortLineEditChange(const QString &)));
    connect(ipLineEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(tcpIpLineEditChange(const QString &)));
        connect(passLineEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(passLineEditChange(const QString &)));
}
/*closeEvent()..............................................................*/
void SystemSetDialog::closeEvent(QCloseEvent *event) {
    qDebug("SystemSetDialog recieve Close Event");
    this->hide();
    event->ignore();
}
/*$ SystemSetDialog::displayError().........................................*/
void SystemSetDialog::displayError(QAbstractSocket::SocketError socketErr) {
    if (socketErr == QTcpSocket::RemoteHostClosedError) {
        return;
    }
    qDebug("Socket Error\n");
    QMessageBox::information(this,
        "NetWork Error:",
        tcpClient.errorString());
    tcpClient.close();
    avail = 0;
}
/*$ SystemSetDialog::clientConnected()......................................*/
void SystemSetDialog::clientConnected(void) {
    char *pIp;
    QByteArray bAy;
    QString info;
    bool ok;
    int32_t uport;
    
    avail = 1;
    bAy = tcpIp.toLatin1(); 
    pIp = bAy.data();
    if (!tcpPort.isEmpty()) {
        uport = (int32_t)tcpPort.toInt(&ok, 10);
        info.clear();
        info.sprintf("connect to (%s-%d) successfully",
                    pIp, uport);
        /* can set connect server successful */
        qDebug("Connect to Server success(%s-%d)\n",
            pIp, uport);
        QMessageBox::information(this, "Socket Success",
                info, QMessageBox::Ok,
                0, 0);
    }
}
/*$ SystemSetDialog::clientDisConnected()...................................*/
void SystemSetDialog::clientDisConnected(void) {
    char *pIp;
    QByteArray bAy;
    QString info;
    bool ok;
    int32_t uport;
    
    avail = 0;
    bAy = tcpIp.toLatin1(); 
    pIp = bAy.data();
    if (!tcpPort.isEmpty()) {
        uport = (int32_t)tcpPort.toInt(&ok, 10);
        info.clear();
        info.sprintf("DisConnect From Server(%s-%d) Success",
                    pIp, uport);
        /* can set disconnect server successful */
        qDebug("DisConnect From Server(%s-%d) Success\n",
                pIp, uport);
        QMessageBox::information(this, "Socket Success",
                info, QMessageBox::Ok,
                0, 0);
    }
}
/*$ SystemSetDialog::qtPortReady()..........................................*/
void SystemSetDialog::qtPortReady(void) {
    if (avail) {
        /* publish port readable signal */
        ARCS::A0_Transmitor->POST(&qtCltRdEvt, (void*)0);
    }
}
/*$ SystemSetDialog::setTcpSocket().........................................*/
void SystemSetDialog::setTcpSocket(char const * const pip,
    int _port)
{
    char *pIp;
    bool ok;
    int32_t uport;
    QByteArray bAy;
    QString s;
    
    tcpIp.clear();
    tcpIp.sprintf("%s", pip);
    s = QString::number(_port, 10);
    tcpPort = s;
    bAy = tcpIp.toLatin1(); 
    pIp = bAy.data();
    if (!tcpPort.isEmpty()) {
        uport = (int32_t)tcpPort.toInt(&ok, 10);
        qDebug("tcpPort(%d) changed \n", uport);
    }
    /* will connect to server */
    qDebug("will connect to server[%s-%d]",
        pIp, uport);
    tcpClient.connectToHost(tcpIp, uport);
    avail = 0;
    datagram = (uint8_t *)0;
}
/*$ SystemSetDialog::setTcpSocket().........................................*/
void SystemSetDialog::setTcpSocket(void)
{
    char *pIp;
    bool ok;
    int32_t uport;
    QByteArray bAy = tcpIp.toLatin1(); 
    pIp = bAy.data();
    if (!tcpPort.isEmpty()) {
        uport = (int32_t)tcpPort.toInt(&ok, 10);
        qDebug("tcpPort(%d) changed \n", uport);
    }
    /* will connect to server */
    qDebug("will connect to server[%s-%d](none parameter )",
        pIp, uport);
    tcpClient.connectToHost(tcpIp, uport);
    avail = 0;
    datagram = (uint8_t *)0;
}
/*$ specific Extern port init function......................................*/
void SystemSetDialog::init_s(void) {
    bool ok;
    int32_t iport;
    if (!tcpPort.isEmpty()) {
        iport = (int32_t)tcpPort.toInt(&ok, 10);
        tcpClient.connectToHost(tcpIp, iport);
    }
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
            tcpClient.flush();
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
/*$ SystemSetDialog::changePort()...........................................*/
void SystemSetDialog::changePort(void) {
    /* get change value */
    QString s = portSelectComboBox->currentText();
    if (s == "Tcp") {
        /* set tcp port */
        tcpClient.close();
        setTcpSocket();
    }
    else if (s == "Serial"){
        /* set Serial port */
    }
    else {
        qDebug("no other port ");
    }
}
/*$ SystemSetDialog::changePass()...........................................*/
void SystemSetDialog::changePass(void) {

}
/*$ */
void SystemSetDialog::tcpPortLineEditChange(const QString &txt) {
    bool ok;
    int32_t iport;
    if (!txt.isEmpty()) {
        tcpPort = txt;
        iport = (int32_t)txt.toInt(&ok, 10);
        qDebug("tcpPort(%d) changed \n", iport);
    }
}
/*$ */
void SystemSetDialog::tcpIpLineEditChange(const QString &txt) {
    char *pIp;
    QByteArray bAy;
    
    tcpIp.clear();
    tcpIp = txt;    
    bAy = txt.toLatin1();
    pIp = bAy.data();
    /* ip debug */
    qDebug("ip(%s) change", pIp);
}
/*$ */
void SystemSetDialog::passLineEditChange(const QString &txt) {
    char *pass;
    QByteArray bAy;
    
    passWord.clear();
    passWord = txt;
    bAy = passWord.toLatin1(); 
    pass = bAy.data();
    /* ip debug */
    qDebug("password(%s) change", pass);
}

