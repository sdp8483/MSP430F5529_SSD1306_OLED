/*
 * i2c.h
 *
 *  Created on: May 23, 2019
 *      Author: samper
 */

#ifndef I2C_H_
#define I2C_H_

#include <msp430.h>

unsigned char *PTxData;                     // Pointer to TX data
unsigned char TXByteCtr;

void i2c_init(void); // Setup UCB1 for I2C
void i2c_write(unsigned char *, unsigned char); // write date to i2c bus

#endif /* I2C_H_ */
