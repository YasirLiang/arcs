/*
* @file
* @brief user timer
* @ingroup user timer
* @cond
******************************************************************************
* Build Date on  2016-10-25
* Last updated for version 1.0.0
* Last updated on  2016-10-25
*
*                    Moltanisk Liang
*                    ---------------------------
*                    Avdecc Reception Control System
*
* Copyright (C) Moltanisk Liang, GuangZhou Suntron. All rights reserved.
******************************************************************************
* @endcond
*/
#include "usertimer.h"

#ifdef WIN32
extern "C" Timestamp clk_monotonic(void);
Timestamp clk_monotonic(void) {
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);

    return count.QuadPart;
}
#elif defined __linux__
extern "C" Timestamp clk_monotonic(void);
Timestamp clk_monotonic(void) {
    struct timespec tp;
    Timestamp time;

    if (clock_getres( CLOCK_MONOTONIC, &tp ) != 0) {
        printf("Timer not supported in "
            "asios_Clock_monotonic(), asios.c\n");
    }

    clock_gettime( CLOCK_MONOTONIC, &tp );
    time = (Timestamp)(tp.tv_sec * 1000) + (Timestamp)(tp.tv_nsec/1000000);
    return time;
}
#elif defined __MACH__
extern "C" Timestamp clk_monotonic(void);
Timestamp clk_monotonic(void) {
    struct timespec tp;
    Timestamp time;

    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tp.tv_sec = mts.tv_sec;
    tp.tv_nsec = mts.tv_nsec;

    time = (Timestamp)(tp.tv_sec * 1000) + (Timestamp)(tp.tv_nsec/1000000);
    return time;
}
#endif

#ifdef WIN32
extern "C" uint32_t clk_convert_to_ms(Timestamp time_stamp);
uint32_t clk_convert_to_ms(Timestamp time_stamp) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (uint32_t)((time_stamp * 1000/freq.QuadPart) & 0xfffffff);
}
#elif defined __linux__ || defined __MACH__
extern "C" uint32_t clk_convert_to_ms(Timestamp time_stamp);
uint32_t clk_convert_to_ms(Timestamp time_stamp) {
    return time_stamp;
}
#endif

/*userTimerstart()..........................................................*/
void userTimerstart(uint32_t duration_ms, TUserTimer *timer) {
    timer->running = true;
    timer->elapsed = false;
    timer->count = duration_ms;
    timer->start_time = clk_monotonic();
}
/*userTimerstop()...........................................................*/
void userTimerstop(TUserTimer *timer) {
    timer->running = false;
    timer->elapsed = false;
}
/*userTimertimeout()........................................................*/
bool userTimertimeout(TUserTimer *timer) {
    if ((timer->running)
         && (!timer->elapsed))
    {
        uint32_t elapsed_ms;
        Timestamp current_time = clk_monotonic();
        elapsed_ms = (uint32_t)clk_convert_to_ms(current_time - timer->start_time);

        if (elapsed_ms > timer->count) {
            timer->elapsed = true;
        }
    }

    return timer->elapsed;
}

