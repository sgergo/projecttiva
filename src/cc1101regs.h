#ifndef CC1101_REGS_H_
#define CC1101_REGS_H_

// Configuration Registers
#define CC1101_IOCFG2           0x00        // GDO2 output pin configuration
#define CC1101_IOCFG1           0x01        // GDO1 output pin configuration
#define CC1101_IOCFG0           0x02        // GDO0 output pin configuration
#define CC1101_FIFOTHR          0x03        // RX FIFO and TX FIFO thresholds
#define CC1101_SYNC1            0x04        // Sync word, high byte
#define CC1101_SYNC0            0x05        // Sync word, low byte
#define CC1101_PKTLEN           0x06        // Packet length
#define CC1101_PKTCTRL1         0x07        // Packet automation control
#define CC1101_PKTCTRL0         0x08        // Packet automation control
#define CC1101_ADDR             0x09        // Device address
#define CC1101_CHANNR           0x0A        // Channel number
#define CC1101_FSCTRL1          0x0B        // Frequency synthesizer control
#define CC1101_FSCTRL0          0x0C        // Frequency synthesizer control
#define CC1101_FREQ2            0x0D        // Frequency control word, high byte
#define CC1101_FREQ1            0x0E        // Frequency control word, middle byte
#define CC1101_FREQ0            0x0F        // Frequency control word, low byte
#define CC1101_MDMCFG4          0x10        // Modem configuration
#define CC1101_MDMCFG3          0x11        // Modem configuration
#define CC1101_MDMCFG2          0x12        // Modem configuration
#define CC1101_MDMCFG1          0x13        // Modem configuration
#define CC1101_MDMCFG0          0x14        // Modem configuration
#define CC1101_DEVIATN          0x15        // Modem deviation setting
#define CC1101_MCSM2            0x16        // Main Radio Cntrl State Machine config
#define CC1101_MCSM1            0x17        // Main Radio Cntrl State Machine config
#define CC1101_MCSM0            0x18        // Main Radio Cntrl State Machine config
#define CC1101_FOCCFG           0x19        // Frequency Offset Compensation config
#define CC1101_BSCFG            0x1A        // Bit Synchronization configuration
#define CC1101_AGCCTRL2         0x1B        // AGC control
#define CC1101_AGCCTRL1         0x1C        // AGC control
#define CC1101_AGCCTRL0         0x1D        // AGC control
#define CC1101_WOREVT1          0x1E        // High byte Event 0 timeout
#define CC1101_WOREVT0          0x1F        // Low byte Event 0 timeout
#define CC1101_WORCTRL          0x20        // Wake On Radio control
#define CC1101_FREND1           0x21        // Front end RX configuration
#define CC1101_FREND0           0x22        // Front end TX configuration
#define CC1101_FSCAL3           0x23        // Frequency synthesizer calibration
#define CC1101_FSCAL2           0x24        // Frequency synthesizer calibration
#define CC1101_FSCAL1           0x25        // Frequency synthesizer calibration
#define CC1101_FSCAL0           0x26        // Frequency synthesizer calibration
#define CC1101_RCCTRL1          0x27        // RC oscillator configuration
#define CC1101_RCCTRL0          0x28        // RC oscillator configuration
#define CC1101_FSTEST           0x29        // Frequency synthesizer cal control
#define CC1101_PTEST            0x2A        // Production test
#define CC1101_AGCTEST          0x2B        // AGC test
#define CC1101_TEST2            0x2C        // Various test settings
#define CC1101_TEST1            0x2D        // Various test settings
#define CC1101_TEST0            0x2E        // Various test settings
// Status registers
#define CC1101_PARTNUM          0x30        // Part number
#define CC1101_VERSION          0x31        // Current version number
#define CC1101_FREQEST          0x32        // Frequency offset estimate
#define CC1101_LQI              0x33        // Demodulator estimate for link quality
#define CC1101_RSSI             0x34        // Received signal strength indication
#define CC1101_MARCSTATE        0x35        // Control state machine state
#define CC1101_WORTIME1         0x36        // High byte of WOR timer
#define CC1101_WORTIME0         0x37        // Low byte of WOR timer
#define CC1101_PKTSTATUS        0x38        // Current GDOx status and packet status
#define CC1101_VCO_VC_DAC       0x39        // Current setting from PLL cal module
#define CC1101_TXBYTES          0x3A        // Underflow and # of bytes in TXFIFO
#define CC1101_RXBYTES          0x3B        // Overflow and # of bytes in RXFIFO
#define CC1101_RCCTRL1_STATUS   0x3C        // Last RC oscillator calibration result
#define CC1101_RCCTRL0_STATUS   0x3D        // Last RC oscillator calibration result
// Multi byte memory locations
#define CC1101_PATABLE          0x3E
#define CC1101_TXFIFO           0x3F
#define CC1101_RXFIFO           0x3F

