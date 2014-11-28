#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "console.h"
#include "console_command.h"
#include "cmdline.h"
#include "uartstdio.h"
//#include "board.h"



static int console_command_help(int argc, char *argv[]) {
	tCmdLineEntry *psEntry;

    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    psEntry = &g_psCmdTable[0]; // Point at the beginning of the command table.

    while(psEntry->pcCmd) {
        UARTprintf("%10s: %s\n", psEntry->pcCmd, psEntry->pcHelp); // Print the command name and the brief description.
        psEntry++; // Advance to the next entry in the table.
    }
    return(0);
}

void console_command_execute(char *commandline_received) {
	int32_t ret;
	ret = CmdLineProcess(commandline_received);
	if (ret) console_printtext("Error code: %d\n", ret); // If CmdLineProcess returns with a non-zero something went bad
}

// Command table entries - fill it!
tCmdLineEntry g_psCmdTable[] = {
    { "help", console_command_help,   "Display list of commands" },   
    { 0, 0, 0 } // Don't touch it, last entry must be a terminating NULL entry
};