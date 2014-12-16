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
#include "board_spi.h"
#include "board_eeprom.h"
#include "cc1101.h"
#include "cc1101regs.h"
#include "task.h"

extern cc1101_state_t volatile cc1101_state;
extern taskentry_t tasktable[];
uint8_t volatile command_verbosity_level;

// Command 'help'
static int command_help(int argc, char *argv[]) {
	tCmdLineEntry *psEntry;

    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    psEntry = &g_psCmdTable[0]; // Point at the beginning of the command table.

    while(psEntry->pcCmd) {
        // Print the command name and the brief description.
        UARTprintf("%10s: %s\n", psEntry->pcCmd, psEntry->pcHelp);
        // Advance to the next entry in the table. 
        psEntry++;
        ROM_SysCtlDelay(100000); // TODO: This is needed otherwise minicom outputs damaged text
    }
    UARTprintf("------------------\n");
    UARTprintf("Useful register addresses: MARCSTATE: 0x35, PKTLEN: 0x06, ADDR: 0x09\n");
    UARTprintf("------------------\n");

    UARTprintf("\n");
    return(0);
}

// Command 'adr'
static int command_setaddress(int argc, char *argv[]) {
    uint32_t num;
    char *endptr;

    if (argc < 3) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing fields.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[2], &endptr, 10);
    if (*endptr != 0 || errno != 0) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
        return (0);
    }       

    if (num > 255) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid value.\n");
        return (0);
    } else {
        if (!strcmp (argv[1], "dev")) {    
                cc1101_write_reg(CC1101_ADDR, (uint8_t) num);
                cc1101_state.device_address = (uint8_t) num;
        
        } else if (!strcmp (argv[1],"dst")) {
            cc1101_state.destination_address = 0;
        } else {
            if (command_verbosity_level > VERBOSITY_NONE)
                UARTprintf("error: invalid input.\n");
            return (0);
        }

        if (command_verbosity_level > VERBOSITY_ERROR) 
            UARTprintf("%s address updated.\n", argv[1]);  
    }
    return (0);       
}

// Command 'eep'
static int command_eeprom(int argc, char *argv[]) {
    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing field.\n");
        return (0);
    }

    if (!strcmp (argv[1], "load")) {
        if (board_eeprom_loadregisterspace() && command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: EEPROM is corrupted.\n");
        else
            UARTprintf("register space is updated from EEPROM.\n");
    } else if (!strcmp (argv[1],"save")) {
        if (command_verbosity_level > VERBOSITY_NONE) {
            if (board_eeprom_saveregisterspace())
                UARTprintf("error: EEPROM is corrupted.\n");
            else
                UARTprintf("register space has been saved to EEPROM.\n");
        }
    } else {
        UARTprintf("error: invalid input.\n");
        return (0);
    }

    return (0);       
}

// Command 'get'
static int command_getreg(int argc, char *argv[]) {
    uint8_t status;
    uint32_t num;
    char *endptr;

    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing register address.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);
    if (*endptr != 0 || errno != 0) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
        return (0);
    }       

    if ((num & 0xff) < 0x30) 
        status = cc1101_read_reg((uint8_t) num);
    else
        status = cc1101_read_status_reg((uint8_t) num);

    if (command_verbosity_level > VERBOSITY_ERROR) 
        UARTprintf("received: 0x%02x\n", status);
    return(0);
}

// Command 'ping'
static int command_ping(int argc, char *argv[]) {

    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing argument.\n");
        return (0);
    }
    
    if (!strcmp (argv[1], "on")) {
        tasktable[0].taskrepetition = -1;
        UARTprintf("ping is due in every 2 seconds.\n");
    } else if (!strcmp (argv[1], "off")) {
        tasktable[0].taskrepetition = 0;
        UARTprintf("ping is off.\n");
    } else {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
    }

    return(0);
}

// Command 'pow'
static int command_setpow(int argc, char *argv[]) {
    uint32_t num;
    char *endptr;

    if (argc < 2) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing power value.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);
    if (*endptr != 0 || errno != 0) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
        return (0);
    }       

    cc1101_state.output_power = (uint8_t) num;
    cc1101_update_state();
    return(0);
}

// Command 'rep'
static int command_report(int argc, char *argv[]) {
    
    UARTprintf("Radio state variables report\n");
    UARTprintf("----------------------------\n");
    UARTprintf("Device address:               %d\n", cc1101_state.device_address);
    UARTprintf("Destination address:          %d\n", cc1101_state.destination_address);
    UARTprintf("Output power (PATABLE):     0x%02x\n", cc1101_state.output_power);

    UARTprintf("Console verbosity level:      ");
    switch (command_verbosity_level) {
        case VERBOSITY_NONE:
            UARTprintf("none.\n");
            break;
        case VERBOSITY_ERROR:
            UARTprintf("error messages only.\n");
            break;
        case VERBOSITY_ALL:
            UARTprintf("all messages.\n");
            break;
        default:
            break;
    }
    UARTprintf("----------------------------\n");

    return(0);
}

// Command 'rst'
static int command_reset(int argc, char *argv[]) {

    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
    return (0);
}

static int command_send(int argc, char *argv[]) {

    // cc1101_sendpacket((uint8_t *)"PING", 5, 0);
    tasktable[0].taskrepetition = 5;
    if (command_verbosity_level > VERBOSITY_ERROR) 
        UARTprintf("PING packet sent.\n");
    return(0);
}

// Command 'set'
static int command_setreg(int argc, char *argv[]) {
    uint32_t num[2], i;
    char *endptr;

    if (argc < 3) {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing fields.\n");
        return (0);
    }
    for (i = 0; i < 2; i++) {

        errno = 0;
        num[i] = strtol(argv[i+1], &endptr, 16);
        if (*endptr != 0 || errno != 0) {
            if (command_verbosity_level > VERBOSITY_NONE)
                UARTprintf("error: invalid input.\n");
            return (0);
        }
    }       
    if ((num[0] & 0xff) < 0x30) 
        cc1101_write_reg((uint8_t) num[0], (uint8_t) num[1]);
    else {
        if (command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid value.\n");
    }

    if (command_verbosity_level > VERBOSITY_ERROR)
        UARTprintf("register overwritten.\n");
    return(0);
}

// Command 'snop'
static int command_snop(int argc, char *argv[]) {
    uint8_t status;    
    status = cc1101_cmd(CC1101_SNOP);

    if (command_verbosity_level > VERBOSITY_ERROR) 
        UARTprintf("received: 0x%02x\n", status);
    return(0);
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

    { "adr", command_setaddress,   "{dev/dst address(d)} Set device or destination address." }, 
    { "eep", command_eeprom,   "{load/save} Loads/saves register space to MCU EEPROM." }, 
    { "get", command_getreg,   "{reg address(h)} Gets normal or status register content." },
    { "help", command_help,   "Display list of commands." },
    { "ping", command_ping,   "{on/off} Sends periodic ping packet."},
    { "pow", command_setpow,   "{value(h)} Sets output power." },
    { "rep", command_report,   "Reports radio state variables." },
    { "rst", command_reset,   "Reset." },
    { "set", command_setreg,   "{reg address(h) value(h)} Gets normal or status register content." },
    { "send", command_send,   "Sends a ping packet."},
    { "snop", command_snop,   "Get status byte from CC1101." },
    { "verb", command_setverbosity,   "{none/error/all} Sets verbosity level." },
    { 0, 0, 0 } // Don't touch it, last entry must be a terminating NULL entry
};