// Definitions for burst/single access to registers
#define CC1101_WRITE_BURST      0x40
#define CC1101_READ_SINGLE      0x80
#define CC1101_READ_BURST       0xC0

// Strobe commands
#define CC1101_SRES             0x30        // Reset chip.
#define CC1101_SFSTXON          0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
// If in RX/TX: Go to a wait state where only the synthesizer is
// running (for quick RX / TX turnaround).
#define CC1101_SXOFF            0x32        // Turn off crystal oscillator.
#define CC1101_SCAL             0x33        // Calibrate frequency synthesizer and turn it off
// (enables quick start).
#define CC1101_SRX              0x34        // Enable RX. Perform calibration first if coming from IDLE and
// MCSM0.FS_AUTOCAL=1.
#define CC1101_STX              0x35        // In IDLE state: Enable TX. Perform calibration first if
// MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
// Only go to TX if channel is clear.
#define CC1101_SIDLE            0x36        // Exit RX / TX, turn off frequency synthesizer and exit
// Wake-On-Radio mode if applicable.
#define CC1101_SAFC             0x37        // Perform AFC adjustment of the frequency synthesizer
#define CC1101_SWOR             0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CC1101_SPWD             0x39        // Enter power down mode when CSn goes high.
#define CC1101_SFRX             0x3A        // Flush the RX FIFO buffer.
#define CC1101_SFTX             0x3B        // Flush the TX FIFO buffer.
#define CC1101_SWORRST          0x3C        // Reset real time clock.
#define CC1101_SNOP             0x3D        // No operation. May be used to pad strobe commands to two
// bytes for simpler software.


//----------------------------------------------------------------------------------
// Chip Status Byte
//----------------------------------------------------------------------------------

// Bit fields in the chip status byte
#define CC1101_STATUS_CHIP_RDYn_BM             0x80
#define CC1101_STATUS_STATE_BM                 0x70
#define CC1101_STATUS_FIFO_BYTES_AVAILABLE_BM  0x0F

// Chip states
#define CC1101_STATE_IDLE                      0x00
#define CC1101_STATE_RX                        0x10
#define CC1101_STATE_TX                        0x20
#define CC1101_STATE_FSTXON                    0x30
#define CC1101_STATE_CALIBRATE                 0x40
#define CC1101_STATE_SETTLING                  0x50
#define CC1101_STATE_RX_OVERFLOW               0x60
#define CC1101_STATE_TX_UNDERFLOW              0x70

//----------------------------------------------------------------------------------
// Other register bit fields
//----------------------------------------------------------------------------------
#define CC1101_LQI_CRC_OK_BM                   0x80
#define CC1101_LQI_EST_BM                      0x7F

