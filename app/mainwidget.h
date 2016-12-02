/*
* @file
* @brief main surface
* @ingroup main surface
* @cond
******************************************************************************
* Build Date on  2016-10-20
* Last updated for version 1.0.0
* Last updated on  2016-10-24
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __MAIN_WIDGET_H__
#define __MAIN_WIDGET_H__

/*! Including file----------------------------------------------------------*/
#include <QFile>
#include "user.h"
#include "ui_mainwidget.h"

/*! class MainSurface */
class MainSurface : public QWidget, public Ui_MainWidget {
    Q_OBJECT
public:
    class QValidator *pLvSpIntValidator;
    class QValidator *pVtSpIntValidator;
    class SystemSetDialog *sysDlg;
    class QProgressDialog *pSysUpatePro;
    static ARCS::RequestEvt e;
    uint8_t *pBytesToWriteBuf;
    uint64_t totalBytes;
    uint64_t bytesToWrite;
    bool upDateCel; /* update cel */
    
    MainSurface(QWidget * parent = 0);
    ~MainSurface(void);
    static MainSurface *instance(void);
    void lockUi(void);
    void unLockUi(void);
    void setStatus(char const * const status_);
    void setResult(char const * const result_);
    uint64_t loadUpdateData(uint8_t * const buf, uint64_t reqSize);
    void updateBarProccess(void); 
private:
    bool uiLocked; /*it's setted to one until user's request is finished. */
    uint32_t requstId;
    enum cameraCtlEType {
        FOCUS,
        APERT,
        ZOOM
    }cmrCType;
    enum TenumOptType {
        TERMINAL_SPEAK,
        TERMINAL_SIGN,
        TERMINAL_VOTE
    }optType;
    uint8_t curCamera;
    uint8_t levelSpeed;
    uint8_t vertivalSpeed;
    uint8_t curOutput;
    uint8_t curInput;
    uint16_t queryAddr;
    uint16_t curAddr;
    uint16_t curId;
    void setE64_u64(uint8_t *const buf, uint64_t const ie64);
protected:
    /*! override closeEvent() virtual function */
    void closeEvent(QCloseEvent *event);    
private slots:
    void add(void);
    void begin(void);
    void cameraSwitch(void);
    void clearUnConnected(void);
    void clearUnRegister(void);
    void down(void);
    void downLeft(void);
    void downRight(void);
    void left(void);
    void right(void);
    void matrixSwitch(void);
    void pause(void);
    void query(void);
    void reduce(void);
    void regain(void);
    void setSys(void);
    void showConnected(void);
    void stop(void);
    void up(void);
    void upLeft(void);
    void upRight(void);
    void levelSpeedDoTxtChange(const QString &);
    void verticalSpeedDoTxtChange(const QString &);
    void cmrCtlTypeComboBoxValChanged(void);
    void curcameraComboBoxValChanged(void);
    void optTypeComboBoxValChanged(void);
    void optComboBoxValChanged(void);
    void inputComboBoxValChanged(void);
    void outputComboBoxValChanged(void);
    void idComboBoxValChanged(void);
    void beginSystem(void);
    void stopSystem(void);
    void updateSystem(void);
    void cancelProcessBars(void);
};

#endif /* __MAIN_WIDGET_H__ */

