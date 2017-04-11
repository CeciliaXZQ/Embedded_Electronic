/* Project_Final.c */  
/* Compiler CC5x ¨C not ANSI-C */                  

#include "16F690.h"
#include "int16Cxx.h"
#pragma config |= 0x00D4 
#define ON 0   /* LEDs are commom anode, active low */
#define OFF 1
/* The AN10_SELECT value*/
#define AN10_SELECT 0b11101001

 void init( void );
 char R_Duty;
 char G_Duty;
 char B_Duty;
 char colour;
 unsigned long thermistor;
 char colourmix; 

#pragma origin 4 
interrupt int_server( void ){ /* the place for the interrupt routine */
 int_save_registers

	if(TMR1IF){
	   
	    if (thermistor > 510){
	    R_Duty = 32;
		G_Duty = 52;
		B_Duty = 113;
		}
		
		if (thermistor > 490 && thermistor < 509){
			R_Duty = 77;
			G_Duty = 128;
			B_Duty = 0;
		}
		
		if (thermistor > 470 && thermistor < 489){
			R_Duty = 119;
			G_Duty = 101;
			B_Duty = 0;
		}
		
		if (thermistor > 450 && thermistor < 469){
			R_Duty = 119;
			G_Duty = 60;
			B_Duty = 0;
		}
		
		if (thermistor > 430 && thermistor < 449){
			R_Duty = 119;
			G_Duty = 32;
			B_Duty = 0;
		}
		
		if (thermistor > 410 && thermistor < 429){
			R_Duty = 88;
			G_Duty = 12;
			B_Duty = 0;
		}	
		TMR1IF = 0;
	}	 
  int_restore_registers
}
 

void main( void)
{
 init();         /* init portpins as input or output */
 
  while(1){
        /* PWM-generation   */
       	if (TMR0 < R_Duty) colourmix.0 = ON;
		else colourmix.0 = OFF;
		if (TMR0 < G_Duty) colourmix.1 = ON;
		else colourmix.1 = OFF;
		if (TMR0 < B_Duty) colourmix.2 = ON;
		else colourmix.2 = OFF;
		
        GO=1;         // start AD
        while(GO);    // wait for done
		thermistor = ADRESH * 256;	/* store upper two bits from AD result register */
		thermistor += ADRESL;		/* store lower 8 bits from AD result register */
		
		PORTC = colourmix;				
   }
}

void init( void )
{
  R_Duty = 110;
  G_Duty = 0;
  B_Duty = 0;
  
  TRISC.0 = 0;  /* Red light channel */
  TRISC.1 = 0;  /* Green light channel */
  TRISC.2 = 0;  /* Blue light channel */
  T1CON.5 = 1;  /* Set timer 1 with 8-bit prescaler */
  T1CON.4 = 1;  /* Second prescale bit */
  T1CON.0 = 1;  /* TMR1ON - start timer 1 */
  PIE1.0  = 1;	/* TMR1IE - timer 1 interrupt enabled */
   
  OPTION = 0b11000101;    /* Timer0 Prescaler divide by 64   */
  INTCON |= 0b11.000000;  /* enables peripheral interrupts */
  
  TRISB.4 = 1;  /* RB4 used for analogue voltage input */
  ANSELH.2 = 1; /* AN10 analogue input channel */
  
  ADCON0 = 0b1.0.1010.0.1;
  /* ADCON0 register definitions:
		0b1.x.xxxx.x.x  Value read in register is right-justified
		0bx.0.xxxx.x.x  Vdd is used for voltage reference
		0bx.x.1010.x.x  AN10 channel is used 
		0bx.x.xxxx.0.x  AD converter not in progress
		0bx.x.xxxx.x.1  AD converter enabled
  */
  
  ADCON1 = 0b0.101.0000;
  /* ADCON1 register definitions:
		0bx.101.xxxx    AD conversion clock = Fosc/16
  */
  colourmix = 0; 
   
  GIE 	  = 1;  /* Global interrupt flag enabled */
}




/* *********************************** */
/*            HARDWARE                 */
/* *********************************** */

/*
               ___________  ___________  
              |           \/           |
        +5V---|Vdd     16F690       Vss|---GND 
              |RA5        RA0/AN0/(PGD)|
              |RA4            RA1/(PGC)|
              |RA3/!MCLR/(Vpp)  RA2/INT|
              |RC5/CCP              RC0|->- LED-R
              |RC4                  RC1|->- LED-G
              |RC3                  RC2|->- LED-B
              |RC6             RB4/AN10|-<- Thermistor
              |RC7                  RB5|
              |RB7/Tx               RB6|
              |________________________|

*/
