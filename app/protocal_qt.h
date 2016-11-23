/*
* @file protocal_qt.h
* @brief protocal communicate with qt application
* @ingroup protocal with qt and server
* @cond
******************************************************************************
* Build Date on  2016-11-11
* Last updated for version 1.0.0
* Last updated on  2016-11-11
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
/*Including-----------------------------------------------------------------*/
#ifndef __PROTOCAL_QT_H__
#define __PROTOCAL_QT_H__
/*! make a destinction between c and c++ file */
#ifdef __cplusplus /* only define in .cpp file */
    extern "C" { /*! for c++ file transfer c function */
#endif /* __cplusplus */
/*! the head TYPE of qt protocal with server */
#define PROTOCAL_QT_TYPE 0xAA
/*! protocal response mask */
#define PRO_RESP_MASK 0x80
/*! protocal query type */
#define PRO_QUERY_TYPE 0x00
/*! protocal set type */
#define PRO_SET_TYPE 0x20
/*! protocal report type from server */
#define PRO_REPORT_TYPE 0x40
/*! protocal handling mask */
#define PRO_HANDLING_MASK 0x02
/*! protocal error mask */
#define PRO_ERR_MASK 0x01
/*! protocal commom lenght */
#define PRO_COMMON_LEN 0x07
/*! protocal data offset */
#define PRO_DATA_POS 0x07
/*! data max lenght define */
#define PRO_QT_MAX 256 
/*!{@ protocal command bewteen qt and server define */
/*! qt queue terminal command */
#define QT_QUEUE_ID 0x01
/*! qt switch matrix command */
#define QT_SWITCH_MATRIX 0x02
/*! qt operation terminal command */
#define QT_OPT_TMNL 0x03
/*! qt system setting command */
#define QT_SYS_SET 0x04
/*! qt camera control command */
#define QT_CMR_CTL 0x05
/*!end of command define @} */
/*!struct of qt protocal with service define */
typedef struct TProtocalQt {
    uint8_t head; /*! protocal head */
    uint8_t type;/*! protocal type */
    uint16_t seq;/*! protocal seqId */
    uint8_t cmd;/*! protocal command */
    uint16_t dataLen;/*! protocal data lenght */
    uint8_t dataBuf[PRO_QT_MAX];/*! protocal databuf */
}__attribute__((packed)) TProtocalQt;
/*! struct of qt protocal with query id data */
typedef struct TProtocalQtQueryData {
    uint16_t id;
    uint8_t rgst:1; /*! register flags */
    uint8_t sign:1; /*! sign flags */
    uint8_t vote:1; /*! vote flag */
    uint8_t select:1; /*! select flag */
    uint8_t grade:1; /*! grade flag */
    uint8_t online:1; /*! online flag */
    uint8_t avbIdentity:1;
    uint8_t :1;          /*! reserved */
    uint8_t permision:4; /*! permision of terminal */
    uint8_t micStatus:4; /*! micphone status */
    uint16_t cnntNum; /*! connect num */
    uint64_t id_1722; /*! 1722 id */
    uint8_t name[64]; /*! name of avb device */
}__attribute__((packed)) TProtocalQtQueryData;
/*! common terminal type */
#define COMMON_TYPE 0
/*! vip type terminal */
#define VIP_TYPE 1
/*! commom chairman type */
#define COMMON_CHM_TYPE 2
/*! execute chairman type */
#define EXECUTE_CHM_TYPE 3
/*! close status */
#define MIC_CLOSE 0
/*! open status */
#define MIC_OPEN 1
/*! first apply status */
#define MIC_FIRST_APPLY 2
/*! apply status */
#define MIC_APPLY 3
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif