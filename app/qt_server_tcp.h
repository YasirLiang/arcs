/*
* @file qt_server_tcp.h
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
#ifndef __QT_SERVER_TCP_H__
#define __QT_SERVER_TCP_H__

class QtTcpSocket : public ExternPortQt {
private:
    uint8_t ip[32];
    int port;
    QTcpSocket tcpClient;
    QByteArray datagram;
    uint8_t avail;
    ARCS::PortEvt qtCltRdEvt(ARCS::PORTREADABLE_SIG,
        ARCS::QT_PORT, ARCS::READABLE);
public:
    QtTcpSocket();
protected:
    void init_s(void);
    int send_s(void const * const, int);
    int recv_s(void * const, int);
    int destroy_s(void);
};

#endif /* __QT_SERVER_TCP_H__ */
