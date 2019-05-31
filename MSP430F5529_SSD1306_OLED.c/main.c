//******************************************************************************
//  MSP430F5529 SSD1306 OLED Display
//
//  Description: This demo connects two MSP430's via the I2C bus. The master
//  transmits to the slave. This is the MASTER CODE. It cntinuously
//  transmits an array of data and demonstrates how to implement an I2C
//  master transmitter sending multiple bytes using the USCI_B0 TX interrupt.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
//
//
//                                /|\  /|\
//                MSP430F5529     10k  10k      SSD1306 OLED
//                   master        |    |         Display
//             -----------------   |    |   -----------------
//           -|XIN  P4.1/UCB0SDA|<-|----+->|SDA              |-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |                 |-
//            |     P4.2/UCB0SCL|<-+------>|SCL              |
//            |                 |          |                 |
//
//******************************************************************************

#include <msp430.h>
#include "ssd1306.h"
#include "i2c.h"
#include "clock.h"

extern unsigned char *PTxData;                  // Pointer to TX data, defined in i2c.h
extern unsigned char TXByteCtr;                 // number of bytes to transmit, defined in i2c.h

#define MAX_COUNT 4294967295UL

int main(void)
{

    WDTCTL = WDTPW + WDTHOLD;                   // Stop WDT
    clock_init();

    P1DIR |= BIT5;                              // P1.5 output
    P1OUT |= BIT5;                              // P1.5 high to turn on power to display

    i2c_init();                                 // initialize UCB1 I2C, port 4 pins 1, 2

    ssd1306_init();                             // initialize SSD1306 OLED
    ssd1306_clearDisplay();                     // clear garbage data

    ssd1306_printText(0,0, "I Like to Count!");
    ssd1306_printTextBlock(0,6, "This will take some time, please wait.");

    uint32_t val;
    uint32_t a;
    for (a=0; a<MAX_COUNT; a++){
        ssd1306_printUI32(0,2,a, HCENTERUL_ON);
        for (val=0; val<MAX_COUNT; val++) {
            ssd1306_printUI32(0,4, val, HCENTERUL_ON);
        }
    }
    __bis_SR_register(LPM0_bits + GIE);         // Enter LPM0, enable interrupts
    __no_operation();
}

//------------------------------------------------------------------------------
// The USCIAB1TX_ISR is structured such that it can be used to transmit any
// number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
// points to the next byte to transmit.
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB1IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10: break;                           // Vector 10: RXIFG
  case 12:                                  // Vector 12: TXIFG  
    if (TXByteCtr)                          // Check TX byte counter
    {
      UCB1TXBUF = *PTxData++;               // Load TX buffer
      TXByteCtr--;                          // Decrement TX byte counter
    }
    else
    {
      UCB1CTL1 |= UCTXSTP;                  // I2C stop condition
      UCB1IFG &= ~UCTXIFG;                  // Clear USCI_B1 TX int flag
      __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
    }  
  default: break;
  }
}
