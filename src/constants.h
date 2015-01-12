#ifndef CONSTANTS_H
#define CONSTANTS_H

#define STDINBUFFERSIZE						128	// Static stdin buffer size in console_process function
#define STDINDEFAULTCOMMANDSIZE				3   // Default length of a console command
#define CONSOLE_DEFAULT_BAUDRATE			115200

#define SYSTICKPERIODMS 					50
#define LEDBLINKPERIODMS 					5
#define WATCHDOGPERIODMS					999

#define LOGTYPE_ERROR						1
#define LOGTYPE_WARNING						2
#define LOGTYPE_MESSAGE						3

#endif //CONSTANTS_H