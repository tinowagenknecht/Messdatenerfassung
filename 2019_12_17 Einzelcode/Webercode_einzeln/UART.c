#include "main.h"
#include "UART.h"
//Kommentar wurde geändert!!!
/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					uart_putcX																					|*/
/*+-------------------------------------------------------------------------------------------------------------+*/

int uart_putc0(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)));						// warten bis Senden möglich
	UDR0 = c;											// sende Zeichen
	return 0;
}

int uart_putc1(unsigned char c)
{
	while (!(UCSR1A & (1<<UDRE1)));						// warten bis Senden möglich
	UDR1 = c;											// sende Zeichen
	return 0;
}


int uart_putc2(unsigned char c)
{
	while (!(UCSR2A & (1<<UDRE2)));						// warten bis Senden möglich
	UDR2 = c;											// sende Zeichen
	return 0;
}

int uart_putc3(unsigned char c)
{
	while (!(UCSR3A & (1<<UDRE3)));						// warten bis Senden möglich
	UDR3 = c;											// sende Zeichen
	return 0;
}

/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					uart_putsX																					|*/
/*+-------------------------------------------------------------------------------------------------------------+*/

void uart_puts0(char *s)
{
	while (*s)											// so lange *s != '\0' also ungleich dem
	{													// "String-Endezeichen(Terminator)"
		uart_putc0(*s);
		s++;
	}
	while (!(UCSR0A & (1<<UDRE0)));					// warten bis gesendet
}

void uart_puts1(char *s)
{
	while (*s)											// so lange *s != '\0' also ungleich dem
	{													// "String-Endezeichen(Terminator)"
		uart_putc1(*s);
		s++;
	}
	while (!(UCSR1A & (1<<UDRE1)));					// warten bis gesendet
}

void uart_puts2(char *s)
{
	while (*s)											// so lange *s != '\0' also ungleich dem
	{													// "String-Endezeichen(Terminator)"
		uart_putc2(*s);
		s++;
	}
	while (!(UCSR2A & (1<<UDRE2)));						// warten bis gesendet
}

void uart_puts3(char *s)
{
	while (*s)											// so lange *s != '\0' also ungleich dem
	{													// "String-Endezeichen(Terminator)"
		uart_putc3(*s);
		s++;
	}
	while (!(UCSR3A & (1<<UDRE3)));					// warten bis gesendet
}
/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					uart_put_value          																	|*/
/*+-------------------------------------------------------------------------------------------------------------+*/
void uart_put_value0 (float val , uint8_t n)
{
	
	char s [ 8 ] ;
	dtostrf ( val , 6 , n , s ) ;
	uart_puts0 ( s ) ;
	uart_puts0(";");
	
}

void uart_put_value1 (float val , uint8_t n)
{
	
	char s [ 8 ] ;
	dtostrf ( val , 6 , n , s ) ;
	uart_puts1 ( s ) ;
	uart_puts1(";");
	
}

void uart_put_value2 (float val , uint8_t n)
{
	
	char s [ 8 ] ;
	dtostrf ( val , 6 , n , s ) ;
	uart_puts2 ( s ) ;
	uart_puts2(";");
	
}

void uart_put_value3 (float val , uint8_t n)
{
	
	char s [ 8 ] ;
	dtostrf ( val , 6 , n , s ) ;
	uart_puts3 ( s ) ;
	uart_puts3(";");
	
}

//*****************************************************************************************************************
//					Laengere Wartezeiten als 10 ms mit "_delay_ms()" Funktion
//*****************************************************************************************************************

void long_delay(uint16_t ms)
{
	uint16_t i;
	for(i=0; i < ms; i++) 
	{
		_delay_ms(1);									// für Zeiten laenger 10 ms mit "_delay_ms()" Funktion
	}
}


/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					USART initialisieren																		|*/
/*+-------------------------------------------------------------------------------------------------------------+*/
 
