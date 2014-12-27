#ifndef TASK_H
#define TASK_H

typedef void (*taskfunc_t)(void*);

#define TASKPRIORITYLEVEL_LOW 0
#define TASKPRIORITYLEVEL_HIGH 1
#define TASKREPETITION_CONTINUOUS -1

#define EXITCRITICAL 0
#define ENTERCRITICAL 1

typedef struct {
	char const *taskinfo;
	taskfunc_t const taskfunction;
	defuint_t volatile taskperiod;
	defuint_t volatile periodcounter;
	defint_t volatile taskrepetition;
	level_t taskpriority;
	void *taskarg;
}taskentry_t;

void task_process(void);
void task_systick(void);
void task_watchdog_expired (void);
defint_t task_find_task_ID_by_infostring( char* taskstr);

#endif