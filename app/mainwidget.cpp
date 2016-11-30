/*
* @file
* @brief main surface
* @ingroup main surface
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-11-21
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
#include "user.h"
#include "systemset.h"
#include "mainwidget.h"
/*$ Class Local decralation-------------------------------------------------*/
ARCS::RequestEvt MainSurface::e(ARCS::REQUEST_SIG,
                    0U,
                    (TRequestType)0,
                    (uint8_t *)0,
                    0U,
                    QT_REQUEST);
/* Local mainsurface instance-----------------------------------------------*/
static MainSurface *l_instance;
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
    /* set result to NULL */
    setResult("Result:");
    /* set status to Ready */
    setStatus("Status: Ready");
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
        this, SLOT(optTypeComboBoxValChanged()));
     connect(optComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(optComboBoxValChanged()));
     connect(optComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(inputComboBoxValChanged()));
     connect(inputComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(inputComboBoxValChanged()));
     connect(outputComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(outputComboBoxValChanged()));
     connect(idComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(idComboBoxValChanged()));
}
/*~MainSurface()............................................................*/
MainSurface::~MainSurface(void) {
    qDebug("Exit MainSurface");
    delete pLvSpIntValidator;
    delete pVtSpIntValidator;
    delete sysDlg;
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
        qresultTableWidget->clear();
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

void MainSurface::curcameraComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = curcameraComboBox->currentText();
    curCamera = (uint8_t)s.toInt(&ok, 10);
    qDebug(" curCamera = %d\n", curCamera);
}

void MainSurface::optTypeComboBoxValChanged(void) {
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
        pausePushBtn->setEnabled(1); /* enabled */
        resumePushBtn->setEnabled(1); /* enabled */
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
    qDebug(" curCamera = %d\n", curAddr);
}

void MainSurface::inputComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = inputComboBox->currentText();
    curInput = (uint8_t)s.toInt(&ok, 10);
    qDebug(" curInput = %d\n", curInput);
}

void MainSurface::outputComboBoxValChanged(void) {
    bool ok;
    QString s;
    /* get change value */
    s = outputComboBox->currentText();
    curOutput = (uint8_t)s.toInt(&ok, 10);
    qDebug(" curOutput = %d\n", curOutput);
}

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

