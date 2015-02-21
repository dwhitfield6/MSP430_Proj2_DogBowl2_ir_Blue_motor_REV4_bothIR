#include "msp430.h"

#define motorF BIT2
#define motorR BIT5
#define LED0 BIT3
#define LED1 BIT4
#define IRLED1 BIT5
#define IRLED2 BIT6
#define IRrec BIT1
#define IRrec1 BIT0
#define debug BIT4
// Hardware-related definitions
#define UART_TXD 0x02                                  // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD 0x04                                  // RXD on P1.2 (Timer0_A.CCI1A)

#define UART_TBIT_DIV_2 (1000000 / (9600 * 2))         // Conditions for 9600 Baud SW UART, SMCLK = 1MHz
#define UART_TBIT (1000000 / 9600)
// Globals for full-duplex UART communication
unsigned int txData;                                   // UART internal variable for TX
unsigned char rxBuffer;                                // Received UART character
unsigned int BlueData =0;

int i = 0;
int ii = 0;
int iii=0;

int count=0;
unsigned long irsendcount =5000;
int loopcount =0;
int resetcount=0;
int bit =0;
int irbit =0;
int checksum =0;
int one =0;
int two =0;
int three =0;
int four =0;
int five =0;
int six =0;
int seven =0;
int eight =0;
int nine =0;
int ten =0;
int led1count =0;
int led0count =0;
int lid =0;
int Bluecount =990;
int Bluereal =0;
int manual =0;
int txcount =0;
int stage =1;
unsigned long lidcount =9990;
unsigned long lidopen =9990;
unsigned long lidclosed =9990;

void tx_send(int irdata);

static volatile unsigned long data = 0;
static volatile unsigned long realdata = 0;
static volatile unsigned long nowdata = 0;
static volatile unsigned long irdata = 0;


void TimerA_UART_tx(unsigned char byte);               // Function prototypes
void TimerA_UART_print(char *string);