#ifdef POWERSAVINGCOMM
#define cc1101_init_regs() \
	cc1101_write_reg(CC1101_IOCFG2, 0x00); /* GDO2 - threshold interrupt - see cc1101.pdf page 62 */ \
	cc1101_write_reg(CC1101_IOCFG1, 0x2e); \
	cc1101_write_reg(CC1101_IOCFG0, 0x06); /* GDO0 - packet interrupt */ \
	cc1101_write_reg(CC1101_FIFOTHR, 7); /* See cc1101.pdf page 56 */ \
	cc1101_write_reg(CC1101_SYNC1, 0xd3); \
	cc1101_write_reg(CC1101_SYNC0, 0x91); \
	cc1101_write_reg(CC1101_PKTLEN, sizeof(lowerlayer_signal_packet_t)); /* We are expecting an RTS packet first */ \
	cc1101_write_reg(CC1101_PKTCTRL1, 0x06); /* 0x06 - default, 0x04 - no address matching */ \
	cc1101_write_reg(CC1101_PKTCTRL0, 0x44); /* 0x44 - fixed packet length, 0x45 - variable pl, 0x46 - infinite pl */ \
	cc1101_write_reg(CC1101_ADDR, 0xff); \
	cc1101_write_reg(CC1101_CHANNR, comm_basechannel*2); \
	cc1101_write_reg(CC1101_FSCTRL1, 0x06); \
	cc1101_write_reg(CC1101_FSCTRL0, 0x00); \
	cc1101_write_reg(CC1101_FREQ2, 0x10); \
	cc1101_write_reg(CC1101_FREQ1, 0xa8); /* 2.4k: 0xb1 10k: a8 */ \
	cc1101_write_reg(CC1101_FREQ0, 0x5f); /* 0x5f for ch0: 433.1MHz, wake ch (ch1): 433.19MHz */ \
	cc1101_write_reg(CC1101_MDMCFG4, 0xd8); /* 2.4k: 0xf6 10k: 0xd8*/ \
	cc1101_write_reg(CC1101_MDMCFG3, 0x93); /* 2.4k: 0x83 10k: 0x93 */\
	cc1101_write_reg(CC1101_MDMCFG2, 0x13); /* 2.4k: 0x13 10k: 0x13 */\
	cc1101_write_reg(CC1101_MDMCFG1, 0xc1); /* 75k: 0x21 2.4k: 0x22 FEC_2.4k: 0xc1 10k_FEC: 0xc1 */ \
	cc1101_write_reg(CC1101_MDMCFG0, 0xf8); /* 75k: 0x7a 2.4k: 0xf8 10k: 0xf8 */ \
	cc1101_write_reg(CC1101_DEVIATN, 0x34); /* 2.4k: 0x15 10k: 0x34*/ \
	cc1101_write_reg(CC1101_MCSM2, 0x1); /* DIFF: Only check for a sync word for a short time (this is the max, at 0x2 it's not always waking up) */ \
	cc1101_write_reg(CC1101_MCSM1, 0x30); \
	cc1101_write_reg(CC1101_MCSM0, 0x04); /* No auto calibration */ \
	cc1101_write_reg(CC1101_FOCCFG, 0x16); \
	cc1101_write_reg(CC1101_BSCFG, 0x6c); \
	cc1101_write_reg(CC1101_AGCCTRL2, 0x47); /* This is important for RSSI sensing, 2.4k: 0x47 10k: 0x47*/ \
	cc1101_write_reg(CC1101_AGCCTRL1, 0x40); /* 2.4k: 0x40 10k: 0x40 */\
	cc1101_write_reg(CC1101_AGCCTRL0, 0x91); /* 2.4k: 0x91 10k: 0x91 */ \
	cc1101_write_reg(CC1101_WOREVT1, 0x07); /* DIFF: Wake up every 3.6s: cc1101.pdf p87 (750/26000000)*3900*(2^5)=3.6 -> 3900/2 */ \
	cc1101_write_reg(CC1101_WOREVT0, 0x3a); \
	cc1101_write_reg(CC1101_WORCTRL, 0xf9); \
	cc1101_write_reg(CC1101_FREND1, 0x56); \
	cc1101_write_reg(CC1101_FREND0, 0x10); \
	cc1101_write_reg(CC1101_FSCAL3, 0xe9); \
	cc1101_write_reg(CC1101_FSCAL2, 0x2a); \
	cc1101_write_reg(CC1101_FSCAL1, 0x00); \
	cc1101_write_reg(CC1101_FSCAL0, 0x1f); \
	cc1101_write_reg(CC1101_RCCTRL1, 0x41); \
	cc1101_write_reg(CC1101_RCCTRL0, 0x00);
