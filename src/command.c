#include <stdint.h>
#include "types.h"
#include "constants.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "uartstdio.h"

#include "console.h"
#include "command.h"
#include "cmdline.h"
#include "task.h"
#include "taskarg.h"

loglevel_t volatile loglevel;

// Command 'help'
static int command_help(int argc, char *argv[]) {
	commandentry_t *entry;

    console_printtext("\nAvailable commands\n");
    console_printtext("-------------------------------------------------------------------------------------------------\n");
    console_printtext("%10s %40s %10s %10s\n", "Command", "Args", "Format", "Description");
    console_printtext("-------------------------------------------------------------------------------------------------\n");

    entry = &commandtable[0]; // Point at the beginning of the command table.

    while(entry->commandname_ptr) {

        // Print the command name and the brief description.
        console_printtext("%10s %40s %10s %10s\n", 
            entry->commandname_ptr, 
            entry->commandarg_ptr, 
            entry->commandargformat_ptr, 
            entry->commandhelptext_ptr );

        // Advance to the next entry in the table. 
        entry++;
    }
    
    console_printtext("\n");
    return(0);
}

// Command 'example1'
static int command_example1(int argc, char *argv[]) {
    
    if (argc < 2) {
        console_printlog(LOGTYPE_ERROR, "error: missing argument.\n");
        return (0);
    }

    if (!strcmp (argv[1], "on")) {

        if (!task_launchtask ("example1", TASKREPETITION_CONTINUOUS, NULL))
            console_printlog(LOGTYPE_MESSAGE, "example1 task is continuously on.\n");
        else
            console_printlog(LOGTYPE_ERROR, "error: couldn't launch task.\n");

    } else if (!strcmp (argv[1], "off")) {

        if (!task_killtask ("example1"))
            console_printlog(LOGTYPE_MESSAGE, "example1 task is off.\n");
        else
            console_printlog(LOGTYPE_ERROR, "error: couldn't stop task.\n");
    } else
        console_printlog(LOGTYPE_ERROR, "error: invalid input.\n");

    return(0);
}

// Command 'example2'
static int command_example2(int argc, char *argv[]) {

    if (argc < 2) {
        console_printlog(LOGTYPE_ERROR, "error: missing argument.\n");
        return (0);
    }

    if (!strcmp (argv[1], "on")) {

        if (!task_launchtask ("example2", 10, NULL))
            console_printlog(LOGTYPE_MESSAGE, "example2 task will be repeated 10 times.\n");
        else
            console_printlog(LOGTYPE_ERROR, "error: couldn't launch task.\n");
    } else if (!strcmp (argv[1], "off")) {

        if (!task_killtask ("example2"))
            console_printlog(LOGTYPE_MESSAGE, "example2 task is off.\n");
        else
            console_printlog(LOGTYPE_ERROR, "error: couldn't stop task.\n");
    } else {
        console_printlog(LOGTYPE_ERROR, "error: invalid input.\n");
    }

    return(0);
}

// Command 'example3'
static int command_example3(int argc, char *argv[]) {
    long num;
    char *endptr;
    static default_task_arg_t task_arg;
   
    if (argc < 2) {
        console_printlog(LOGTYPE_ERROR, "error: missing value.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);

    if (*endptr != 0 || errno != 0) {
        console_printlog(LOGTYPE_ERROR, "error: invalid input.\n");
        return (0);
    }

    // TODO: num contains a valid value, now do something with it
    task_arg.uintval = num;
    if (!task_launchtask ("example3", TASKREPETITION_ONCE, &task_arg))
            console_printlog(LOGTYPE_MESSAGE, "example3 command received with value: 0x%02x\n", num);
        else
            console_printlog(LOGTYPE_ERROR, "error: couldn't launch task.\n");
    return(0);
}

// Command 'ext1'
static int command_ext1(int argc, char *argv[]) {
    
    if (argc < 2) {
        console_printlog(LOGTYPE_ERROR, "error: missing argument.\n");
        return (0);
    }

    if (!strcmp (argv[1], "on")) {

        timer_example_enable ();

    } else if (!strcmp (argv[1], "off")) {

        timer_example_disable ();
    } else
        console_printlog(LOGTYPE_ERROR, "error: invalid input.\n");

    return(0);
}

// Command 'rep'
static int command_report(int argc, char *argv[]) {
    
    return(0);
}

// Command 'rst'
static int command_reset(int argc, char *argv[]) {

    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
    return (0);
}

// Command 'verb'
static int command_setverbosity(int argc, char *argv[]) {
    if (argc < 2) {
        console_printtext("verbosity level: %d\n", loglevel.byte);
        return (0);
    }

    // Clear log level byte
    loglevel.byte = 0x00;

    if (!strcmp (argv[1], "n")) {
        loglevel.byte = 0x00;
    } else if (!strcmp (argv[1],"e")) {
        loglevel.bits.error = 1;
    } else if (!strcmp (argv[1],"w")) {
        loglevel.bits.warning = 1;
    } else if (!strcmp (argv[1],"m")) {
        loglevel.bits.message = 1;
    } else if (!strcmp (argv[1],"a")) {
        loglevel.byte = 0xff;
    } else {
        console_printlog(LOGTYPE_ERROR, "error: invalid input.\n");
        return (0);
    }

    return (0);       
}

void command_execute(char *commandline_received) {
	defint_t ret;
	ret = cmdline_process(commandline_received);

    // If CmdLineProcess returns with a non-zero value something went wrong
	if (ret) 
        console_printlog(LOGTYPE_ERROR, "Unknown command, error code: %d\n", ret); 
}

// Command table entries - fill it!
commandentry_t commandtable[] = {

    { "ex1", "{on/off}" , "ascii", command_example1, "Starts/stops a continuous task." },
    { "ex2", "{on/off}" , "ascii", command_example2, "Starts/stops a task with 10 repetitions." },
    { "ex3", "{VALUE}" , "hex", command_example3, "Execute a task with a hex value input." },
    { "ext1", "{on/off}" , "ascii", command_ext1, "Starts/stops an external interrupt task, modelled with a timer." },
    { "help", "-" , "-", command_help,   "Display list of commands." }, 
    { "rep", "-" , "-",  command_report,   "Reports state variables." },
    { "rst", "-" , "-",  command_reset,   "Reset." },
    { "verb", "{n/e/w/m/a}" , "ascii",  command_setverbosity, "Sets verbosity level." },
    { 0, 0, 0, 0, 0} // Don't touch it, last entry must be a terminating NULL entry
};
