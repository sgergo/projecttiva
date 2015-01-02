#ifndef BOARDCONFIG_H
#define BOARDCONFIG_H

#define CONSOLEUARTPERIPHERIAL				SYSCTL_PERIPH_UART0
#define CONSOLEUARTPINPERIPHERIAL			SYSCTL_PERIPH_GPIOA
#define CONSOLEUARTPERIPHERIALBASE			UART0_BASE
#define CONSOLEUARTPINPERIPHERIALBASE		GPIO_PORTA_BASE
#define CONSOLEUARTRXPINTYPE				GPIO_PA0_U0RX
#define CONSOLEUARTTXPINTYPE				GPIO_PA1_U0TX
#define CONSOLEUARTRXPIN					GPIO_PIN_0
#define CONSOLEUARTTXPIN 					GPIO_PIN_1

#define SPIPERIPHERIAL						SYSCTL_PERIPH_SSI2
#define SPIPINPERIPHERIAL					SYSCTL_PERIPH_GPIOB
#define SPIPERIPHERIALBASE					SSI2_BASE
#define SPIPINPERIPHERIALBASE				GPIO_PORTB_BASE
#define SPISCLKPINTYPE						GPIO_PB4_SSI2CLK
#define SPISFSSPINTYPE						GPIO_PB5_SSI2FSS
#define SPISOMIPINTYPE						GPIO_PB6_SSI2RX
#define SPISIMOPINTYPE						GPIO_PB7_SSI2TX
#define SPISCLKPIN							GPIO_PIN_4
#define SPISFSSPIN							GPIO_PIN_5
#define SPISOMIPIN							GPIO_PIN_6
#define SPISIMOPIN							GPIO_PIN_7

#define SPICSPINPERIPHERIAL					SYSCTL_PERIPH_GPIOE
#define SPICSPINPERIPHERIALBASE				GPIO_PORTE_BASE
#define SPICSPIN							GPIO_PIN_0

#define LED_1_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_2_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_3_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_ALL_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_1_PORTBASE						GPIO_PORTF_BASE
#define LED_2_PORTBASE						GPIO_PORTF_BASE
#define LED_3_PORTBASE						GPIO_PORTF_BASE
#define LED_ALL_PORTBASE					GPIO_PORTF_BASE
#define LED_RED_PORTBASE					LED_1_PORTBASE
#define LED_BLUE_PORTBASE					LED_2_PORTBASE
#define LED_GREEN_PORTBASE					LED_3_PORTBASE
#define LED_1_PIN 							GPIO_PIN_1
#define LED_2_PIN 							GPIO_PIN_2
#define LED_3_PIN 							GPIO_PIN_3
#define LED_RED								LED_1_PIN
#define LED_BLUE 							LED_2_PIN
#define LED_GREEN 							LED_3_PIN

#define WATCHDOGBASE						WATCHDOG0_BASE
#define WATCHDOGPERIPHERIAL					SYSCTL_PERIPH_WDOG0
#define WATCHDOGINT							INT_WATCHDOG
#define WATCHDOGPINPERIPHERIAL				SYSCTL_PERIPH_GPIOB
#define WATCHDOGPINPERIPHERIALBASE			GPIO_PORTB_BASE
#define WATCHDOGPIN							GPIO_PIN_2

#define GPSUARTPERIPHERIAL					SYSCTL_PERIPH_UART1
#define GPSUARTPINPERIPHERIAL				SYSCTL_PERIPH_GPIOB
#define GPSUARTPERIPHERIALBASE				UART1_BASE
#define GPSUARTPINPERIPHERIALBASE			GPIO_PORTB_BASE
#define GPSUARTRXPINTYPE					GPIO_PB0_U1RX
#define GPSUARTTXPINTYPE					GPIO_PB1_U1TX
#define GPSUARTRXPIN						GPIO_PIN_0
#define GPSUARTTXPIN 						GPIO_PIN_1

#define GPSPPSOUTPINPERIPHERIAL				SYSCTL_PERIPH_GPIOB
#define GPSPPSOUTPINPERIPHERIALBASE			GPIO_PORTB_BASE
#define GPSPPSOUTPIN 						GPIO_PIN_3
#define GPSPPSOUTINTPIN						GPIO_INT_PIN_3
#define GPSPPSOUTINT 						INT_GPIOB

#define GPSPUSHTOFIXPINPERIPHERIAL			SYSCTL_PERIPH_GPIOA
#define GPSPUSHTOFIXPINPERIPHERIALBASE		GPIO_PORTA_BASE
#define GPSPUSHTOFIXPIN 					GPIO_PIN_3

#define GPSFIXAVAILABLEPINPERIPHERIAL		SYSCTL_PERIPH_GPIOE
#define GPSFIXAVAILABLEPINPERIPHERIALBASE	GPIO_PORTE_BASE
#define GPSFIXAVAILABLEPIN 					GPIO_PIN_3
#define GPSFIXAVAILABLEINTPIN				GPIO_INT_PIN_3
#define GPSFIXAVAILABLEINT 					INT_GPIOE

#define GPSRESETPINPERIPHERIAL				SYSCTL_PERIPH_GPIOC
#define GPSRESETPINPERIPHERIALBASE			GPIO_PORTC_BASE
#define GPSRESETPIN 						GPIO_PIN_6
#endif
