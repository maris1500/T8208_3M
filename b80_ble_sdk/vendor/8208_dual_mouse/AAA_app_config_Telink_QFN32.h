#ifndef _TELINK_QFN32_AAA_H_
#define _TELINK_QFN32_AAA_H_


#define PID_TESTA       0xFF
#define PID_Q15         1 //3m


#define PROJECT_ID        PID_Q15

#if (PROJECT_ID == PID_TESTA)
	#include "../project_id/pid_TESTA.h"
#elif (PROJECT_ID == PID_Q15)
	#include "../project_id/pid_Q15.h"
#else
	#include "../project_id/pid_ms13.h"
#endif

#endif



