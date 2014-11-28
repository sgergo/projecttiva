#ifndef CONSOLE_H
#define CONSOLE_H

#define STDINBUFFERSIZE						128	// Static stdin buffer size in console_process function
#define STDINDEFAULTCOMMANDSIZE				3   // Default length of a console command

void console_printtext(const char *format , ...);
void console_execute(char *commandline_received);
void console_process(void);

#endif

