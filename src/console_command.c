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
#include "console_command.h"
#include "cmdline.h"
#include "uartstdio.h"
#include "board_spi.h"
#include "board_eeprom.h"
#include "cc1101.h"
#include "cc1101regs.h"

extern cc1101_state_t volatile cc1101_state;

uint8_t volatile console_command_verbosity_level;

// Command 'help'
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
        ROM_SysCtlDelay(100000); // TODO: This is needed otherwise minicom outputs damaged text
    }
    UARTprintf("------------------\n");
    UARTprintf("Useful register addresses: MARCSTATE: 0x35, PKTLEN: 0x06, ADDR: 0x09\n");
    UARTprintf("------------------\n");

    UARTprintf("\n");
    return(0);
}

// Command 'adr'
static int console_command_setaddress(int argc, char *argv[]) {
    uint32_t num;
    char *endptr;

    if (argc < 3) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing fields.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[2], &endptr, 10);
    if (*endptr != 0 || errno != 0) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
        return (0);
    }       

    if (num > 255) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid value.\n");
        return (0);
    } else {
        if (!strcmp (argv[1], "dev")) {    
                cc1101_write_reg(CC1101_ADDR, (uint8_t) num);
                cc1101_state.device_address = (uint8_t) num;
        
        } else if (!strcmp (argv[1],"dst")) {
            cc1101_state.destination_address = 0;
        } else {
            if (console_command_verbosity_level > VERBOSITY_NONE)
                UARTprintf("error: invalid input.\n");
            return (0);
        }

        if (console_command_verbosity_level > VERBOSITY_ERROR) 
            UARTprintf("%s address updated.\n", argv[1]);  
    }
    return (0);       
}

// Command 'eep'
static int console_command_eeprom(int argc, char *argv[]) {
    if (argc < 2) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing field.\n");
        return (0);
    }

    if (!strcmp (argv[1], "load")) {
        if (board_eeprom_loadregisterspace() && console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: EEPROM is corrupted.\n");
        else
            UARTprintf("register space is updated from EEPROM.\n");
    } else if (!strcmp (argv[1],"save")) {
        if (console_command_verbosity_level > VERBOSITY_NONE) {
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
static int console_command_getreg(int argc, char *argv[]) {
    uint8_t status;
    uint32_t num;
    char *endptr;

    if (argc < 2) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing register address.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);
    if (*endptr != 0 || errno != 0) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
        return (0);
    }       

    if ((num & 0xff) < 0x30) 
        status = cc1101_read_reg((uint8_t) num);
    else
        status = cc1101_read_status_reg((uint8_t) num);

    if (console_command_verbosity_level > VERBOSITY_ERROR) 
        UARTprintf("received: 0x%02x\n", status);
    return(0);
}

// Command 'pow'
static int console_command_setpow(int argc, char *argv[]) {
    uint32_t num;
    char *endptr;

    if (argc < 2) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing power value.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);
    if (*endptr != 0 || errno != 0) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid input.\n");
        return (0);
    }       

    cc1101_state.output_power = (uint8_t) num;
    cc1101_update_state();
    return(0);
}

// Command 'rep'
static int console_command_report(int argc, char *argv[]) {
    
    UARTprintf("Radio state variables report\n");
    UARTprintf("----------------------------\n");
    UARTprintf("Device address:               %d\n", cc1101_state.device_address);
    UARTprintf("Destination address:          %d\n", cc1101_state.destination_address);
    UARTprintf("Output power (PATABLE):     0x%02x\n", cc1101_state.output_power);

    UARTprintf("Console verbosity level:      ");
    switch (console_command_verbosity_level) {
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
static int console_command_reset(int argc, char *argv[]) {

    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
    return (0);
}

// Command 'set'
static int console_command_setreg(int argc, char *argv[]) {
    uint32_t num[2], i;
    char *endptr;

    if (argc < 3) {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: missing fields.\n");
        return (0);
    }
    for (i = 0; i < 2; i++) {

        errno = 0;
        num[i] = strtol(argv[i+1], &endptr, 16);
        if (*endptr != 0 || errno != 0) {
            if (console_command_verbosity_level > VERBOSITY_NONE)
                UARTprintf("error: invalid input.\n");
            return (0);
        }
    }       
    if ((num[0] & 0xff) < 0x30) 
        cc1101_write_reg((uint8_t) num[0], (uint8_t) num[1]);
    else {
        if (console_command_verbosity_level > VERBOSITY_NONE)
            UARTprintf("error: invalid value.\n");
    }

    if (console_command_verbosity_level > VERBOSITY_ERROR)
        UARTprintf("register overwritten.\n");
    return(0);
}

// Command 'snop'
static int console_command_snop(int argc, char *argv[]) {
    uint8_t status;
    
    status = cc1101_cmd(CC1101_SNOP);
    // status = board_spi_sendbyte(CC1101_SNOP);
    //board_spi_cspin_toggle();
    if (console_command_verbosity_level > VERBOSITY_ERROR) 
        UARTprintf("received: 0x%02x\n", status);
    return(0);
}

// Command 'verb'
static int console_command_setverbosity(int argc, char *argv[]) {
    if (argc < 2) {
        UARTprintf("verbosity level: %d\n", console_command_verbosity_level);
        return (0);
    }

    if (!strcmp (argv[1], "none")) {
        console_command_verbosity_level = VERBOSITY_NONE;
    } else if (!strcmp (argv[1],"error")) {
        console_command_verbosity_level = VERBOSITY_ERROR;
    } else if (!strcmp (argv[1],"all")) {
        console_command_verbosity_level = VERBOSITY_ALL;
    } else {
        UARTprintf("error: invalid input.\n");
        return (0);
    }

    return (0);       
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

    { "adr", console_command_setaddress,   "{dev/dst address(d)} Set device or destination address." }, 
    { "eep", console_command_eeprom,   "{load/save} Loads/saves register space to MCU EEPROM." }, 
    { "get", console_command_getreg,   "{reg address(h)} Gets normal or status register content." },
    { "help", console_command_help,   "Display list of commands." },
    { "pow", console_command_setpow,   "{value(h)} Sets output power." },
    { "rep", console_command_report,   "Reports radio state variables." },
    { "rst", console_command_reset,   "Reset." },
    { "set", console_command_setreg,   "{reg address(h) value(h)} Gets normal or status register content." },
    { "snop", console_command_snop,   "Get status byte from CC1101." },
    { "verb", console_command_setverbosity,   "{none/error/all} Sets verbosity levels." },
    { 0, 0, 0 } // Don't touch it, last entry must be a terminating NULL entry
};