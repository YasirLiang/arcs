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
#ifndef __TRANSMITOR_H__
#define __TRANSMITOR_H__

namespace ARCS {

/*! Transmit Recieve Buffer type */
typedef struct TRecieveBuf {
    uint8_t *pBuf;
    int bufSize;
}TRecieveBuf;
/*! Transmit Recieve Buffer type */
typedef struct TRingMsgPro {
    /*! state switch and interval timer */
    TUserTimer smTimer, itvTimer;
    /* receive message over flag */
    bool recvOver;
    /*! recieve message lenght */
    uint32_t msgLen;
}TRingMsgPro;
/*! class Transmitor--------------------------------------------------------*/
class Transmitor : public QP::QMActive {
private:
    /*! qt extern port recv buffer size */
    #define QT_ET_RECV_BUF_SIZE 4096
    /*! port state machine */
    static QP::QMsm *port[EXTERN_PORT_NUM];
    /* recv ring buffer for all port */
    static TCharRingBuf ringBuf[EXTERN_PORT_NUM];
    /* recieve buffer for port */
    static TRecieveBuf recvBuf[EXTERN_PORT_NUM];
    static TRingMsgPro ringMsgPro[EXTERN_PORT_NUM];
    /* 1ms time out event */
    QP::QTimeEvt m_timeEvt;
#ifdef MSG_QUEUE_PSM
    /* define send queue here for port message SM, But
        in inflight, there is not need. */
#endif
    static void qtCharMsgPro(void);
    static void charMsgPro(int port);
    bool initialRecvBufList(void);
public:
    Transmitor();
    ~Transmitor();
protected:
    static QP::QMState const active_s;
    static QP::QMState const serving_s;
    static QP::QState active(Transmitor * const me,
        QP::QEvt const * const e);
    static QP::QState initial(Transmitor * const me,
        QP::QEvt const * const e);
    static QP::QState serving(Transmitor * const me,
        QP::QEvt const * const e);
    static QP::QState serving_e(Transmitor * const me);
    static QP::QState serving_x(Transmitor * const me);
};

} /* namespace ARCS */

#endif /* __TRANSMITOR_H__ */

