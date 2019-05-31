/*
 * clock.h
 *
 *  Created on: May 29, 2019
 *      Author: samper
 */

#include <msp430.h>

#ifndef CLOCK_H_
#define CLOCK_H_

void clock_init(void);
void SetVcoreUp (unsigned int level);

#endif /* CLOCK_H_ */
