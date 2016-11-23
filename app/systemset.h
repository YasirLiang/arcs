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
#ifndef __SYSTEMSET_H__
#define __SYSTEMSET_H__

/*! class SystemSetDialog */
class SystemSetDialog : public QDialog, public Ui_SysSetDialog {
    Q_OBJECT
public:
    SystemSetDialog(QWidget * parent = 0);
private:
    uin8_t serialPort;
    uin8_t tcpIp[20];
    int32_t tcpPort;
private slots:
    void changePass(void);
    void changePort(void);
} ;

#endif /* __SYSTEMSET_H__ */

