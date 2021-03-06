#include <msp430.h>
#include "spi.h"

void spi_init(void)
{
	/*
	 * Initialize pin configurations
	 *
	 * See p.76 of device-specific datasheet
	 */
	P1SEL1 |= MOSI | MISO;				//pins P1.6 and P1.7 are MOSI and MISO respectively
	P1SEL0 &= ~(MOSI | MISO);

	P1SEL1 |= CSN;						//pin P1.3 is STE
	P1SEL0 &= ~CSN;
//	P1DIR |= CSN;
//	P1OUT |= CSN;

	P2SEL1 |= CLK;						//pin P2.2 is CLK
	P2SEL0 &= ~CLK;


	/*
	 * Initialize the SPI module.
	 *
	 * 1. Enable software reset.
	 * 2. Configure SPI
	 * 3. Clear sw reset, resume operation
	 * 4. Enable RX interrupt
	 */

	UCB0CTLW0 |= UCSWRST;							//software reset

	UCB0CTLW0 |= UCMSB | UCMST | UCSYNC;			//Inactive low, synchronous, master, msb first
	UCB0CTLW0 |= UCSSEL_2 | UCMODE_2 | UCSTEM;		//use SMCLK, 4 pin mode with active low on CSN (STE)
	UCB0BR0 |= 0x02;
	UCB0BR1 = 0;									//configure clock freq
	UCB0STATW |= UCLISTEN;							//enable loopback mode

	UCB0CTLW0 &= ~UCSWRST;							//disable software reset, enable operation

	UCB0IE |= UCRXIE;								//enable rx interrupt
}
