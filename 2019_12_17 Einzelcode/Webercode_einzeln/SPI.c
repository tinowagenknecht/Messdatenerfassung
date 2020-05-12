#include "main.h"
#include "SPI.h"

//Kommentar hinzugefügt!!!

//Funktion: "SPI_init":
//initialisiert die SPI-Schnittstelle des Controllers
void SPI_init()
{
	uint8_t temp;
	// Set MOSI(PB2), SCK(PB7) output (all others input)
	// SS (PB4) has also to be configured as output before SPI configuration to prevent the controller
	// to be configured as SPI Slave by a low signal on the SS-line
	DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4)| (1<<DDB5) | (1<<DDB6) | (1<<DDB7) ; //CS2/CS1/MOSI/SCK
	DDRH |= (1<<DDB6);
	// SPI - Control Register | DS: P.129 f.
	// ##############################################################
	// BIT        7	 |	6  |   5  |   4  |   3  |   2  |  1   |  0
	// SPCR - > SPIE | SPE | DORD | MSTR | CPOL | CPHA | SPR1 | SPR0
	// --------------------------------------------------------------
	// VALUE	  0     1      0      1      0      1      0     1

	// SPIE = 0 -> Interrups disabled
	// SPE	= 1 -> SPI enabled
	// DORD = 0 -> MSB first
	// MSTR = 1 -> Master
	// CPOL = 0 -> SCK low when IDLE; Leading edge rising; trailing edge falling	(CPOL = 1 -> SCL high when IDLE; leading edge falling; trailing edge rising)
	// CPHA = 0 -> sample on leading edge of SCK, setup on trailing edge of SCK		(CPHA = 1 -> sample on trailing edge of sck, setup on leading edge of SCK)
	// SPR1	= 0 ->
	// SPR0 = 1 -> SCK rate = f_CPU/16
	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<CPHA) ;
	temp = SPSR;
	temp = SPDR;
	//PORTB	|= (1<<PINB1);			//CS1 high
	//PORTB	|= (1<<PINB2);			//CS2 high
	//PORTB	|= (1<<PINB3);			//CS2 high
}



//Funktion: "transmit_byte":
//überträgt ein einzelnes Byte per SPI-Schnittstelle
uint8_t transmit_byte(const uint8_t byte)
{
	uint8_t return_byte;
	// send & receive byte
	SPDR = byte;
	while (!( SPSR & (1<<SPIF)));			// wait for end of transmission
	return_byte = SPDR;
	return return_byte;						// return received byte
}


//Funktion: "TLC4545_Reset":
//führt einen Reset Zyklus für den TLC4545 durch
void TLC4545_Reset(uint8_t CS,volatile uint8_t *Port)
{

	//- - - Variablen Deklaration - - -
	uint8_t temp=0;			//Zwischenspeicher
	uint16_t reset_value=0;	//Reset Wert
	uint8_t MSB=0;			//most significant byte
	uint8_t LSB=0;			//least significant byte
	//- - - - - - - - - - - - - - - - -
	
	#define dummy_byte 0xFF	//Byte für SPI Datenübertragung
	
	//- - - Reset Zyklus starten - - -
	*Port	&= ~(1<<CS);				//fallende Flanke von CS
	temp = transmit_byte(dummy_byte);	//SCLK 8 mal takten, empfangenes Byte verwerfen
	
	//- - - Umwandlungszyklus CS1_high - - -
	*Port	|= (1<<CS);					//CS high
	_delay_us(5);						//kurze Zeit warten, bis die Umwandlung erfolgt ist
	*Port	&= ~(1<<CS);				//CS low
	
	//- - - Reset Wert einlesen - - -
	MSB = transmit_byte(dummy_byte);	//SCLK 8 mal takten, und MSB auslesen
	LSB = transmit_byte(dummy_byte);	//SCLK 8 mal takten, und LSB auslesen
	
	temp = transmit_byte(dummy_byte);	//SCLK 8 mal takten, empfangenes Byte verwerfen
	
	reset_value= ((MSB<<8)|LSB);		//MSB und LSB kombinieren
	
	*Port	|= (1<<CS);					//CS wieder auf normal high
}

//Funktion: "TLC4545_Read":
//liest einen 16-Bit Wert von dem TLC4545 ein
uint16_t TLC4545_Read(uint8_t CS,uint16_t Aufloesung,volatile uint8_t *Port)
{
	//- - - Variablen Deklaration - - -
	uint16_t ADC_value=0;
	//uint8_t temp=0;
	uint8_t MSB=0;
	uint8_t LSB=0;
	//- - - - - - - - - - - - - - - - -
	
	//- - - Umwandlungszyklus CS1_high - - -
	*Port	|= (1<<CS);					//CS high
	_delay_us(5);						//kurze Zeit warten, bis die Umwandlung erfolgt ist
	*Port	&= ~(1<<CS);				//CS low
	
	//- - - Messwert einlesen - - -
	MSB = transmit_byte(dummy_byte);	//SCLK 8 mal takten, und MSB auslesen
	LSB = transmit_byte(dummy_byte);	//SCLK 8 mal takten, und LSB auslesen
	
	*Port	|= (1<<CS);					//CS wieder auf normal high
	
	ADC_value =(MSB<<8) | LSB;			//MSB und LSB kombinieren
	ADC_value =ADC_value & Aufloesung;
	return	ADC_value;					//aktuellen Messwert zurückgeben
}

void ADC_Intern_Init(void)
{
   
  // oder interne Referenzspannung als Referenz für den ADC wählen:
   ADMUX = (1<<REFS1) | (1<<REFS0);

  // Bit ADFR ("free running") in ADCSRA steht beim Einschalten
  // schon auf 0, also single conversion
  ADCSRA = (1<<ADPS2)|(1<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
  ADCSRA |= (1<<ADEN);                  // ADC aktivieren

  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */

  ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung 
  while (ADCSRA & (1<<ADSC) ) {         // auf Abschluss der Konvertierung warten
  }
  /* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
     Wandlung nicht übernommen. */
  (void) ADCW;
}

// ADC Einzelmessung 
uint16_t ADC_Intern_Read()
{
	//- - - Variablen Deklaration - - -
	uint16_t ADC_value=0;
	uint8_t MSB=0;
	uint8_t LSB=0;
	//- - - - - - - - - - - - - - - - -
	
// Kanal waehlen, ohne andere Bits zu beeinflußen
ADCSRB |= (1<<MUX5);			// ADC 8 auswählen
ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
while (ADCSRA & (1<<ADSC) ) {   // auf Abschluss der Konvertierung warten
}
LSB=ADCL;						// LSB auslesen
MSB=ADCH;						// MSB auslesen

ADC_value =(MSB<<8) | LSB;
return ADC_value;                    // 10 BIT ADC Wert zurückgeben
}