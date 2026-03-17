// FInal Project
// PIC16F1829 Configuration Bit Settings VERSION 3 MOST CURRENT
// 'C' source line config statements

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

// CONFIG1
#pragma config FOSC = INTOSC  // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF     // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF    // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON     // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF       // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF      // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF    // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF     // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF   // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO   // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON    // Low-Voltage Programming Enable (Low-voltage programming enabled)

/*Serial Configuration*/
#define BAUD 9600                                 // Bits per second transfer rate
#define FOSC 400000L                             // Frequency Oscillator
#define DIVIDER ((int)(FOSC / (16UL * BAUD) - 1)) // Should be 25 for 9600/4MhZ
#define NINE_BITS 0
#define SPEED 0x4     // T Speed
#define RX_PIN TRISC5 // Recieve Pin
#define TX_PIN TRISC4 // Transmit Pin

#define _XTAL_FREQ 4000000.0
const int note_delay = 1000000 / 523 / 2;
const int duration_delay = 320;

void usartConfig(void);
unsigned char getche(void);
unsigned char getch();
void putch(unsigned char byte);
void setup_comms(void);

void play_a_chord_tone();
void play_b_chord_tone();
void play_c_chord_tone();
void play_d_chord_tone();
void play_e_chord_tone();
void play_f_chord_tone();
void play_g_chord_tone();
void play_dash();
void play_dot();
void play_piano_chord();
void play_happy_birthday();
void play_star_wars();
void play_siren_tone();

void initialize_adc() {
   // Configure ADC
   ADCON1bits.ADPREF = 0b00; // VDD is the reference voltage
   ADCON1bits.ADCS = 0b101; // FOSC/16 is the conversion clock

   // Configure ADC
   TRISCbits.TRISC2 = 1; // Set RC2 as input
   TRISCbits.TRISC3 = 1; // Set RC3 as input
   TRISBbits.TRISB5 = 1; // Set RB4 as input
   TRISCbits.TRISC1 = 1; // Set RB5 as input

   ANSELCbits.ANSC2 = 1; // Set RC2 as analog input
   ANSELCbits.ANSC3 = 1; // Set RC3 as analog input
   ANSELBbits.ANSB5 = 1; // Set RB4 as analog input
   ANSELCbits.ANSC1 = 1; // Set RB5 as analog input
}

unsigned int read_adc(unsigned char channel) {
   // Start ADC conversion
   ADCON0bits.ADON = 1; // Enable ADC module
   ADCON0bits.CHS = channel; // Set input channel
   __delay_us(5); // Wait for acquisition time
   ADCON0bits.GO = 1; // Start conversion

   // Wait for the conversion to complete
   while (ADCON0bits.GO)
      ;

   // Read the result
   unsigned int adc_value = ADRES;

   // Disable ADC module
   ADCON0bits.ADON = 0;

   return adc_value;
}

int mode_select(int mode) {
   mode = 0;
   printf("\nModes Available:\n\r beats = 1\n\r diffKey = 2\n\r morse = 3\n\r signal = 4\n\r");
   __delay_ms(100);
   printf("Mode Select:");
   __delay_ms(2500);
   while (!RCIF);
   mode = RCREG;
   printf("(%i) \n\r", mode);
   return mode;
}

