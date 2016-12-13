/*
* @file
* @brief main surface
* @ingroup main surface
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-12-2
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
/*Incluing file-------------------------------------------------------------*/
#include <QtWidgets>
#include <fstream>
#include <QTextCodec> 
#include "user.h"
#include "systemset.h"
#include "mainwidget.h"
#include "protocal_qt.h"
/*$ Class Local decralation-------------------------------------------------*/
ARCS::RequestEvt MainSurface::e(ARCS::REQUEST_SIG,
                    0U,
                    (TRequestType)0,
                    (uint8_t *)0,
                    0U,
                    QT_REQUEST);
/* Local mainsurface instance-----------------------------------------------*/
static MainSurface *l_instance;

/*$ Global varailable-------------------------------------------------------*/
struct {
    uint8_t err;
    const char *errorStr;
}ArcsProErrStr[MAX_CMD_NUM][MAX_ERR_CODE] = {
    {
        {QR_SUCCESS, "QR_SUCCESS"},
        {NO_ID, "NO_ID"},
        {HOST_HANDING, "HOST_HANDING"},
        {MAX_QR_ER_PUB, "UNKOWN"}
    },
    {
        {SM_SUCCESS, "SM_SUCCESS"},
        {MAX_SM_ER_PUB, "UNKOWN"}
    },
    {
        {OPT_SUCCESS, "OPT_SUCCESS"},
        {NO_SIGN, "NO_SIGN"},
        {NO_SUCH_ID, "NO_SUCH_ID"},
        {PRESET_STATE, "PRESET_STATE"},
        {MAX_OPT_ER_PUB, "UNKOWN"}
    },
    {
        {SSET_SUCCESS, "SSET_SUCCESS"},
        {UPDATE_WRITE_ERR, "UPDATE_WRITE_ERR"},
        {DATA_FORMAT_ERR, "DATA_FORMAT_ERR"},
        {START_SYS_ERR, "START_SYS_ERR"},
        {STOP_SYS_ERR, "STOP_SYS_ERR"},
        {NO_SPACE, "NO_SPACE"},
        {TRANSMIT_ERR, "TRANSMIT_ERR"},
        {MAX_SYSSET_ER_PUB, "UNKOWN"}
    },
    {
        {CMR_CTL_SUCCESS, "CMR_CTL_SUCCESS"},
        {MAX_CMRCTL_ER_PUB, "UNKOWN"}
    }
};
/*MainSurface().............................................................*/
MainSurface::MainSurface(QWidget *parent)
    : QWidget(parent)
{
    int index;
    bool ok;
    QString s;
    
    l_instance = this;
    setupUi(this);
    /* set current camera control type */
    index = cmrCtlTypeComboBox->currentIndex();
    if (index == 0) {/* focus */
        cmrCType = FOCUS;
    }
    else if (index == 1) { /* apert */
        cmrCType = APERT;
    }
    else if (index == 2) { /* zoom */
        cmrCType = ZOOM;
    }
    else {
        /* no else case */
    }
    /* set current terminal id */
    idComboBoxValChanged();
    /* set current camera number */
    s = curcameraComboBox->currentText();
    curCamera = s.toInt(&ok, 10);

    /* set operation type */
    index = optTypeComboBox->currentIndex();
    if (index == 0) {/* operetion to speak */
        optType = TERMINAL_SPEAK;
        optComboBox->setEnabled(1); /* enabled */
        pausePushBtn->setEnabled(0); /* disabled */
        resumePushBtn->setEnabled(0); /* disabled */
    }
    else if (index == 1) { /* SIGN */
        optType = TERMINAL_SIGN;
        /* disable */
        optComboBox->setEnabled(0); /* disabled */
    }
    else if (index == 2) { /* vote */
        optType = TERMINAL_VOTE;
        optComboBox->setEnabled(0); /* disabled */
    }
    else {
        /* no else case */
    }
    /* set line Edit QIntValidator */
    pLvSpIntValidator = new QIntValidator(0, 255, this);
    pVtSpIntValidator = new QIntValidator(0, 255, this);
    leveSpeedLineEdit->setValidator(pLvSpIntValidator);
    verticalSpeedLineEdit->setValidator(pLvSpIntValidator);
    /* set editable */
    idComboBox->setEditable(1);
    optComboBox->setEditable(1);

    requstId = 0; /* request id form zero */
    uiLocked = 0;
    curAddr = 0xffff; /* all terminal */
    curOutput = 1;
    curInput = 1;
    /* create system set dialog */
    sysDlg = new SystemSetDialog(this);
    /* create system update proccess bar */
    pSysUpatePro = new QProgressDialog(this);
    pSysUpatePro->setWindowTitle(tr("UpLoad Process"));
    pSysUpatePro->setLabelText(tr("Transfering...")); 
    pSysUpatePro->setCancelButtonText(tr("cancel"));
    pSysUpatePro->reset(); /* hide dialog */
    cancelUpdate = (bool)0;
    /* set result to NULL */
    setResult("Result:");
    /* set status to Ready */
    setStatus("Status: Ready");
    pBytesToWriteBuf = (uint8_t *)0;
    totalBytes = 0;
    bytesToWrite = 0;
    /* no change set */
    qresultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    /* select whole row */
    qresultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    /* sort column table when clicked head table */
    QHeaderView *headerGoods = qresultTableWidget->horizontalHeader();
    headerGoods->setSortIndicator(0, Qt::AscendingOrder);
    headerGoods->setSortIndicatorShown(true);
    connect(headerGoods, SIGNAL(sectionClicked(int)),
        qresultTableWidget, SLOT(sortByColumn(int)));
    
    connect(zoomPushBtn, SIGNAL(clicked()), this, SLOT(add()));
    connect(beginPushBtn, SIGNAL(clicked()), this, SLOT(begin()));
    connect(curcameraSwPushBtn, SIGNAL(clicked()),
        this, SLOT(cameraSwitch()));
    connect(clearUnCPushBtn, SIGNAL(clicked()),
        this, SLOT(clearUnConnected()));
    connect(clearUnRegPushBtn, SIGNAL(clicked()),
        this, SLOT(clearUnRegister()));
    connect(downPushBtn, SIGNAL(clicked()), this, SLOT(down()));
    connect(downLeftPushBtn, SIGNAL(clicked()), this, SLOT(downLeft()));
    connect(downRightPushBtn, SIGNAL(clicked()), this, SLOT(downRight()));
    connect(leftPushBtn, SIGNAL(clicked()), this, SLOT(left()));
    connect(rightPushBtn, SIGNAL(clicked()), this, SLOT(right()));
    connect(matrixSwPushBtn, SIGNAL(clicked()), this, SLOT(matrixSwitch()));
    connect(pausePushBtn, SIGNAL(clicked()), this, SLOT(pause()));
    connect(queryPushBtn, SIGNAL(clicked()), this, SLOT(query()));
    connect(narrowPushBtn, SIGNAL(clicked()), this, SLOT(reduce()));
    connect(resumePushBtn, SIGNAL(clicked()), this, SLOT(regain()));
    connect(setSysPushBtn, SIGNAL(clicked()), this, SLOT(setSys()));
    connect(connectedCheckBox, SIGNAL(clicked()),
        this, SLOT(showConnected()));
    connect(stopPushBtn, SIGNAL(clicked()), this, SLOT(stop()));
    connect(upPushBtn, SIGNAL(clicked()), this, SLOT(up()));
    connect(upLeftPushBtn, SIGNAL(clicked()), this, SLOT(upLeft()));
    connect(upRightPushBtn, SIGNAL(clicked()), this, SLOT(upRight()));
    connect(leveSpeedLineEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(levelSpeedDoTxtChange(const QString &)));
    connect(verticalSpeedLineEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(verticalSpeedDoTxtChange(const QString &)));
    connect(cmrCtlTypeComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(cmrCtlTypeComboBoxValChanged()));
    connect(curcameraComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(curcameraComboBoxValChanged()));
     connect(optTypeComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(optTypeComboBoxValChanged(int)));
     connect(optComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(optComboBoxValChanged()));
     connect(optComboBox, SIGNAL(currentTextChanged(const QString &)),
        this, SLOT(optComboBoxCurValChanged(const QString &)));
     connect(inputComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(inputComboBoxValChanged()));
     connect(outputComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(outputComboBoxValChanged()));
     connect(idComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(idComboBoxValChanged()));
     connect(idComboBox, SIGNAL(currentTextChanged(const QString &)),
        this, SLOT(idComboBoxCurValChanged(const QString &)));
     connect(beginSysPushBtn, SIGNAL(clicked()),
        this, SLOT(beginSystem()));
     connect(stopSysPushBtn, SIGNAL(clicked()),
        this, SLOT(stopSystem()));
     connect(updateSysPushBtn, SIGNAL(clicked()),
        this, SLOT(updateSystem()));
     /* tap cancel button to process cancel event */
     connect(pSysUpatePro, SIGNAL(canceled()),
        this, SLOT(cancelProcessBars()));
}
/*~MainSurface()............................................................*/
MainSurface::~MainSurface(void) {
    qDebug("Exit MainSurface");
    if (pBytesToWriteBuf != (uint8_t *)0) {
        delete pBytesToWriteBuf;
        pBytesToWriteBuf = (uint8_t *)0;
    }
    delete pLvSpIntValidator;
    delete pVtSpIntValidator;
    delete sysDlg;
    delete pSysUpatePro;
}
/*closeEvent()..............................................................*/
void MainSurface::closeEvent(QCloseEvent *event) {
    qDebug("MainSurface recieve Close Event");
    static QP::QEvt const e(ARCS::TERMINATE_SIG);
    QP::QF::PUBLISH(&e, (void *)0);
    (void)event;
}
/*instance()................................................................*/
MainSurface* MainSurface::instance(void) {
    return l_instance;
}
/*lockUi()..................................................................*/
void MainSurface::lockUi(void) {
    uiLocked = (uint8_t)1;
}
/*unLockUi()................................................................*/
void MainSurface::unLockUi(void) {
    uiLocked = (uint8_t)0;
}
/*setStatus()...............................................................*/
void MainSurface::setStatus(char const * const status_) {
    if (status_ != (char *)0) {
        statusLabel->setText(tr(status_));
    }
}
/*setResult()...............................................................*/
void MainSurface::setResult(char const * const result_) {
    if (result_ != (char *)0) {
        resultLabel->setText(tr(result_));
    }
}
/*setE64_u64()..............................................................*/
void MainSurface::setE64_u64(uint8_t *const buf, uint64_t const ie64) {
    for (int i = 0; i < 8; i++) {
        buf[i] = (uint8_t)(ie64 >> (i * 8));
    }
}
/*loadUpdateData()..........................................................*/
uint64_t MainSurface::loadUpdateData(uint8_t * const buf,
    uint64_t reqSize)
{
    uint64_t lLen, pos; /* load len */
    if ((buf == (uint8_t *const)0)
         || (totalBytes == 0)
         || (bytesToWrite == 0)
         || (pBytesToWriteBuf == (uint8_t *)0))
    {
        return 0;
    }
    /* get buffer pos to read */
    pos = totalBytes - bytesToWrite;
    if (bytesToWrite > reqSize) {
        memcpy(buf,
            &pBytesToWriteBuf[pos],
            reqSize);
        lLen = reqSize;
        bytesToWrite -= reqSize;
    }
    else if (bytesToWrite != 0) {
        memcpy(buf,
            &pBytesToWriteBuf[pos],
            bytesToWrite);
        lLen = bytesToWrite;
        bytesToWrite = 0;
    }
    else {
        lLen = 0; /* transmit done */
    }
    /* return len, and zero means transmit done */
    return lLen;
}
/*updateBarProccess().......................................................*/
void MainSurface::updateBarProccess(void) {
/* invoked only when request called success */ 
    pSysUpatePro->setValue(totalBytes - bytesToWrite);
}
/*updateBarProccess().......................................................*/
bool MainSurface::isUpdateCancel(void) {
/* get cancel flag to true */ 
    return cancelUpdate;
}
/*error display().......................................................*/
void MainSurface::displayArcsErr(uint8_t cmd, uint8_t err, bool timeOut) {
/* cmd must little max arcs protocal command */
    QString str;
    if (!timeOut) {
        if ((cmd > MAX_CMD_NUM)
             || (err >= MAX_ERR_CODE))
        {/* error arcs error information */
            return;
        }
        
        qDebug("cmd = 0x%x, err = 0x%x", cmd, err);
        if (err != 0) { /* no success */
            str.sprintf("Cmd = %d, status = %s",
                cmd, ArcsProErrStr[cmd-1][err].errorStr);
            QMessageBox::critical(this, "Arcs Cmd Operation Failed",
                str, QMessageBox::Ok,
                0, 0);
        }
        else {
            str.sprintf("Cmd = %d, status = %s",
                cmd, ArcsProErrStr[cmd-1][err].errorStr);
            QMessageBox::information(this, "Arcs Cmd Operation Success",
                str, QMessageBox::Ok,
                0, 0);
        }
    }
    else {
        str.sprintf("Cmd = %d, status = %s",
                cmd, "TIMEOUT");
        QMessageBox::critical(this, "Arcs Cmd Operation Failed",
                str, QMessageBox::Ok,
                0, 0);
    }
}
/*add().....................................................................*/
void MainSurface::add(void) {
    if (!uiLocked) {
        /* current requestment id  will be setted in specific
            command function, so e.id not set here */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        switch (cmrCType) { /* camera control type */
            case FOCUS: {
                /* low 4 bits means operation code. 0x0a near focus */
                e.buf[3] |= 0x1a; /* control focus near */
                break;
            }
            case APERT: {
                e.buf[3] |= 0x1d;/* control apert */
                break;
            }
            case ZOOM: {
                e.buf[3] |= 0x19;/* control zoom big */
                break;
            }
            default: {
                /* error camera control type, return */
                return;
            }
        }
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*begin()...................................................................*/
void MainSurface::begin(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = OPT_TERMINAL;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= 0x00; /* opt code is begining operation */
        switch (optType) {
            case TERMINAL_SPEAK: {
                e.buf[0] |= 0x00; /* speak type */
                e.buf[1] = (uint8_t)(curAddr & 0x00ff); /* low addr is ahead */
                e.buf[2] = (uint8_t)((curAddr & 0xff00) >> 8u);
                e.buflen = (uint8_t)3;
                break;
            }
            case TERMINAL_SIGN: {
                e.buf[0] |= 0x10; /* sign type */
                e.buflen = (uint8_t)1;
                break;
            }
            case TERMINAL_VOTE: {
                e.buf[0] |= 0x20; /* vote type */
                e.buflen = (uint8_t)1;
                break;
            }
            default: {
                /* no other case, return */
                return;
            }
        }
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*cameraSwitch()............................................................*/
void MainSurface::cameraSwitch(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x00; /* switch camera control type */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*clearUnConnected()........................................................*/
void MainSurface::clearUnConnected(void) {

}
/*clearUnRegister().........................................................*/
void MainSurface::clearUnRegister(void) {

}
/*down()....................................................................*/
void MainSurface::down(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x11; /* control down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();        
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*downLeft()................................................................*/
void MainSurface::downLeft(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x16; /* control left down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*downRight()...............................................................*/
void MainSurface::downRight(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x17; /* control right down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*left()....................................................................*/
void MainSurface::left(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x12; /* control left down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*right()...................................................................*/
void MainSurface::right(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x13; /* control right down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*matrixSwitch()............................................................*/
void MainSurface::matrixSwitch(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = SWITCH_MATRIX;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= ((curInput & 0x0f) << 0);
        e.buf[0] |= ((curOutput & 0x0f) << 4);
        e.buflen = (uint8_t)1;
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*pause()...................................................................*/
void MainSurface::pause(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = OPT_TERMINAL;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= 0x01; /* opt code is stop operation */
        switch (optType) {
            case TERMINAL_SPEAK: {
                e.buf[0] |= 0x00; /* speak type */
                e.buf[1] = (uint8_t)(curAddr & 0x00ff); /* low addr is ahead */
                e.buf[2] = (uint8_t)((curAddr & 0xff00) >> 8u);
                e.buflen = (uint8_t)3;
                break;
            }
            case TERMINAL_SIGN: {
                e.buf[0] |= 0x10; /* sign type */
                e.buflen = (uint8_t)1;
                break;
            }
            case TERMINAL_VOTE: {
                e.buf[0] |= 0x20; /* vote type */
                e.buflen = (uint8_t)1;
                break;
            }
            default: {
                /* no other case, return */
                return;
            }
        }
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*query()...................................................................*/
void MainSurface::query(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = QUERY_ID;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[1] &= ~0xff; /* clear buf */
        e.buf[0] |= (uint8_t)((curId & 0x00ff) >> 0U); /* low address */
        e.buf[1] |= (uint8_t)((curId & 0xff00) >> 8U); /* high address */
        e.buflen = (uint8_t)2; /* set buffer lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* clear table widget */
        int row = qresultTableWidget->rowCount();
        for (int i = 0; i < row; i++) {
            qresultTableWidget->removeRow(0);
        }
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*reduce()..................................................................*/
void MainSurface::reduce(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        switch (cmrCType) { /* camera control type */
            case FOCUS: {
                /* low 4 bits means operation code. 0x0c far focus */
                e.buf[3] |= 0x1b; /* control reduce focus */
                break;
            }
            case APERT: {
                e.buf[3] |= 0x1c; /* control reduce apert */
                break;
            }
            case ZOOM: {
                e.buf[3] |= 0x18; /* control reduce zoom */
                break;
            }
            default: {
                /* error camera control type, return */
                return;
            }
        }
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");        
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*regain()..................................................................*/
void MainSurface::regain(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = OPT_TERMINAL;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= 0x02; /* opt code is regain operation */
        switch (optType) {
            case TERMINAL_SIGN: {
                e.buf[0] |= 0x10; /* sign type */
                break;
            }
            case TERMINAL_VOTE: {
                e.buf[0] |= 0x20; /* vote type */
                break;
            }
            default: {
                /* no other case, return */
                return;
            }
        }
        e.buflen = (uint8_t)1;
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*setSys()..................................................................*/
void MainSurface::setSys(void) {
    if (!uiLocked) {
        sysDlg->show();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*showConnected()...........................................................*/
void MainSurface::showConnected(void) {

}
/*stop()....................................................................*/
void MainSurface::stop(void) {
    
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = OPT_TERMINAL;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= 0x03; /* opt code is stop operation */
        switch (optType) {
            case TERMINAL_SPEAK: {
                e.buf[0] |= 0x00; /* speak type */
                e.buf[1] = (uint8_t)(curAddr & 0x00ff); /* low addr is ahead */
                e.buf[2] = (uint8_t)((curAddr & 0xff00) >> 8U);
                e.buflen = (uint8_t)3;
                break;
            }
            case TERMINAL_SIGN: {
                e.buf[0] |= 0x10; /* sign type */
                e.buflen = (uint8_t)1;
                break;
            }
            case TERMINAL_VOTE: {
                e.buf[0] |= 0x20; /* vote type */
                e.buflen = (uint8_t)1;
                break;
            }
            default: {
                /* no other case, return */
                return;
            }
        }
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*up()........................................................................*/
void MainSurface::up(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x10; /* control right down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*upLeft()..................................................................*/
void MainSurface::upLeft(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x16; /* control right down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*upRight().................................................................*/
void MainSurface::upRight(void) {
    if (!uiLocked) {
        e.id = ++requstId; /* increasement */
        e.type = CAMERA_CONTROL;
        e.buf[0] = curCamera;
        e.buf[1] = levelSpeed;
        e.buf[2] = vertivalSpeed;
        e.buf[3] &= ~0xff; /* clear buf */
        /* high 4 bits means operation code. one is control camera */
        e.buf[3] &= 0x17; /* control right down */
        e.buflen = (uint8_t)4; /* four lenght */
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*levelSpeedDoTxtChange()...................................................*/
void MainSurface::levelSpeedDoTxtChange(const QString &txt) {
    bool ok;
    if (!txt.isEmpty()) {
        levelSpeed = (uint8_t)txt.toInt(&ok, 10);
        qDebug("levelSpeed speed number = %d\n", levelSpeed);
    }
}
/*verticalSpeedDoTxtChange()................................................*/
void MainSurface::verticalSpeedDoTxtChange(const QString &txt) {
    bool ok;
    if (!txt.isEmpty()) {
        vertivalSpeed = (uint8_t)txt.toInt(&ok, 10);
        qDebug("Vertival speed number = %d\n", vertivalSpeed);
    }
}
/*$ */
void MainSurface::cmrCtlTypeComboBoxValChanged(void) {
    int index;
    index = cmrCtlTypeComboBox->currentIndex();
    if (index == 0) {/* focus */
        cmrCType = FOCUS;
    }
    else if (index == 1) { /* apert */
        cmrCType = APERT;
    }
    else if (index == 2) { /* zoom */
        cmrCType = ZOOM;
    }
    else {
        /* no else case */
    }
    qDebug(" cmrCtlType = %d\n", cmrCType);
}
/*$ */
void MainSurface::curcameraComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = curcameraComboBox->currentText();
    curCamera = (uint8_t)s.toInt(&ok, 10);
    qDebug(" curCamera = %d\n", curCamera);
}
/*$ */
void MainSurface::optTypeComboBoxValChanged(int) {
    int index;
    index = optTypeComboBox->currentIndex();
    if (index == 0) {/* operetion to speak */
        optType = TERMINAL_SPEAK;
        optComboBox->setEnabled(1); /* enabled */
        pausePushBtn->setEnabled(0); /* disabled */
        resumePushBtn->setEnabled(0); /* disabled */
    }
    else if (index == 1) { /* SIGN */
        optType = TERMINAL_SIGN;
        /* disable */
        optComboBox->setEnabled(0); /* disabled */
        pausePushBtn->setEnabled(0); /* enabled */
        resumePushBtn->setEnabled(0); /* enabled */
    }
    else if (index == 2) { /* vote */
        optType = TERMINAL_VOTE;
        optComboBox->setEnabled(0); /* disabled */
        pausePushBtn->setEnabled(1); /* enabled */
        resumePushBtn->setEnabled(1); /* enabled */
    }
    else {
        /* no else case */
    }
    qDebug(" operation type is %d\n", index);
}
/*$ */
void MainSurface::optComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = curcameraComboBox->currentText();
    if (s == "all") {
        curAddr = 0xffff;
    }
    else {   
        curAddr = (uint16_t)s.toInt(&ok, 10);
    }
    qDebug(" curAddr = %d\n", curAddr);
}
/*$ */
void MainSurface::optComboBoxCurValChanged(const QString &txt) {
    bool ok;
    if (!txt.isEmpty()) {
        if (txt == "all") {
            curAddr = 0xffff;
        }
        else {   
            curAddr = (uint16_t)txt.toInt(&ok, 10);
        }
    }
    /* get change value */
    qDebug(" curAddr = %d\n", curAddr);
}
/*$ */
void MainSurface::inputComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = inputComboBox->currentText();
    curInput = (uint8_t)s.toInt(&ok, 10);
    qDebug(" curInput = %d\n", curInput);
}
/*$ */
void MainSurface::outputComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = outputComboBox->currentText();
    curOutput = (uint8_t)s.toInt(&ok, 10);
    qDebug(" curOutput = %d\n", curOutput);
}
/*$ */
void MainSurface::idComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = idComboBox->currentText();
    if (s == "all") {
        curId = 0xffff;
    }
    else {   
        curId = (uint16_t)s.toInt(&ok, 10);
    }
    qDebug(" curId  = %d\n", curId );
}
/*$ */
void MainSurface::idComboBoxCurValChanged(const QString &txt) {
    bool ok;
    if (!txt.isEmpty()) {
        if (txt == "all") {
            curId = 0xffff;
        }
        else {   
            curId = (uint16_t)txt.toInt(&ok, 10);
        }
    }
    /* get change value */
    qDebug(" curId = %d\n", curId);
}
/*$ */
void MainSurface::beginSystem(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = SET_SYS;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= 0x03; /* begin server system */
        e.buflen = (uint8_t)1;
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*$ */
void MainSurface::stopSystem(void) {
    if (!uiLocked) {
        e.id = ++requstId;
        e.type = SET_SYS;
        /*high four bits is opt type, and low four bits means opt code */
        e.buf[0] &= ~0xff; /* clear buf */
        e.buf[0] |= 0x01; /* stop server system */
        e.buflen = (uint8_t)1;
        /* post event to Controller active object */
        ARCS::A0_Controller->POST(&e, this);
        /* set result to NULL */
        setResult("Result:");
        /* set status to Excuting */
        setStatus("Status: Excuting");
        /* lock ui until last request being finished */
        lockUi();
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*$ */
void MainSurface::updateSystem(void) {
    QString fileName;
    /* set request */
    if (!uiLocked) {
        if (pBytesToWriteBuf != (uint8_t *)0) {
            /* free last time space */
            delete pBytesToWriteBuf;
            pBytesToWriteBuf = (uint8_t *)0;
        }
        
        bytesToWrite = 0;
        totalBytes = 0;
        fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) {
            char *msg;
            QByteArray ba = fileName.toLatin1(); 
            msg = ba.data();
            std::ifstream file;
            qDebug("fileName = %s", msg);
            file.open(msg, std::ios::binary|std::ios::ate);
            int fileSize = file.tellg();
            qDebug("fileSize = %d", fileSize);
            char* fileBuf = new char[fileSize];
            file.seekg(0, std::ios::beg);
            file.read(fileBuf, fileSize);
            file.close();
            
            totalBytes = (uint64_t)fileSize;
            
            char *pCurFile;
            QString currentFile = fileName.right(fileName.size() -
                fileName.lastIndexOf('/') - 1);
            QByteArray pCurFileArray = currentFile.toLatin1();  
            pCurFile = pCurFileArray.data();
            qDebug("CurFile = %s", pCurFile);
            int fileNameSize = strlen(pCurFile);
            qDebug("fileNameSize = %d", fileNameSize);
            
            totalBytes += (uint64_t)fileNameSize;
            qDebug("totalBytes = %lld", totalBytes);
            
            char *totalBuf = new char[totalBytes];
            memcpy(totalBuf, pCurFile, fileNameSize);
            memcpy(totalBuf + fileNameSize, fileBuf, fileSize);
            
            pBytesToWriteBuf = (uint8_t *)totalBuf;
            bytesToWrite = totalBytes;
            
            delete fileBuf;
            if (QMessageBox::question(this,
                            "Update System",
                            "Press Yes to Confirm Update\n"
                            "Or No to cancel Update",
                            QMessageBox::Yes,
                            QMessageBox::No) == QMessageBox::Yes) {
                e.id = ++requstId;
                e.type = SET_SYS;
                /*high four bits is opt type,
                    and low four bits means opt code */
                e.buf[0] &= ~0xff; /* clear buf */
                e.buf[0] |= 0x05;  /* update server system */
                e.buf[1] = 0x10;   /* total len and file name len */
                /* set total len */
                setE64_u64(&e.buf[2], totalBytes);
                /* set file name length */
                setE64_u64(&e.buf[10], fileNameSize);
                e.buflen = (uint8_t)0x12;
                /* post event to Controller active object */
                ARCS::A0_Controller->POST(&e, this);
                /* set result to NULL */
                setResult("Result:");
                /* set status to Excuting */
                setStatus("Status: Excuting");
                pSysUpatePro->setRange(0, totalBytes);
                pSysUpatePro->setModal(true);
                cancelUpdate = (bool)0;
                /* lock ui until last request being finished */
                lockUi();
            }
            else {
                totalBytes = 0;
                bytesToWrite = 0;
                if (pBytesToWriteBuf != (uint8_t *)0) {
                    free(pBytesToWriteBuf);
                    pBytesToWriteBuf = (uint8_t *)0;
                }
            }
        }
    }
    else {
        QMessageBox::information(this, "Request Waitting....",
            "Please waitting last requst finish......", QMessageBox::Ok,
            0, 0);
    }
}
/*$ */
void MainSurface::cancelProcessBars(void) {
    totalBytes = 0;
    bytesToWrite = 0;
    if (pBytesToWriteBuf != (uint8_t *)0) {
        free(pBytesToWriteBuf);
        pBytesToWriteBuf = (uint8_t *)0;
    }
    cancelUpdate = (bool)1;
}

