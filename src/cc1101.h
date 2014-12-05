#ifndef CC1101_H
#define CC1101_H

#define EDGE_SELECT_RISING 0
#define EDGE_SELECT_FALLING 1
#define EDGE_SELECT_BOTH 3

typedef struct {
	void (*spi_high) (void);
	void (*spi_low) (void);
	uint8_t (*spi_write) (uint8_t bytetosend);
	void (*board_delay_us) (uint32_t delay);
	void (*board_delay_ms) (uint32_t delay);
	void (*gdio0_edge_select) (uint32_t edge);
	void (*gdio2_edge_select) (uint32_t edge);
	void (*gdio0_int_clear) (void);
	void (*gdio2_int_clear) (void);
}cc1101_interface_t;

typedef struct {
	int lofasz:1;
}cc1101_switch_t;

typedef struct {
	uint8_t device_address;
	uint8_t destination_address;
	uint8_t output_power;
	
	cc1101_switch_t switches;
}cc1101_state_t;

void cc1101_write_reg(uint8_t reg, uint8_t val);
uint8_t cc1101_read_reg(uint8_t reg);
uint8_t cc1101_read_status_reg(uint8_t reg);
uint8_t cc1101_cmd(uint8_t cmd);

void cc1101_init(void);
void cc1101_update_state(void);

void cc1101_GDIO0_ISR(void);
void cc1101_GDIO2_ISR(void);

// comm_recv_result_t cc1200_receivepacket(uint8_t *dstpacket, packetsize_t *packetsize, rssi_dBm_t *rssi_dBm, lqi_t *lqi);
// void cc1200_sendpacket(uint8_t *packet, packetsize_t packetsize, flag_t waitforclearchannel);
#endif