void uart_init(void)
{
	
	//************************************************************************************************************
	//				USART PC
	//************************************************************************************************************
		
	UBRR0H=(unsigned char)(UBRR_BAUD >> 8);				// Baudrate in High UBRR-Register schreiben falls 
														// UBRR_BAUD > 8
	UBRR0L=UBRR_BAUD;									// Baudrate in Low UBRR-Register schreiben
	//---------------------------
	UCSR0A = (1 << UDRE0);								// Enable Flag: transmit buffer is ready to receive new data
	//---------------------------
	UCSR0B|= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);						// Enable Receive Complete Interrupt
														// Enable USART Receiver
														// Enable USART Transmitter
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00)|(1<<USBS0);			// 8 Datenbits und 2 Stop Bits

	while (UCSR0A & (1 << RXC0))
	{
		UDR0;											// UDR auslesen (Wert wird nicht verwendet)
	}
	
	UBRR1H=(unsigned char)(UBRR_BAUD >> 8);				// Baudrate in High UBRR-Register schreiben falls
	// UBRR_BAUD > 8
	UBRR1L=UBRR_BAUD;									// Baudrate in Low UBRR-Register schreiben
	//---------------------------
	UCSR1A = (1 << UDRE1);								// Enable Flag: transmit buffer is ready to receive new data
	//---------------------------
	UCSR1B|= (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);						// Enable Receive Complete Interrupt
	// Enable USART Receiver
	// Enable USART Transmitter
	UCSR1C|=(1<<UCSZ11)|(1<<UCSZ10)|(1<<USBS1);			// 8 Datenbits und 2 Stop Bits

	while (UCSR1A & (1 << RXC1))
	{
		UDR1;											// UDR auslesen (Wert wird nicht verwendet)
	}
	
	UBRR2H=(unsigned char)(UBRR_BAUD >> 8);				// Baudrate in High UBRR-Register schreiben falls
	// UBRR_BAUD > 8
	UBRR2L=UBRR_BAUD;									// Baudrate in Low UBRR-Register schreiben
	//---------------------------
	UCSR2A = (1 << UDRE2);								// Enable Flag: transmit buffer is ready to receive new data
	//---------------------------
	UCSR2B|= (1<<RXCIE2)|(1<<RXEN2)|(1<<TXEN2);						// Enable Receive Complete Interrupt
	// Enable USART Receiver
	// Enable USART Transmitter
	UCSR2C|=(1<<UCSZ21)|(1<<UCSZ20)|(1<<USBS2);			// 8 Datenbits und 2 Stop Bits

	while (UCSR2A & (1 << RXC2))
	{
		UDR2;											// UDR auslesen (Wert wird nicht verwendet)
	}
	
	UBRR3H=(unsigned char)(UBRR_BAUD >> 8);				// Baudrate in High UBRR-Register schreiben falls
	// UBRR_BAUD > 8
	UBRR3L=UBRR_BAUD;									// Baudrate in Low UBRR-Register schreiben
	//---------------------------
	UCSR3A = (1 << UDRE3);								// Enable Flag: transmit buffer is ready to receive new data
	//---------------------------
	UCSR3B|= (1<<RXCIE3)|(1<<RXEN3)|(1<<TXEN3);						// Enable Receive Complete Interrupt
	// Enable USART Receiver
	// Enable USART Transmitter
	UCSR3C|=(1<<UCSZ31)|(1<<UCSZ30)|(1<<USBS3);			// 8 Datenbits und 2 Stop Bits

	while (UCSR3A & (1 << RXC3))
	{
		UDR3;											// UDR auslesen (Wert wird nicht verwendet)
	}
			
}


//*****************************************************************************************************************
//					Kommunikation mit PC: senden und empfangen über RS485 Schnittstelle
//*****************************************************************************************************************

void RS485_send_0()										// aktiviere senden an PC (ueber RS485)
{												
	PORTH |= (1<<PH5);									// PINC0 = 1 --> aktiviere senden / deaktiviere emfpangen
}


void RS485_receive_0()									// aktiviere empfangen von PC (ueber RS485)
{
	PORTH &= ~(1<<PH5);									// PINC0 = 0 --> aktiviere empfangen / deaktiviere senden
}



//*****************************************************************************************************************
//					Kommunikation mit PC: senden und empfangen über RS485 Schnittstelle
//*****************************************************************************************************************

void RS485_send_2()										// aktiviere senden an Strommessplatine (ueber RS485)
{
	PORTH |= (1<<PH4);									// PINC0 = 1 --> aktiviere senden / deaktiviere emfpangen
}


void RS485_receive_2()									// aktiviere empfangen von Strommessplatine (ueber RS485)
{
	PORTH &= ~(1<<PH4);									// PINC0 = 0 --> aktiviere empfangen / deaktiviere senden
}
