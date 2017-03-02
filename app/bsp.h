/*
* @file bsp.h
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
#ifndef bsp_h
#define bsp_h

#define BSP_TICKS_PER_SEC   1000U

void BSP_init(void);
void BSP_terminate(int);

#endif /* bsp_h */