void main(void){

  WDTCTL = WDTPW + WDTHOLD;                            // Stop watchdog timer
  // Initialize all GPIO
  P1SEL = UART_TXD + UART_RXD;
  P1DIR |= (UART_TXD + IRLED2 + motorR + debug);                            // Set all pins but RXD to output
  P2DIR |= (motorF + LED0 + LED1 +IRLED1);
  P1OUT =0;
  P2OUT=0;
  P2IE |= IRrec; // P1.3 interrupt enabled
  P2IES &= ~IRrec; // P1.3 interrupt enabled
  P2IFG &= ~IRrec; // P1.3 IFG cleared
  P2IE |= IRrec1; // P1.3 interrupt enabled
  P2IES |= IRrec1; // P1.3 interrupt enabled
  P2IFG &= ~IRrec1; // P1.3 IFG cleared

  TA1CCTL0 |= CCIE;                             // CCR0 interrupt enabled
  TA1CTL = TASSEL_2 + MC_1 + ID_2;           // SMCLK/8, upmode
  TA1CCR0 =  30;


  TA0CCTL0 = OUT;                                      // Set TXD Idle as Mark = '1'
  TA0CCTL1 = SCS + CM1 + CAP + CCIE;                   // Sync, Neg Edge, Capture, Int
  TA0CTL = TASSEL_2 + MC_2;                            // SMCLK, start in continuous mode


  __enable_interrupt(); // enable all interrupts                                   // Enable CPU interrupts

  TimerA_UART_print("G2553 TimerA UART\r\n");          // Send test message
  TimerA_UART_print("READY.\r\n");

  while(1)
  {
    // Wait for incoming character

    // Enter low poser mode

    _delay_cycles(1000);
    Bluecount++;
    if(Bluecount == 70)
    {
      Bluereal = BlueData;
      BlueData =0;
    }
    if(Bluecount >1000)
    {
      Bluecount =990;
    }

    if(Bluereal == 65)
    {
      manual = 0;
    }
    else if (Bluereal == 66)
    {
      manual = 1;
      lid = 0;
    }
    else if (Bluereal == 67)
    {
      manual = 1;
      lid =1;
    }

    if(manual ==0)
    {
      if(stage == 1)
      {
        P2OUT |= (LED0);
        led0count=0;
        tx_send(1);//most is 15
        stage =2;
        irsendcount++;
      }
      else if(stage == 2)
      {
        if(realdata == 2)
        {
          P2OUT |= (LED1);
          realdata = 0;
          led1count=0;
          lid = 1;
          lidcount  =0;
          stage =3;
          irsendcount =0;
        }
        if(realdata == 4)
        {
          realdata = 0;
          stage =1;
          _delay_cycles(1000);
          irsendcount =0;
        }
        irsendcount++;

      }
      else if(stage ==3)
      {
        stage =4;
        _delay_cycles(1000);
      }
      else if(stage ==4)
      {
        if(realdata == 4)
        {
          _delay_cycles(1000);
          realdata = 0;
          stage =1;
          irsendcount =0;
        }
        irsendcount++;
      }

      resetcount++;
      if(resetcount == 300)
      {
        loopcount =0;
        checksum =0;
      }
      if(resetcount>1000)
      {
        resetcount = 990;
      }

      if(checksum == 2)
      {
        realdata = data;
        nowdata = data;
        checksum =0;
        //P2OUT |= (LED1); // set P1.0 to 0 (LED OFF)
        loopcount =0;
        //led1count=0;
      }

    }

    if(irsendcount > 1000)
    {
      stage =1;
      irsendcount =0;
    }

    lidcount++;
    if (lidcount == 2200)
    {
      lid =0;
    }

    if(lid == 1)
    {
      lidopen++;
      if(lidopen <3500)
      {
        P2OUT |= (motorF);
        P1OUT &= ~(motorR);
      }
      else
      {
        P2OUT &= ~(motorF);
        P1OUT &= ~(motorR);
      }
    }
    else
    {
      lidopen=0;
    }
    if(lid ==0)
    {
      lidclosed++;
      if(lidclosed <3500)
      {
        P1OUT |= (motorR);
        P2OUT &= ~(motorF);
      }
      else
      {
        P1OUT &= ~(motorR);
        P2OUT &= ~(motorF);
      }
    }
    else
    {
      lidclosed =0;
    }
    if(lidclosed >8999)
    {
      lidclosed =8990;
    }
    if(lidopen >8999)
    {
      lidopen =8990;
    }

    if(lidcount >10000)
    {
      lidcount =9999;
    }

    if(led0count == 580)
    {
      P2OUT &= ~(LED0); // set P1.0 to 0 (LED OFF)
    }

    led0count++;
    if(led0count > 1000)
    {
      led0count =990;
    }

    if(led1count == 580)
    {
      P2OUT &= ~(LED1); // set P1.0 to 0 (LED OFF)
    }

    led1count++;
    if(led1count > 1000)
    {
      led1count =990;
    }

  }

}



void TimerA_UART_tx(unsigned char byte) {              // Outputs one byte using the Timer_A UART

  while (TACCTL0 & CCIE);                              // Ensure last char got TX'd

  TA0CCR0 = TAR;                                       // Current state of TA counter

  TA0CCR0 += UART_TBIT;                                // One bit time till first bit

  TA0CCTL0 = OUTMOD0 + CCIE;                           // Set TXD on EQU0, Int

  txData = byte;                                       // Load global variable

    txData |= 0x100;                                     // Add mark stop bit to TXData

  txData <<= 1;                                        // Add space start bit
}


void TimerA_UART_print(char *string) {                 // Prints a string using the Timer_A UART

  while (*string)
    TimerA_UART_tx(*string++);
}


#pragma vector = TIMER0_A0_VECTOR                      // Timer_A UART - Transmit Interrupt Handler

__interrupt void Timer_A0_ISR(void) {

  static unsigned char txBitCnt = 10;

  TA0CCR0 += UART_TBIT;                                // Add Offset to CCRx

  if (txBitCnt == 0) {                                 // All bits TXed?

    TA0CCTL0 &= ~CCIE;                                 // All bits TXed, disable interrupt

    txBitCnt = 10;                                     // Re-load bit counter
  }
  else {
    if (txData & 0x01)
      TA0CCTL0 &= ~OUTMOD2;                            // TX Mark '1'
    else
      TA0CCTL0 |= OUTMOD2;                             // TX Space '0'
  }
  txData >>= 1;                                        // Shift right 1 bit
  txBitCnt--;
}


