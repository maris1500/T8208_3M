#ifndef _TELINK_QFN32_AAA_H_
#define _TELINK_QFN32_AAA_H_


#define PID_TESTA         0xFF
#define PID_104           1 //3m
#define PID_Q15           2 //3m


#define PROJECT_ID        PID_Q15

#if (PROJECT_ID == PID_TESTA)
	#include "../project_id/pid_TESTA.h"
#elif (PROJECT_ID == PID_104)
	#include "../project_id/pid_104.h"
#elif (PROJECT_ID == PID_Q15)
	#include "../project_id/pid_Q15.h"
#else
	#include "../project_id/pid_ms13.h"
#endif

#endif



