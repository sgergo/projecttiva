#include <stdint.h>
#include <stdbool.h>

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "console.h"
#include "console_command.h"
#include "cmdline.h"
#include "uartstdio.h"
#include "board_spi.h"
#include "cc1101.h"
#include "cc1101regs.h"


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
        ROM_SysCtlDelay(100000);
    }

    UARTprintf("\n");
    return(0);
}

static int console_command_snop(int argc, char *argv[]) {
    uint8_t status;
    //status = board_spi_send(0x3d);
    status = cc1101_cmd(CC1101_SNOP);
    UARTprintf("Answered: 0x%02x\n", status);
    return(0);
}

static int console_command_get(int argc, char *argv[]) {
    uint8_t status;
    status = cc1101_read_status_reg(CC1101_MARCSTATE);
    UARTprintf("Answered: 0x%02x\n", status);
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
    { "snop", console_command_snop,   "Get status byte from CC1101" },
    { "get", console_command_get,   "Get register content" },
    { 0, 0, 0 } // Don't touch it, last entry must be a terminating NULL entry
};