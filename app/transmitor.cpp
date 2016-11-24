/*
* @file
* @brief transmitor active object
* @ingroup transmitor active object
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-11-24
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
#include "transmitor.h"
/*$ Qt Tcp socket get Vtbl..................................................*/
TExternPortVtbl const * QtTcpSocket_getVtbl(void);
namespace ARCS {

/*Local variable------------------------------------------------------------*/
QP::QMState const Transmitor::active_s = {
    static_cast<QP::QMState const *>(0), /* superstate (top) */
    Q_STATE_CAST(&active), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
/*Local variable------------------------------------------------------------*/
QP::QMState const Transmitor::serving_s = {
    &Transmitor::active_s, /* superstate (top) */
    Q_STATE_CAST(&serving), /* state handle */
    Q_ACTION_CAST(0), /* entry action */
    Q_ACTION_CAST(0), /* exit action */
    Q_ACTION_CAST(0) /* initial action */
};
/*Local variable------------------------------------------------------------*/
static Transmitor l_transmitor;
/*Global variable-----------------------------------------------------------*/
QP::QMActive *A0_Transmitor = &l_transmitor;
/*$ Transmitor::Transmitor()................................................*/
Transmitor::Transmitor()
    : QMActive(Q_STATE_CAST(&Transmitor::initial))
      m_timeEvt(this, TICK_1MS_SIG, 0U)
{
    if (!initialRecvBufList()) {
        Q_ASSERT(0); /* assert failed */
    }
    /* set information */
    for (uin8_t i = 0; i < EXTERN_PORT_NUM; ++i) {
        /* initial ring message proccess */
        userTimerStop(&ringMsgPro[i].smTimer);
        userTimerStop(&ringMsgPro[i].itvTimer);
        ringMsgPro.recvOver = (bool)1;
        ringMsgPro.msgLen = 0;
        /* initial ring buffer */
        RingBuffer_initial(&ringBuf[i],
            recvBuf[i].bufSize, recvBuf[i].pBuf);
        /* set StateMachine ring buffer,
            ring buffer must be initial */
        PortMsgStateMachine_setRingBuf(&ringBuf[i], i);
#ifdef INFLIGHT_PSM
        /* get port state machine msm */
        port[i] = PortInflightStateMachine_getIns(i);
#endif
    }
#ifdef INFLIGHT_PSM   
    /* set port state machine virtual table */
    PortInflightStateMachine_setPortVtbl(QtTcpSocket_getVtbl(),
            QT_SV_EP);
#endif
}
/*$ Transmitor::active()....................................................*/
bool Transmitor::initialRecvBufList(void) {
    /* new space and initial
        for transmitor port recv buffer list */
    uint8_t *ptr;
    bool bret = (bool)0;
    /* for qt port transmitor recv buffer */
    ptr = (uint8_t *)malloc(QT_ET_RECV_BUF_SIZE);
    if (ptr != (uint8_t *0)) {
        recvBuf[QT_SV_EP].pBuf = ptr;
        recvBuf[QT_SV_EP].bufSize = QT_ET_RECV_BUF_SIZE;
        bret = (bool)1;
    }
    /* initial other recv buffer */
    if (bret) {

    }
    /* return successfully flag */
    return bret;
}
/*$ Transmitor::active()....................................................*/
QP::QState Transmitor::initial(Transmitor * const me,
        QP::QEvt const * const e)
{
    static QP::QMTranActTable const table_ = {/* transition-action table */
        &serving_s,
        {
            Q_ACTION_CAST(&serving_e); /* entry */
            Q_ACTION_CAST(0)/* zero terminator */
        }
    };

    /* initial port state machine msm */
    for (uin8_t i = 0; i < EXTERN_PORT_NUM; ++i) {
        port[i]->init();
    }
    /* tran state table */
    return QM_TRAN_INIT(&table_);
}
/*$ Transmitor::active()....................................................*/
QP::QState Transmitor::active(Transmitor * const me,
    QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case TICK_1MS_SIG: {
            /* check for all ring buffer */
            for (int i = 0; i < EXTERN_PORT_NUM; ++i) {
                charMsgPro(i);
            }
            status_ = QM_HANDLE();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*$ Transmitor::serving()...................................................*/
QP::QState Transmitor::serving(Transmitor * const me,
        QP::QEvt const * const e)
{
    QP::QState status_;
    switch (e->sig) {
        case TRANSMIT_SIG: {
            switch (static_cast<TransmitEvt *>(e)->port) {
                case QT_PORT: {
                    port[QT_SV_EP]->dispatch(e);
                    break;
                }
                default: {/* no exit port */
                    break;
                }
            }
            status_ = QM_HANDLE();
            break;
        }
        case PORT_CHANGE_SIG: {
            TransmitEvt *pe = static_cast<TransmitEvt *>(e);
            switch (pe->port) {
                case QT_PORT: {
                    if (pe->buf[0] == 0) {/* Tcp */
                        uint16_t *port_ = (uint16_t*)&pe->buf[1];
                        char ip[32] = {0};
                        uint8_t ip_[4] = {0};
                        for (int i = 0; i < 4; ++i) {
                            ip_[i] = pe->buf[i+3];
                            itoa(ip_[i], (char*)&ip[i*4], 10);
                            if (i < 3) {/* three point */
                                ip[(i + 1) * 4 - 1] = '.';
                            }
                        }
                        qDebug("chang port-ip:%d, %s\n", *port_, ip);
                        QtTcpSocket_setTcpSocket(ip, *port_);
                    }
                    else if (pe->buf[0] == 1) {
                        qDebug("Not surport serial port now\n");
                    }
                    else {
                        /* Error port */
                    }
                    break;
                }
                default: {/* no exit port */
                    break;
                }
            }
            status_ = QM_HANDLE();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
/*$ Transmitor::serving_e().................................................*/
QP::QState Transmitor::serving_e(Transmitor * const me) {
    me->m_timeEvt.postIn(me, (QP::QTimeEvtCtr)1);
    return QM_ENTRY(&serving_s);
}
/*$ Transmitor::serving_x().................................................*/
QP::QState Transmitor::serving_x(Transmitor * const me) {
    me->m_timeEvt.disarm();
    return QM_EXIT(&serving_s);
}
/*$ Transmitor::qtCharMsgPro()..............................................*/
void Transmitor::qtCharMsgPro(void) {
    static TProtocalQt l_msgPro;
    char ch; /* char store */
    uint16_t dataLen;
    uint8_t crc = 0;
    if (ringMsgPro[QT_SV_EP].recvOver) {
        userTimerStart(3, &ringMsgPro[QT_SV_EP].itvTimer);
        if (userTimerTimeout(&ringMsgPro[QT_SV_EP].smTimer)) {
            A0_Controller->POST(Q_NEW(TransmitEvt,
                    ringMsgPro.msgLen, (uint8_t*)&l_msgPro),
                        &l_transmitor),
                &l_transmitor);
            userTimerStop(&ringMsgPro[QT_SV_EP].smTimer);
            ringMsgPro.msgLen = 0;
            ringMsgPro.recvOver = (bool)0;
        }
    }
    /* get ring char in buffer */
    while (RingBuffer_getChar(&ringBuf[QT_SV_EP], &ch)) {
        userTimerStart(3, &ringMsgPro[QT_SV_EP].itvTimer);
        ringMsgPro.recvOver = (bool)0;
        if ((ringMsgPro.msgLen == 0)
              && (ch == PROTOCAL_QT_TYPE))
        {
            l_msgPro.head = PROTOCAL_QT_TYPE;
            ringMsgPro.msgLen = 1;
        }
        else if (ringMsgPro.msgLen == 1) {
            l_msgPro.type = ch;
            ringMsgPro.msgLen = 2;
        }
        else if (ringMsgPro.msgLen == 2) {
            l_msgPro.seq = (((uint16_t)ch) & 0x00ff);
            ringMsgPro.msgLen = 3;
        }
        else if (ringMsgPro.msgLen == 3) {
            l_msgPro.seq = ((((uint16_t)ch) << 8) & 0xff00);
            ringMsgPro.msgLen = 4;
        }
        else if (ringMsgPro.msgLen == 4) {
            l_msgPro.cmd = ch;
            ringMsgPro.msgLen = 5;
        }
        else if (ringMsgPro.msgLen == 5) {
            l_msgPro.dataLen = ((uint16_t)ch) & 0x00ff;
            ringMsgPro.msgLen = 6;
        }
        else if (ringMsgPro.msgLen == 6) {
            l_msgPro.dataLen = (((uint16_t)ch) << 8) & 0xff00;
            if (l_msgPro.dataLen > QT_ET_RECV_BUF_SIZE) {
                ringMsgPro.msgLen = 0;
            }
            else {/* Not out of rang s*/
                ringMsgPro.msgLen = 7;
                dataLen = 0;
            }
        }
        else if (ringMsgPro.msgLen >= 7) {
            if (dataLen < l_msgPro.dataLen) {
                l_msgPro.dataBuf[dataLen] = ch;
                ringMsgPro.msgLen++;
                dataLen++;
            }
            else if (dataLen == l_msgPro.dataLen) {
                uint8_t const *p = (uint8_t *)l_msgPro;
                for (int i = 0; i < ringMsgPro.msgLen; ++i) {
                    crc ^= *(p++);
                }
                if (crc == ch) {
                    ringMsgPro.recvOver = (bool)1;
                    ringMsgPro.msgLen = 0;
                    userTimerStart(2, &ringMsgPro[QT_SV_EP].smTimer);
                }
            }
            else {
                ringMsgPro.msgLen = 0;
            }
        }
        else {
            /* never come this else */
        }
    }
}
/*$ Transmitor::charMsgPro()................................................*/
void Transmitor::charMsgPro(int port) {
    switch (port) {
        case QT_SV_EP: {
            qtCharMsgPro();
            break;
        }
        default: {/* Error port */
            break;
        }
    }
}

}/* namespace ARCS */

