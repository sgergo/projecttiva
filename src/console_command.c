#include <stdint.h>
#include <stdbool.h>

#include "console.h"
#include "console_command.h"
#include "cmdline.h"
#include "uartstdio.h"
#include "board_spi.h"

static int console_command_help(int argc, char *argv[]) {
	tCmdLineEntry *psEntry;

    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    psEntry = &g_psCmdTable[0]; // Point at the beginning of the command table.

    while(psEntry->pcCmd) {
        // Print the command name and the brief description.
        UARTprintf("%10s: %s\n", psEntry->pcCmd, psEntry->pcHelp);
        // Advance to the next entry in the table. 
        psEntry++; 
    }
    UARTprintf("\n");
    return(0);
}

void console_command_execute(char *commandline_received) {
	int32_t ret;
	ret = CmdLineProcess(commandline_received);

    // If CmdLineProcess returns with a non-zero value something went wrong
	if (ret) 
        console_printtext("Unknown command, error code: %d\n", ret); 
}

// Command table entries - fill it!
tCmdLineEntry g_psCmdTable[] = {
    { "help", console_command_help,   "Display list of commands" },
    { 0, 0, 0 } // Don't touch it, last entry must be a terminating NULL entry
};