#else /* ifdef POWERSAVINGCOMM */
#define cc1101_init_regs() \
	cc1101_write_reg(CC1101_IOCFG2, 0x00); /* GDO2 - threshold interrupt - see cc1101.pdf page 62 */ \
	cc1101_write_reg(CC1101_IOCFG1, 0x2e); \
	cc1101_write_reg(CC1101_IOCFG0, 0x06); /* GDO0 - packet interrupt */ \
	cc1101_write_reg(CC1101_FIFOTHR, 7); /* See cc1101.pdf page 56 */ \
	cc1101_write_reg(CC1101_SYNC1, 0xd3); \
	cc1101_write_reg(CC1101_SYNC0, 0x91); \
	cc1101_write_reg(CC1101_PKTLEN, sizeof(lowerlayer_signal_packet_t)); /* We are expecting an RTS packet first */ \
	cc1101_write_reg(CC1101_PKTCTRL1, 0x06); /* 0x06 - default, 0x04 - no address matching */ \
	cc1101_write_reg(CC1101_PKTCTRL0, 0x44); /* 0x44 - fixed packet length, 0x45 variable pl, 0x46 - infinite pl */ \
	cc1101_write_reg(CC1101_ADDR, 0xff); \
	cc1101_write_reg(CC1101_CHANNR, comm_basechannel*2); \
	cc1101_write_reg(CC1101_FSCTRL1, 0x06); \
	cc1101_write_reg(CC1101_FSCTRL0, 0x00); \
	cc1101_write_reg(CC1101_FREQ2, 0x10); /* 433mhz: 0x10 868mhz: 0x21 */ \
	cc1101_write_reg(CC1101_FREQ1, 0xa8); /* 433mhz: 0xa8 868mhz: 0x62 2.4k: 0xb1 10k: a8 */\
	cc1101_write_reg(CC1101_FREQ0, 0x5f); /* 433mhz: 0x5f 868mhz: 0x76 0x3b for 433.1 MHz */ \
	cc1101_write_reg(CC1101_MDMCFG4, 0xd8); /* 2.4k: 0xf6 10k: 0xd8*/ \
	cc1101_write_reg(CC1101_MDMCFG3, 0x93); /* 2.4k: 0x83 10k: 0x93 */\
	cc1101_write_reg(CC1101_MDMCFG2, 0x13); /* 2.4k: 0x13 10k: 0x13 */\
	cc1101_write_reg(CC1101_MDMCFG1, 0xc1); /* 75k: 0x21 2.4k: 0x22 FEC_2.4k: 0xc1 10k_FEC: 0xc1 */ \
	cc1101_write_reg(CC1101_MDMCFG0, 0xf8); /* 75k: 0x7a 2.4k: 0xf8 10k: 0xf8 */ \
	cc1101_write_reg(CC1101_DEVIATN, 0x34); /* 2.4k: 0x15 10k: 0x34*/ \
	cc1101_write_reg(CC1101_MCSM2, 0x0); /* Only check for a sync word for a short time */ \
	cc1101_write_reg(CC1101_MCSM2, 0x07); /* DIFF: Continuous RX needed if power saving is disabled */ \
	cc1101_write_reg(CC1101_MCSM1, 0x30); \
	cc1101_write_reg(CC1101_MCSM0, 0x04); /* No auto calibration */ \
	cc1101_write_reg(CC1101_FOCCFG, 0x16); \
	cc1101_write_reg(CC1101_BSCFG, 0x6c); \
	cc1101_write_reg(CC1101_AGCCTRL2, 0x47); /* 433mhz: 0x47 868mhz: 0x43 This is important for RSSI sensing, 2.4k: 0x47 10k: 0x47*/ \
	cc1101_write_reg(CC1101_AGCCTRL1, 0x40); /* 2.4k: 0x40 10k: 0x40 */\
	cc1101_write_reg(CC1101_AGCCTRL0, 0x91); /* 2.4k: 0x91 10k: 0x91 */ \
	cc1101_write_reg(CC1101_WOREVT1, 0xff); \
	cc1101_write_reg(CC1101_WOREVT0, 0xff); \
	cc1101_write_reg(CC1101_WORCTRL, 0xf9); \
	cc1101_write_reg(CC1101_FREND1, 0x56); \
	cc1101_write_reg(CC1101_FREND0, 0x10); \
	cc1101_write_reg(CC1101_FSCAL3, 0xe9); \
	cc1101_write_reg(CC1101_FSCAL2, 0x2a); \
	cc1101_write_reg(CC1101_FSCAL1, 0x00); \
	cc1101_write_reg(CC1101_FSCAL0, 0x1f); \
	cc1101_write_reg(CC1101_RCCTRL1, 0x41); \
	cc1101_write_reg(CC1101_RCCTRL0, 0x00);
#endif /* ifdef POWERSAVINGCOMM */

#endif