#-----------------------------------------------------------------------------
# Product: DPP-GUI exampe for Qt5
# Last Updated for Version: QP 5.5.0/Qt5.x
# Date of the Last Update:  2015-09-26
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) Quantum Leaps, LLC. All rights reserved.
#
# This program is open source software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# Alternatively, this program may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GNU General Public License and are specifically designed for
# licensees interested in retaining the proprietary status of their code.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Contact information:
# http://www.state-machine.com
# mailto:info@state-machine.com
#-----------------------------------------------------------------------------

TEMPLATE = app

QT      += core gui widgets sql network
TARGET   = arcs-gui
DEFINES += QT_NO_STATEMACHINE

QPCPP = ../deplib/qpcpp

INCLUDEPATH = . \
    $$QPCPP/include \
    $$QPCPP/ports/qt

SOURCES += \
    database.cpp \
    extern_port.c \
    ring_buffer.c \
    port_message_state_machine.cpp \
    main.cpp \
    bsp.cpp \
    mainwidget.cpp \
    systemset.cpp \
    commander.cpp \
    command.c \
    qt_user.cpp \
    request.c \
    controller.cpp \
    requestor.cpp \
    port_inflight_state_machine.cpp \
    transmitor.cpp \
    usertimer.c \
    circular_link_queue.c \
    inflight.c

HEADERS += \
    version.h \
    circular_link_queue.h \
    extern_port.h \
    port_message_state_machine.h \
    port_inflight_state_machine.h \
    usertimer.h \
    ring_buffer.h \
    protocal_qt.h \
    database.h \
    bsp.h \
    list.h \
    systemset.h \
    uos.h \
    user.h \
    command.h \
    commander.h \
    qt_user.h \
    controller.h \
    inflight.h \
    request.h \
    requestor.h \
    arcs.h \
    transmitor.h \
    mainwidget.h

FORMS += login.ui \
    mainwidget.ui \
    systemset.ui

##############################################################################
# NOTE:
# This project demonstrats how to build the QP/C++ framework from sources,
# as opposed to linking the QP/C++ library. The following headers and sources
# are included from QP/C++.

# QP-Qt port headers/sources
HEADERS +=  \
    $$QPCPP/ports/qt/tickerthread.h \
    $$QPCPP/ports/qt/aothread.h \
    $$QPCPP/ports/qt/guiapp.h \
    $$QPCPP/ports/qt/guiactive.h \
    $$QPCPP/ports/qt/pixellabel.h

SOURCES += \
    $$QPCPP/ports/qt/qf_port.cpp \
    $$QPCPP/ports/qt/guiapp.cpp \
    $$QPCPP/ports/qt/pixellabel.cpp


# QP/C++ headers/sources
SOURCES += \
    $$QPCPP/source/qep_hsm.cpp \
    $$QPCPP/source/qep_msm.cpp \
    $$QPCPP/source/qf_act.cpp \
    $$QPCPP/source/qf_actq.cpp \
    $$QPCPP/source/qf_defer.cpp \
    $$QPCPP/source/qf_dyn.cpp \
    $$QPCPP/source/qf_mem.cpp \
    $$QPCPP/source/qf_ps.cpp \
    $$QPCPP/source/qf_qact.cpp \
    $$QPCPP/source/qf_qeq.cpp \
    $$QPCPP/source/qf_qmact.cpp \
    $$QPCPP/source/qf_time.cpp

INCLUDEPATH += $$QPCPP/source


CONFIG(debug, debug|release) {
} else {
    # Release build configuartion
    DEFINES += NDEBUG
}
