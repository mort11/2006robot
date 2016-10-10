#include "ifi_default.h"
#include "MORT_TIMERS.h"

long Timer1_Count = 0;

void Init_Timers( void )
{
	T1CONbits.T1CKPS0 	= 	1;		//1:8 Prescale (clock=1.25MHx/each tick = 800ns)
	T1CONbits.T1CKPS1 	= 	1;

	TMR1H = 0x85;					// Set Timer1's most significant byte
	TMR1L = 0xED;					// Set Timer1's least signifigant byte

	T1CONbits.TMR1CS 	= 	0;		// Uses internal clock
	
	T1CONbits.T1OSCEN 	= 	0;		// Internal oscillator off

	T1CONbits.RD16 		=	1;		// Timer1 operation done in 16-bit accesses

	IPR1bits.TMR1IP 	=	0;		// Sets interrupt as low priority

	PIR1bits.TMR1IF 	= 	0;		// Overflow flag

	PIE1bits.TMR1IE 	= 	1;		// Timer1 interrupt OK
	
	INTCONbits.GIEL 	= 	1;		// Low priority interrups OK

	T1CONbits.TMR1ON 	= 	1;		// Timer1 set as on
}

void Timer1_Handler( void )
{
	Timer1_Count++;
}
