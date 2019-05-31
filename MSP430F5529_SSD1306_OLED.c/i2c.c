/*
 * i2c.c
 *
 *  Created on: May 23, 2019
 *      Author: samper
 */

#include "i2c.h"

#include <msp430.h>
#include <stdint.h>

#define SDA BIT1                                // i2c SDA pin on port 4
#define SCL BIT2                                // i2c SCL pin on port 4

void i2c_init(void) {
    P4SEL |= SDA | SCL;                         // Assign I2C pins to USCI_B1
    UCB1CTL1 |= UCSWRST;                        // Enable SW reset
    UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
    UCB1CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK=24MHz, keep SW reset
    UCB1BR0 = 64;                               // fSCL = SMCLK/64 = ~400kHz
    UCB1BR1 = 0;                                // UCBRx = (UCxxBR0 + UCxxBR1 * 256) -> fSCL = SMCLK/USBRx
    UCB1I2CSA = 0x3C;                           // Slave Address is 0x3C
    UCB1CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
    UCB1IE |= UCTXIE;                           // Enable TX interrupt
} // end i2c_init

void i2c_write(unsigned char *DataBuffer, unsigned char ByteCtr) {
    //__delay_cycles(10);                         // small wait
    PTxData = DataBuffer;                       // TX array start address
                                                // Place breakpoint here to see each
                                                // transmit operation.
    TXByteCtr = ByteCtr;                        // Load TX byte counter

    UCB1CTL1 |= UCTR + UCTXSTT;                 // I2C TX, start condition

    __bis_SR_register(LPM0_bits + GIE);         // Enter LPM0, enable interrupts
    __no_operation();                           // Remain in LPM0 until all data
                                                // is TX'd
    while (UCB1CTL1 & UCTXSTP);                 // Ensure stop condition got sent
} // end i2c_write
