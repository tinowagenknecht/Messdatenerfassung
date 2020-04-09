#define CS1 6																					// Chip Select Portnummer Zuweisung PH6
#define CS2 5																					// Chip Select Portnummer Zuweisung PB5
#define CS3 4																					// Chip Select Portnummer Zuweisung PB4
#define CS4 6																					// Chip Select Portnummer Zuweisung PB6
#define CS5 3																					// Chip Select Portnummer Zuweisung PC3
#define Flash_CS 7																				// Chip Select Portnummer Zuweisung PH7
#define RTC_CS 3																				// Chip Select Portnummer Zuweisung PE3
#define SRAM_CS 0																				// Chip Select Portnummer Zuweisung PB0

//*****************************************************************************************************************
//					Funktionen für den SPI Bus sowie die daran angeschlossenen Slaves
//*****************************************************************************************************************



void SPI_init();																				// SPI Schnittstelle initialisieren

uint8_t transmit_byte(const uint8_t byte);

void ADC_Intern_Init(void);																		// Internen 10-BIT ADC initialisieren
uint16_t ADC_Intern_Read();																		// Internen 10-BIT ADC Wert auslesen

void TLC4545_Reset(uint8_t CS,volatile uint8_t *Port);											// Reset des jeweiligen ADC´s
uint16_t TLC4545_Read(uint8_t CS,uint16_t Aufloesung,volatile uint8_t  *Port);					// Auslesen des ADC Wertes