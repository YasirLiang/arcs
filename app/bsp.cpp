//****************************************************************************
// Product: BSP for DPP-GUI example with Qt5
// Last Updated for Version: QP/C++ 5.5.0/Qt 5.x
// Last updated on  2015-09-26
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// http://www.state-machine.com
// mailto:info@state-machine.com
//****************************************************************************
#include <QtWidgets>
//-----------------
#include "user.h"

//............................................................................
void QP::QF_onClockTick(void) {
    QP::QF::TICK_X(0U, &l_time_tick);
}
//............................................................................
void QP::QF::onStartup(void) {
    QP::QF_setTickRate(BSP_TICKS_PER_SEC);
}
//............................................................................
void QP::QF::onCleanup(void) {
}
//............................................................................
void BSP_init(void) {
    Q_ALLEGE(QS_INIT((char *)0));
    QS_RESET();
}
//............................................................................
void BSP_terminate(int) {
    qDebug("terminate");
    QP::QF::stop(); // stop the QF::run() thread
    qApp->quit(); // quit the Qt application *after* the QF_run() has stopped
}

//............................................................................
void Q_onAssert(char_t const * const module, int_t loc) {
    QMessageBox::critical(0, "PROBLEM",
        QString("<p>Assertion failed in module <b>%1</b>,"
                "location <b>%2</b></p>")
            .arg(module)
            .arg(loc));
    QS_ASSERTION(module, loc, 10000); // send assertion info to the QS trace
    qFatal("Assertion failed in module %s, location %d", module, loc);
}

