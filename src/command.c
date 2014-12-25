#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"

#include "console.h"
#include "command.h"
#include "cmdline.h"
#include "uartstdio.h"

#include "task.h"


extern taskentry_t tasktable[];
uint8_t volatile command_verbosity_level;

// Command 'help'
static int command_help(int argc, char *argv[]) {
	tCmdLineEntry *psEntry;

    UARTprintf("\nAvailable commands\n");
    UARTprintf("-----------------------------------------------------------------------------\n");
    UARTprintf("%10s %20s %10s %10s\n", "Command", "Args", "Format", "Description");
    UARTprintf("-----------------------------------------------------------------------------\n");

    psEntry = &g_psCmdTable[0]; // Point at the beginning of the command table.

    while(psEntry->pcCmd) {
        // Print the command name and the brief description.
        // UARTprintf("%10s: %s\n", psEntry->pcCmd, psEntry->pcArgs, psEntry->pcFormat, psEntry->pcHelp);
        UARTprintf("%10s %20s %10s %10s\n", psEntry->pcCmd, psEntry->pcArgs, psEntry->pcFormat, psEntry->pcHelp);
        // Advance to the next entry in the table. 
        psEntry++;
    }
    
    UARTprintf("\n");
    return(0);
}

// Command 'example1'
static int command_example1(int argc, char *argv[]) {

    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing argument.\n");
        return (0);
    }
    
    if (!strcmp (argv[1], "forever")) {
        tasktable[0].taskrepetition = -1;
        UARTprintf("example1 task is on forever.\n");
    } else if (!strcmp (argv[1], "off")) {
        tasktable[0].taskrepetition = 0;
        UARTprintf("example1 task is off.\n");
    } else {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
    }

    return(0);
}

// Command 'example2'
static int command_example2(int argc, char *argv[]) {

    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing argument.\n");
        return (0);
    }
    
    if (!strcmp (argv[1], "on")) {
        tasktable[1].taskrepetition = 10;
        UARTprintf("example2 task will be repeated 10 times.\n");
    } else if (!strcmp (argv[1], "off")) {
        tasktable[1].taskrepetition = 0;
        UARTprintf("example2 task is off.\n");
    } else {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
    }

    return(0);
}

// Command 'example3'
static int command_example3(int argc, char *argv[]) {
    uint32_t num;
    char *endptr;

    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing value.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);
    if (*endptr != 0 || errno != 0) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input: %s\n", argv[1]);
        return (0);
    }       

    // TODO: num contains a valid value, now do something with it
    UARTprintf("example3 command received.\n");
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
        UARTprintf("verbosity level: %d\n", command_verbosity_level);
        return (0);
    }

    if (!strcmp (argv[1], "none")) {
        command_verbosity_level = VERBOSITY_NONE;
    } else if (!strcmp (argv[1],"error")) {
        command_verbosity_level = VERBOSITY_ERROR;
    } else if (!strcmp (argv[1],"all")) {
        command_verbosity_level = VERBOSITY_ALL;
    } else {
        UARTprintf("error: invalid input.\n");
        return (0);
    }

    return (0);       
}

void command_execute(char *commandline_received) {
	int32_t ret;
	ret = CmdLineProcess(commandline_received);

    // If CmdLineProcess returns with a non-zero value something went wrong
	if (ret) 
        console_printtext("Unknown command, error code: %d\n", ret); 
}

// Command table entries - fill it!
tCmdLineEntry g_psCmdTable[] = {

    { "ex1", "{'forever'/'off'}" , "ascii", command_example1, "Starts/stops a continuous task." },
    { "ex2", "{'on'/'off'}" , "ascii", command_example2, "Starts/stops a task with 10 repetitions." },
    { "ex3", "{VALUE}" , "hex", command_example3, "Command with a hex value input." },
   
    { "help", "-" , "-", command_help,   "Display list of commands." }, 
    { "rep", "-" , "-",  command_report,   "Reports state variables." },
    { "rst", "-" , "-",  command_reset,   "Reset." },
    { "verb", "{none/error/all}" , "ascii",  command_setverbosity, "Sets verbosity level." },
    { 0, 0, 0, 0, 0} // Don't touch it, last entry must be a terminating NULL entry
};
