#ifndef TASK_H
#define TASK_H

typedef void (*taskfunc_t)(void*);

#define TASKREPETITION_CONTINUOUS -1
#define TASKREPETITION_HALT 0
#define TASKREPETITION_ONCE 1

#define EXITCRITICAL false
#define ENTERCRITICAL true

typedef uint32_t tasklist_t;

typedef struct {
	char const *taskname;
	taskfunc_t const taskfunction;
	defuint_t volatile taskperiod;
	defuint_t volatile periodcounter;
	defint_t volatile taskrepetition;
	void *taskarg;
}taskentry_t;

typedef struct {
	tasklist_t volatile tasklist;
	bool criticalsection;
	bool taskisbusy;
	defuint_t taskticked;
}schedulerstate_t;

void task_process(void);
void task_systick(void);
void task_taskdog_expired (void);
defint_t task_launchtask (char* taskname, defint_t repetition, void *arg);
defint_t task_killtask (char* taskname);
void task_addtask_by_index(defuint_t taskindex, defint_t repetition, void *arg);
defint_t task_addtask_by_name(char* taskname, defint_t repetition, void *arg);
void task_cleartask_by_index(defuint_t taskindex);
defint_t task_cleartask_by_name(char* taskname);

#endif