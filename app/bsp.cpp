/*
* @file bsp.c
* @brief 
* @ingroup
* @Product BSP for ARCS-GUI example with Qt5
* @cond
******************************************************************************
* Build Date on  2016-11-25
* Last updated for version 1.0.0
* Last updated on  2016-11-28
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#include <QtWidgets>
#include "user.h"

/*..........................................................................*/
void QP::QF_onClockTick(void) {
    QP::QF::TICK_X(0U, &l_time_tick);
}
/*..........................................................................*/
void QP::QF::onStartup(void) {
    QP::QF_setTickRate(BSP_TICKS_PER_SEC);
}
/*..........................................................................*/
void QP::QF::onCleanup(void) {
}
/*..........................................................................*/
void BSP_init(void) {
    Q_ALLEGE(QS_INIT((char *)0));
    QS_RESET();
}
/*..........................................................................*/
void BSP_terminate(int) {
    qDebug("terminate");
    /* stop the QF::run() thread */
    QP::QF::stop();
    /* quit the Qt application *after* the QF_run() has stopped */
    qApp->quit();
}
/*..........................................................................*/
void Q_onAssert(char_t const * const module, int_t loc) {
    QMessageBox::critical(0, "PROBLEM",
        QString("<p>Assertion failed in module <b>%1</b>,"
                "location <b>%2</b></p>")
            .arg(module)
            .arg(loc));
    QS_ASSERTION(module, loc, 10000); // send assertion info to the QS trace
    qFatal("Assertion failed in module %s, location %d", module, loc);
}

