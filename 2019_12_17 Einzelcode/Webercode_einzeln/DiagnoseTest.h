#include "main.h"

//*****************************************************************************************************************
//					Definieren der Messbereichsgrenzen für den Diagnosemodus
//*****************************************************************************************************************
#define obereMessbereichsgrenzeSpannung 60					//Messbereichsgrenze Drehmoment

#define obereMessbereichsgrenzeMoment 250					//Messbereichsgrenze Drehmoment
#define untereMessbereichsgrenzeMoment -250					//Messbereichsgrenze Drehmoment

#define obereMessbereichsgrenzeStrom 15						//Messbereichsgrenze Strom
#define untereMessbereichsgrenzeStrom -15					//Messbereichsgrenze Strom

#define obereMessbereichsgrenzeWindgeschwindigkeit 57.637	//Messbereichsgrenze Windgeschwindigkeit

//*****************************************************************************************************************
//					Externe Variablendeklaration der auch in der main.c verwendeten Variablen
//*****************************************************************************************************************

extern uint8_t	AnzahlTest;									//Zählvariable der Testfunktion

extern uint8_t Diagnosestatus;								// Variable zur Ausgabe des Diagnosestatuses
extern uint8_t Modus;										// Modus=0 ist Messen / Modus = 1 ist Test

extern float adc_Spannung_Spannung;							// Kommazahl der Spannung 0 - 5 V des Spannung ADC
extern float adc_Strom;										//Kommazahl des Stroms
extern float adc_Moment;									// Kommazahl des Drehmoments
extern float adc_Windgeschwindigkeit;						// Kommazahl der Windgeschwindigkeit
extern float adc_Batteriespannung_Spannung;					// Kommazahl der Spannung 0 - 2.56 V des ADC
extern float Fahrradgeschwindigkeit;
extern float	Pedalgeschwindigkeit;

//*****************************************************************************************************************
//					Funktionen für die Diagnose sowie den Testmodus
//*****************************************************************************************************************

void DiagnoseSpannungsmessung (float adc_Spannung_Spannung);				// Diagnosefunktion der Spannungsmessung
void DiagnoseStrommessung (float adc_Strom);								// Diagnosefunktion der Strommessung
void DiagnoseDrehmomentmessung (float adc_Moment);							// Diagnosefunktion der Drehmomentmessung
void DiagnoseWindgeschwindigkeitsmessung (float adc_Windgeschwindigkeit);	// Diagnosefunktion der Windgeschwindigkeit
void TestUebertragung();													// Unterfunktion für den Test der UART