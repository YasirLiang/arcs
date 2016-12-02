/*
* @file qt_user.h
* @brief qt user specific function
* @ingroup qt user specific function
* @cond
******************************************************************************
* Build Date on  2016-11-25
* Last updated for version 1.0.0
* Last updated on  2016-11-25
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#ifndef __QT_USER_H__
#define __QT_USER_H__

/*$ qt mainsurface query id command function................................*/
void* QtCmd_queryId(struct TCmdQElem *pElem);
/*$ */
void* QtCmd_switchMatrix(struct TCmdQElem *pElem);
/*$ */
void* QtCmd_optTerminal(struct TCmdQElem *pElem);
/*$ */
void* QtCmd_setLocalSys(struct TCmdQElem *pElem);
/*$ */
void* QtCmd_setTerminalSys(struct TCmdQElem *pElem);
/*$ */
void* QtCmd_cameraControl(struct TCmdQElem *pElem);

/*$ Protocal command between server and ARCS */
void* ServerCmd_queryId(struct TCmdQElem *pElem);
/*$ */
void* ServerCmd_switchMatrix(struct TCmdQElem *pElem);
/*$ */
void* ServerCmd_optTerminal(struct TCmdQElem *pElem);
/*$ */
void* ServerCmd_setTerminalSys(struct TCmdQElem *pElem);
/*$ */
void* ServerCmd_cameraControl(struct TCmdQElem *pElem);
/*$ */
int QtReq_updateSystem(struct TRequestElem *pElem);

#endif /* __QT_USER_H__ */
