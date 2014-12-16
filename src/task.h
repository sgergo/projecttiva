#ifndef TASK_H
#define TASK_H

typedef void (*taskfunc_t)(void*);

#define TASKPRIORITYLEVEL_LOW 0
#define TASKPRIORITYLEVEL_HIGH 1
#define TASKREPETITION_INFINITE -1

typedef struct {
	char const *taskinfo;
	taskfunc_t const taskfunction;
	uint32_t volatile taskperiod;
	uint32_t volatile periodcounter;
	int32_t volatile taskrepetition;
	uint8_t taskpriority;
	void *taskarg;
}taskentry_t;

void task_process(void);
void task_systick(void);
int32_t task_find_task_ID( char* taskstr);

#endif