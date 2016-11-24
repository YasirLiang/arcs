/*
* @file extern_port_qt.c
* @brief qt specific exern port
* @ingroup qt specific exern port
* @cond
******************************************************************************
* Build Date on  2016-11-22
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
/*$ ExternPortQt::ExternPortQt()............................................*/
ExternPortQt::ExternPortQt(void (*init)(void),
    int (*send)(void const * const, int),
    int (*recv)(void * const, int),
    int (*destroy)(void))
{
    vTable.init = init;
    vTable.send = send;
    vTable.recv = recv;
    vTable.destroy = destroy;
}
/*$ ExternPortQt::initial().................................................*/
void ExternPortQt::initial(void) {
    ExternPort_init(&vTable);
}
/*$ ExternPortQt::send()....................................................*/
int ExternPortQt::send(void const * const buf, int len) {
    return ExternPort_send(&vTable, buf, len);
}
/*$ ExternPortQt::recv()....................................................*/
int ExternPortQt::recv(void * const buf, int len) {
    return ExternPort_recv(&vTable, buf, len);
}
/*$ ExternPortQt::destroy().................................................*/
int ExternPortQt::destroy(void) {
    return ExternPort_destroy(&vTable);
}
/*$ ExternPortQt::getVtbl().................................................*/
TExternPortVtbl const *ExternPortQt::getVtbl(void) {
    return &vTable;
}

