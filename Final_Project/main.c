#include <msp430.h> 

/*Modulated period = 1.408ms
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1DIR |= BIT2;             // P1.2 to output
    P1SEL |= BIT2;             // P1.2 to TA0.1
    CCR0 = 28;             // PWM Period
    CCTL1 = OUTMOD_7;          // CCR1 reset/set
    CCR1 = 28;                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode

 //   _BIS_SR(LPM0_bits);

	for(;;) {
		volatile unsigned int i,t;	// volatile to prevent optimization
		t = 50;
		while(t!=0){
			CCR1 = 14;
			t--;
		}
		i = 50;					// SW Delay
		while(i != 0){
			CCR1 = 14;
			i--;
		}
	}


}
