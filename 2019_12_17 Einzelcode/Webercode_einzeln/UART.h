#define BAUD	115200UL							// Baudrate fuer die UART-Kommunikation festlegen
#define UBRR_BAUD ((F_CPU/(16UL*BAUD))-1)			// Baudrate für USART UBRR-Register ausrechnen
// Datenblatt: UBRR = 3, wenn
//			F_CPU	= 7,3728 MHz und
//		   Baudrate = 115200 Bd
#define UART_MAXSTRLEN 200							// maximale Groesse des Strings
#define ARRAY_MAXSTRANZAHL 125						// maximale Annzahl an Strings in den Messwertarrays

//*****************************************************************************************************************
//					Funktionen für die UART Ausgabe
//*****************************************************************************************************************


int uart_putc0(unsigned char c);					// sende ein Zeichen an PC (RS485) rechte Schnittstelle
int uart_putc1(unsigned char c);					// Bluetooth
int uart_putc2(unsigned char c);					// sende ein Zeichen an PC (RS485) linke Schnittstelle
int uart_putc3(unsigned char c);					// Bluetooth

void uart_puts0(char *s);							// sende einen String an PC (RS485) rechte Schnittstelle
void uart_puts1(char *s);							// Bluetooth
void uart_puts2(char *s);							// sende einen String an PC (RS485) linke Schnittstelle
void uart_puts3(char *s);							// Bluetooth

void uart_put_value0 ( float val , uint8_t n);		// sende eine Variable an den PC (RS485)
void uart_put_value2 ( float val , uint8_t n);		// sende eine Variable an den PC (RS485)
void uart_put_value1 ( float val , uint8_t n);		// sende eine Variable an das Handy
void uart_put_value3 ( float val , uint8_t n);		// sende eine Variable an das Handy

void uart_init();									// Unterprogramm zur Initialisierung der UART
void long_delay(uint16_t ms);						// Unterprogramm fuer Wartezeiten:

void RS485_send_0();								// aktiviere senden an PC (ueber RS485)
void RS485_receive_0();								// aktiviere empfangen von PC (ueber RS485)


void RS485_send_2();								// aktiviere senden an PC (ueber RS485)
void RS485_receive_2();								// aktiviere empfangen PC (ueber RS485)