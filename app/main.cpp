/*
* @file main.c
* @brief
* @ingroup
* @cond
******************************************************************************
* Build Date on  2016-11-24
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
#include "user.h"
#include "mainwidget.h"

//............................................................................
#if 0
static QP::QEvt const l_controllerQueueSto[ARCS::CEVT_N];
#endif
static QP::QEvt const *l_transmitorQueueSto[TEVT_N];
static QP::QSubscrList l_subscrSto[ARCS::MAX_PUB_SIG];

// storage for event pools...
static QF_MPOOL_EL(ARCS::TransmitEvt) l_smlPoolSto[TEVT_N];

//............................................................................
int main(int argc, char *argv[]) {
    QP::GuiApp app(argc, argv);
    MainSurface gui;

    gui.show();

    QP::QF::init(); // initialize the framework
    BSP_init();     // initialize the BSP

    // object dictionaries...
    QS_OBJ_DICTIONARY(l_smlPoolSto);

    QP::QF::psInit(l_subscrSto, Q_DIM(l_subscrSto)); // init publish-subscribe

    // initialize event pools...
    QP::QF::poolInit(l_smlPoolSto,
                     sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

    // start the active objects...
    ARCS::A0_Controller->start((uint_fast8_t)1,
                             static_cast<QP::QEvt const **>(0), 0U,
                             static_cast<void *>(0),
                             0, static_cast<QP::QEvt const *>(0));
    ARCS::A0_Transmitor->start((uint_fast8_t)2,
                                l_transmitorQueueSto,
                                Q_DIM(l_transmitorQueueSto),
                                (void *)0, 0U);

    return QP::QF::run(); // calls qApp->exec()
}