#pragma vector = TIMER0_A1_VECTOR                      // Timer_A UART - Receive Interrupt Handler

__interrupt void Timer_A1_ISR(void) {

  static unsigned char rxBitCnt = 8;

  static unsigned char rxData = 0;

  switch (__even_in_range(TA0IV, TA0IV_TAIFG)) {       // Use calculated branching

  case TA0IV_TACCR1:                                 // TACCR1 CCIFG - UART RX

    TA0CCR1 += UART_TBIT;                         // Add Offset to CCRx

    if (TA0CCTL1 & CAP) {                         // Capture mode = start bit edge

      TA0CCTL1 &= ~CAP;                           // Switch capture to compare mode

      TA0CCR1 += UART_TBIT_DIV_2;                 // Point CCRx to middle of D0
    }
    else {
      rxData >>= 1;

      if (TA0CCTL1 & SCCI)                        // Get bit waiting in receive latch
        rxData |= 0x80;
      rxBitCnt--;

      if (rxBitCnt == 0) {                        // All bits RXed?

        rxBuffer = rxData;                        // Store in global variable

        rxBitCnt = 8;                             // Re-load bit counter

        TA0CCTL1 |= CAP;                          // Switch compare to capture mode

      }
    }
    break;
  }
  BlueData = rxBuffer;
  Bluecount =0;
}


#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  P1OUT ^= debug;
  TA1CTL |= TACLR;
  if(loopcount == 0)
  {
    data =0;
  }
  loopcount++;
  if (loopcount == 3)
  {
    one = count;
    if (one >40 && one <60)
    {
      checksum++;
    }
  }
  else if (loopcount ==5)
  {
    two = count;
    if(two > 18 && two < 30)
    {
      data+=1;
    }

  }
  else if(loopcount ==7)
  {
    three = count;
    if(three > 18 && three < 30)
    {
      data+=2;
    }
  }
  else if(loopcount ==9)
  {
    four = count;
    if(four > 18 && four < 30)
    {
      data+=4;
    }
  }
  else if(loopcount == 11)
  {
    checksum++;
    five = count;
    if(five > 18 && five < 30)
    {
      data+=8;
    }
  }


  count=0;
  resetcount=0;
  P2IFG &= ~IRrec; // P1.3 IFG cleared
  P2IFG &= ~IRrec1; // P1.3 IFG cleared
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{
  count++;
  if(count >1000)
  {
    count=0;
  }
}

void tx_send(int irdata)
{

  P2IE &= ~(IRrec + IRrec1);
  //  __disable_interrupt(); // enable all interrupts
  for (i = 200;i>0;i--)
  {
    P2OUT |= IRLED1;
    P1OUT |= IRLED2;
    P2OUT &= ~IRLED1;
    P1OUT &= ~IRLED2;

  }
  for (i = 10;i>0;i--)
  {

    __delay_cycles(18);

  }
  for(ii = 4; ii>0;ii--)
  {
    bit = irdata & 0x0001;
    if(bit == 1)
    {
      for (i = 100;i>0;i--)
      {
        P2OUT |= IRLED1;
        P1OUT |= IRLED2;
        P2OUT &= ~IRLED1;
        P1OUT &= ~IRLED2;

      }
      for (i = 10;i>0;i--)
      {

        __delay_cycles(18);

      }
    }
    else
    {
      for (i = 50;i>0;i--)
      {
        P2OUT |= IRLED1;
        P1OUT |= IRLED2;
        P2OUT &= ~IRLED1;
        P1OUT &= ~IRLED2;

      }
      for (i = 10;i>0;i--)
      {

        __delay_cycles(18);

      }

    }
    irdata >>= 1;
  }
  P2IE |= (IRrec + IRrec1);
  // __enable_interrupt(); // enable all interrupts
}
