void main(void) {
   // Configure oscillator frequency
   OSCCON = 0x6A;

   // Configure I/O ports
   TRISA = 0x00; // Set RA2 and RA5 as outputs, and RA4 as input
   PORTA = 0x00; // Turn off RA2 and RA5
   TRISC = 0x00;
   PORTC = 0x00;
   TRISB = 0x80; // Set RB7 as input
   ANSELB = 0x00; // Disable analog input on port B

   setup_comms();
   usartConfig();

   __delay_ms(1500);
   printf("\rBooting...\n\r");
   __delay_ms(300);

   // Configure ADC
   TRISCbits.TRISC2 = 1; // Set RC2 as input
   TRISCbits.TRISC3 = 1; // Set RC3 as input
   TRISBbits.TRISB5 = 1; // Set RB4 as input
   TRISCbits.TRISC1 = 1; // Set RC1 as input

   TRISAbits.TRISA2 = 0;

   ANSELCbits.ANSC2 = 1; // Set RC2 as analog input
   ANSELCbits.ANSC3 = 1; // Set RC3 as analog input
   ANSELBbits.ANSB5 = 1; // Set RB4 as analog input
   ANSELCbits.ANSC1 = 1; // Set RC1 as analog input

   initialize_adc(); // Initialize ADC


   int beats = 0;
   int diffKey = 0;
   int morse = 1;
   int signal = 0;

   TRISAbits.TRISA2 = 0;
   PORTAbits.RA2 = 0;
   RA5 = 1;
   RC6 = 1;

   unsigned char Letter = 0;
   if (Letter != 's') {
      while (Letter != 's') {
         printf("Press the Letter 's' to start program ... \n\r"); // waiting to start 'S'
         __delay_ms(2500);
         Letter = getch();
         if (Letter != 's') {
            printf("(%c) please try again ... \n\r", Letter);
         } else {
            printf("(%c) continuing program ... \n\n\r", Letter);
         }

      }
   }

   int mode;
   mode_select(mode);
   //switch (mode) {
   //   case 1:
   //      printf("beats %c\n\r", mode);
   //      beats = 1;
   //      break;
   //   case 2:
   //      printf("diffKey %c\n\r", mode);
   //      diffKey = 1;
   //      break;
   //   case 3:
   //      printf("morse %c\n\r", mode);
   //      morse = 1;
   //      break;
   //   case 4:
   //      printf("signals %c\n\r", mode);
   //      signal = 1;
   //      break;
   //   default:
   //      printf("try again\n");
   //}

   while (beats) {
      // Read ADC value for AN6 (RC2)
      unsigned int adc_value_6 = read_adc(6); // rc2
      unsigned int adc_value_7 = read_adc(7); // rc3
      unsigned int adc_value_11 = read_adc(11); // rb5
      unsigned int adc_value_13 = read_adc(13); // rc1
      // Check if threshold for blue light is passed


      if (adc_value_6 != 0xffc0) {
         RA5 = 1; // Turn on RA2 (blue light)
      } else {
         RA5 = 0; // Turn off RA2
         play_c_chord_tone(); // Play C chord tone
         printf("C chord \n\r");
      }

      if (adc_value_7 != 0xffc0) {
         RA2 = 1; // Turn on RA5 (green light)
      } else {
         RA2 = 0; // Turn off RA5
         play_d_chord_tone();
         printf("D chord \n\r");
      }
      if (adc_value_11 != 0xffc0) {
         RC6 = 1; // Turn on RA5 (green light)
      } else {
         RC6 = 0; // Turn off RA5
         play_e_chord_tone();
         printf("E chord \n\r");
      }
      if (adc_value_13 != 0xffc0) {
         RA5 = 1; // Turn on RA5 (green light)
      } else {
         RA5 = 0; // Turn off RA5
         play_g_chord_tone();
         play_f_chord_tone();
         printf("GF chord \n\r");
      }
      //        // Check if threshold for red light is passed
      //        if (PORTAbits.RA4 == 0 && adc_value_6 > 1023 && adc_value_7 > 1023) {
      //           RC6 = 1; // Turn on RC6 (red light)
      //        } else {
      //            RC6 = 1; // Turn off RC6
      //        }



      __delay_ms(100); // Delay for stability
   }

   while (diffKey) {
      // Read ADC value for AN6 (RC2)
      unsigned int adc_value_6 = read_adc(6);
      unsigned int adc_value_7 = read_adc(7);
      unsigned int adc_value_11 = read_adc(11);
      unsigned int adc_value_13 = read_adc(13);
      // Check if threshold for blue light is passed


      if (adc_value_6 == 0xffc0) {
         RA5 = 1; // Turn on RA2 (blue light)
      } else {
         RA5 = 0; // Turn off RA2
         play_a_chord_tone(); // Play C chord tone
         printf("C chord \n\r");
      }

      if (adc_value_7 == 0xffc0) {
         RA2 = 0; // Turn on RA5 (green light)
      } else {
         RA2 = 1; // Turn off RA5
         play_b_chord_tone();
         printf("D chord \n\r");
      }
      if (adc_value_11 == 0xffc0) {
         RC6 = 1; // Turn on RA5 (green light)
      } else {
         RC6 = 0; // Turn off RA5
         play_c_chord_tone();
         printf("E chord \n\r");
      }
      //        if (adc_value_13 != 0xffc0) // Background Beat
      //        {
      //            RA5 = 1; // Turn on RA5 (green light)
      //        }
      //        else
      //        {
      //            RA5 = 0; // Turn off RA5
      //            play_g_chord_tone();
      //             printf("G chord \n\r");
      //        }

      __delay_ms(100); // Delay for stability
   }

   int dot_played = 0; // Flag variable to keep track of whether a dot has been played
   int dash_played = 0; // Flag variable to keep track of whether a dash has been played

   while (morse) {
      // Read ADC value for AN6 (RC2) and AN1
      unsigned int adc_value_6 = read_adc(6);
      unsigned int adc_value_7 = read_adc(7);
      unsigned int adc_value_11 = read_adc(11);

      if (adc_value_6 != 0xffc0) {
         RA2 = 1; // Turn on RA2 (blue light)
         dot_played = 0; // Reset the dot played flag
      } else if (!dot_played) {
         RA2 = 0; // Turn off RA2
         play_dot(); // Play dot
         printf("-");
         dot_played = 1; // Set the dot played flag
      }

      if (adc_value_7 != 0xffc0) {
         RA5 = 1; // Turn on RA5 (green light)
         dash_played = 0; // Reset the dash played flag
      } else if (!dash_played) {
         RA5 = 0; // Turn off RA5
         play_b_chord_tone(); // Play dash

         printf(".");
         dash_played = 1; // Set the dash played flag
      }
      if (adc_value_11 != 0xffc0) {
         printf("\n\r");
      }

      __delay_ms(100); // Delay for stability
   }

   while (signal) { // yes or no signals, only green and red
      // Read ADC value for AN6 (RC2) and AN1
      unsigned int adc_value_6 = read_adc(6);
      unsigned int adc_value_7 = read_adc(7);
      unsigned int adc_value_11 = read_adc(11);

      if (adc_value_6 == 0xffc0) {
         RA5 = 1; // Turn on RA2 (blue light)
      } else {
         RA5 = 0; // Turn off RA2
         play_c_chord_tone(); // Play C chord tone
         printf("C chord \n\r");
      }

      if (adc_value_7 == 0xffc0) {
         RC6 = 0; // Turn on RA5 (green light)
      } else {
         RC6 = 1; // Turn off RA5
         play_e_chord_tone();
         printf("D chord \n\r");
      }


      if (adc_value_11 == 0xffc0) {
         RA2 = 1; // Turn on RA5 (green light)
         dash_played = 0; // Reset the dash played flag
      } else {
         RA2 = 0; // Turn off RA5 RED
         play_siren_tone();
         printf("HELP:::::: EMERGENCY \n\r");

      }
   }
   __delay_ms(100); // Delay for stability
}

