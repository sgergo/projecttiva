#include <stdint.h>
#include "types.h"
#include <stdbool.h>
#include "board.h"
#include "board_eeprom.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/eeprom.h"
#include "driverlib/sysctl.h"

static int32_t board_eeprom_enable_eeprom_peripherial(void) {
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	board_delay_ms (100);

	return ROM_EEPROMInit();
}

static void board_eeprom_disable_eeprom_peripherial(void) {
	ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_EEPROM0);
}

int32_t board_eeprom_loadregisterspace(void) {
	// Use buf in n*16 bytes chunks
	uint32_t regs[48];
	uint32_t i;
	uint8_t *byteptr;

	// If EEPROM initialisation has failed return with error code
	if (board_eeprom_enable_eeprom_peripherial() != EEPROM_INIT_OK) return (-1);

	// Read register values into local buffer
	ROM_EEPROMRead(regs, 0x400, sizeof(regs));

	// Update register values
	byteptr = (uint8_t *) regs;
	for (i = 0; i < 48; i++)
		/* do_something(i, *byteptr++) */;

	// Disable the EEPROM peripherial to save power
	board_eeprom_disable_eeprom_peripherial();
	return 0;
}

int32_t board_eeprom_saveregisterspace(void) {
	uint32_t regs[48];
	uint32_t i;
	uint8_t *byteptr;

	// If EEPROM initialisation has failed return with error code
	if (board_eeprom_enable_eeprom_peripherial() != EEPROM_INIT_OK) return (-1);

	// Store registers in local buffer
	byteptr = (uint8_t *) regs;
	for (i = 0; i < 48; i++) 
		/* *byteptr++ = do_something(i)*/;

	// Store register space in EEPROM
	ROM_EEPROMProgram(regs, 0x400, sizeof(regs));

	// Disable the EEPROM peripherial to save power
	board_eeprom_disable_eeprom_peripherial();
	return (0);
}
