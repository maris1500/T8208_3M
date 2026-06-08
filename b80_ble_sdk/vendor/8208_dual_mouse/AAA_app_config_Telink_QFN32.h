#ifndef _TELINK_QFN32_AAA_H_
#define _TELINK_QFN32_AAA_H_


#define PID_TESTA         0xFF
#define PID_104           42 //3m



#define PROJECT_ID        PID_104

#if (PROJECT_ID == PID_TESTA)
	#include "../project_id/pid_TESTA.h"
#elif (PROJECT_ID == PID_104)
	#include "../project_id/pid_104.h"
#elif (PROJECT_ID == PID_MS358B)
	#include "../project_id/pid_WT10.h"
#else
	#include "../project_id/pid_ms13.h"
#endif

#endif



