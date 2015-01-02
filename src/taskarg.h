#ifndef TASKARG_H
#define TASKARG_H

typedef struct {
	defuint_t uintval;
	defint_t intval;
	float floatval;
}default_taskarg_t;

typedef struct {
	unsigned char nmeasentencebuffer[1024];
	defuint_t volatile nmeasentencesize;
	defuint_t gpsfixed;
	bool fixfilter;
	bool nmeasentencereceived;
}nmea_taskarg_t;


#endif
