#ifndef __TIMINGSET_H
#define __TIMINGSET_H
#include "sys.h"


void Timing_1s		(void);	/*1s   Timer scan execution function*/
void Timing_1ms	    (void);	/*1ms  Timer scan execution function*/
void Timing_2ms		(void);	/*2ms  Timer scan execution function*/
void Timing_5ms		(void);	/*5ms  Timer scan execution function*/
void Timing_10ms    (void); /*10ms Timer scan execution function*/
void Timing_50ms    (void); /*50ms Timer scan execution function*/
void Timing_500ms   (void); /*500ms Timer scan execution function*/



void Servo_SetAngle(uint8_t angle);

#endif /*__TIMINGSET_H*/
