#ifndef CC1101_H
#define CC1101_H

#define EDGE_SELECT_RISING 0
#define EDGE_SELECT_FALLING 1
#define EDGE_SELECT_BOTH 3

#define RADIO_STATE_IDLE 			0
#define RADIO_STATE_RXSYNCWORD	 	1
#define RADIO_STATE_RX	 			2
#define RADIO_STATE_RXFINISHED	 	3
#define RADIO_STATE_TXSYNCWORD	 	4
#define RADIO_STATE_TX	 			5
#define RADIO_STATE_TXFINISHED	 	6
#define RADIO_STATE_SLEEP			255
typedef uint8_t radio_state_t;

#define FIFO_STATE_TX_FULL_ASSERT				0
#define FIFO_STATE_TX_BELOW_DEASSERT			2

typedef uint8_t fifo_state_t;

typedef struct {
	void (*spi_high) (void);
	void (*spi_low) (void);
	void (*spi_toggle) (void);
	uint32_t (*spi_write) (uint8_t bytetosend);
	void (*board_delay_us) (uint32_t delay);
	void (*board_delay_ms) (uint32_t delay);
	void (*gdio0_edge_select) (uint32_t edge);
	void (*gdio2_edge_select) (uint32_t edge);
	void (*gdio0_int_clear) (void);
	void (*gdio2_int_clear) (void);
}cc1101_interface_t;

typedef struct {
	uint8_t appendstatus:1;
	uint8_t stayinrx:1;
	uint8_t crcenabled:1;
}cc1101_switch_t;

typedef struct {
	uint8_t device_address;
	uint8_t destination_address;
	uint8_t output_power;
	radio_state_t volatile radio_state;
	fifo_state_t volatile fifo_state;	
	cc1101_switch_t switches;
}cc1101_state_t;

typedef struct {
	uint8_t rcvbuf[255];
	uint8_t rcvpacketsize;
	uint8_t expectedpacketsize;		
	int8_t rcvrssidbm;
	uint8_t rcvlqi;
}cc1101_rxpacket_t;

typedef struct {
	uint8_t txbuf[255];
	uint8_t txpacketsize;		
}cc1101_txpacket_t;

void cc1101_write_reg(uint8_t reg, uint8_t val);
uint8_t cc1101_read_reg(uint8_t reg);
uint8_t cc1101_read_status_reg(uint8_t reg);
uint8_t cc1101_cmd(uint8_t cmd);

void cc1101_init(void);
void cc1101_update_state(void);
void cc1101_sendpacket(uint8_t *packet, uint8_t packetsize, uint8_t waitforclearchannel);
void cc1101_receivepacket(uint8_t *dstpacket, uint8_t *packetsize, int8_t *rssi_dBm, uint8_t *lqi);
void cc1101_GDIO0_ISR(void);
void cc1101_GDIO2_ISR(void);

// comm_recv_result_t cc1200_receivepacket(uint8_t *dstpacket, packetsize_t *packetsize, rssi_dBm_t *rssi_dBm, lqi_t *lqi);
// void cc1200_sendpacket(uint8_t *packet, packetsize_t packetsize, flag_t waitforclearchannel);
#endif