void play_a_chord_tone() {
   int i;
   for (i = 0; i < 100; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(2272); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(2272); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_b_chord_tone() {
   int i;
   for (i = 0; i < 100; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(2024); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(2024); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_c_chord_tone() {
   int i;
   for (i = 0; i < 200; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(261.63); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(261.63); // Delay for tone frequency   1911 for deeper longer
   }
   __delay_ms(50); // Delay for tone separation
}

void play_d_chord_tone() {
   int i;
   for (i = 0; i < 200; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(293.7); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(293.7); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_e_chord_tone() {
   int i;
   for (i = 0; i < 200; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(329.6); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(329.6); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_f_chord_tone() {
   int i;
   for (i = 0; i < 200; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(349.2); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(349.2); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_g_chord_tone() {
   int i;
   for (i = 0; i < 200; i++) { // Play for about 1 second
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(392); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(392); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_dot() {
   int i;
   for (i = 0; i < 1; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_ms(100); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_ms(100); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_dash() {
   int i;
   for (i = 0; i < 1; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_ms(20); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_ms(20); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void play_siren_tone() {
   int i;
   for (i = 0; i < 500; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(500); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(500); // Delay for tone frequency
   }
   for (i = 0; i < 500; i++) {
      PORTAbits.RA4 = 1; // Turn on RA4 (buzzer)
      __delay_us(100); // Delay for tone frequency
      PORTAbits.RA4 = 0; // Turn off RA4 (buzzer)
      __delay_us(100); // Delay for tone frequency
   }
   __delay_ms(50); // Delay for tone separation
}

void usartConfig(void) {
   APFCON0 = 0x84; // This sets pins RC5 and RC4 as RX & TX on pic16f1829
   TXCKSEL = 1;
   RXDTSEL = 1; // makes RC4 & 5 TX & RX for USART (Allows ICSP)
}

void setup_comms(void) {

   RX_PIN = 1;
   TX_PIN = 1;
   SPBRG = DIVIDER;
   RCSTA = (NINE_BITS | 0x90);
   TXSTA = (SPEED | NINE_BITS | 0x20);
   TXEN = 1;
   SYNC = 0;
   SPEN = 1;
   BRGH = 1;
}

void putch(unsigned char byte) {
   /* output one byte */
   while (!TXIF) /* set when register is empty */
      continue;
   TXREG = byte;
}

unsigned char getch() {
   /* retrieve one byte */
   while (!RCIF) /* set when register is not empty */
      continue;
   return RCREG;
}

unsigned char getche(void) {
   unsigned char c;
   putch(c = getch());
   return c;
}
