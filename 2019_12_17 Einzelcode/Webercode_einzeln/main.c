/*+-------------------------------------------------------------------------------------------------------------+*/
/*|				Initialisierung 																				|*/
/*+-------------------------------------------------------------------------------------------------------------+*/

#include "main.h"
#include "SPI.h"
#include "UART.h"
#include "DiagnoseTest.h"

//Ein Testkommentar für GitHub
// Ein zweiter Kommentar
// Ein dritter Kommentar
// Ein vierter Kommentar

// Kommentar von User Tino
// Kommentar direkt über die Webseite!!!

#define Faktor_ADC (4.999923706/65536)				// Faktor zur Umrechnung des 16 Bit Integer Wertes in die Eingangspannung 0 - 5 V
#define Faktor_ADC_Intern (2.516/1024.0)			// Faktor zur Umrechnung des 10 Bit Integer Wertes in die Eingangspannung 0 - 2.56 V
#define FaktorSpannungsteilerBatteriespannung (122000.0/22000.0)	//Faktor des Spannungsteilers
#define bit_reso 0xFFFF								// 16Bit Aufloesung ADC
#define FaktorSpannungsteiler (48200.0/4000.0)		// Faktor des Spannungsteilers
#define FaktorSpannungStrom 21.73913				// Faktor zur Umrechnung von der gemessenen Spannung in einen Strom der durch den LEM Wandler flie�t (46mV/A)
#define FaktorSpannungMoment 250.0					// Faktor zur Umrechnung von der gemessenen Spannung in ein Drehmoment (4mV/Nm)
#define FaktorSpannungDruck 1000.0					// Faktor zur Umrechnung von der gemessenen Spannung in einen Differenzdruck in Pa (1mV/Pa)
#define VrefLEM 1.65								// Referenzspannungsausgang des LEM Wandlers zur Strommessung
#define OffsetDrehmoment 2.5						// Die Drehmomentmesswelle gibt bei 0 NM Drehmoment eine Spannung von 3,5 V ab.
#define OffsetDruck	2.5								// DEr Drucksensor gibt bei 0 KPa Differenzdruck eine Spannung von 2,5V ab.
#define DichteLuft 1.2041							// Dichte von Luft bei 20�C in kg/m�

#define Pi 3.14										// Zahl Pi definieren



uint16_t Ordnung_Median =3;							// Ordung des Medianfilters

uint16_t kSp=0;										// Z�hlvariable des Ringspeichers f�r die Spannunsmessung
uint16_t bSp[3];									// Array f�r die Speicherung der Spannungswerte festlegen
uint16_t Spsort[3];									// Array f�r die Speicherung der sortierten Spannungswerte festlegen

uint16_t kStrom=0;									// Z�hlvariable des Ringspeichers f�r die Strommessung
uint16_t bStrom[3];									// Array f�r die Speicherung der Stromwerte festlegen
uint16_t Stromsort[3];								// Array f�r die Speicherung der sortierten Stromwerte festlegen

uint16_t kDrehm=0;									// Z�hlvariable des Ringspeichers f�r die Drehmomentmessung
uint16_t bDrehm[3];									// Array f�r die Speicherung der Drehmomentwerte festlegen
uint16_t Drehmsort[3];								// Array f�r die Speicherung der sortierten Drehmomentwerte festlegen

uint16_t kDruck=0;									// Z�hlvariable des Ringspeichers f�r die Druckmessung
uint16_t bDruck[3];									// Array f�r die Speicherung der Druckwerte festlegen
uint16_t Drucksort[3];								// Array f�r die Speicherung der sortierten Druckwerte festlegen

uint8_t	AnzahlTest=0;								//Z�hlvariable der Testfunktion

float rekFilterSpannung[2];							// Array f�r die Speicherung der Werte des rekursiven Filters
float rekFilterStrom[2];							// Array f�r die Speicherung der Werte des rekursiven Filters
float rekFilterMoment[2];							// Array f�r die Speicherung der Werte des rekursiven Filters
float rekFilterDruck[2];							// Array f�r die Speicherung der Werte des rekursiven Filters

uint16_t UARTTIME;									// Z�hlwert f�r den Timer zur Ausgabe des Messstrings
uint16_t Startausgabe=0;							// Variable zum Starten der Ausgabe mittels UART

uint8_t	Intervalleinlesen=0;						// Z�hlwert f�r den Timer zum Messen der ADC Werte / Filterung / Umrechnung
uint8_t	StartMessung=0;								// Variable zum Starten der Messung / Filterung / Umrechnung





uint16_t adc_Spannung_Wert = 0;						// 16 Bit Integer Wert des ADC
float adc_Spannung_Wert_gefiltert = 0;				// 16 Bit Integer Wert des ADC gefiltert
float adc_Spannung_Spannung;						// Kommazahl der Spannung 0 - 5 V des Spannung ADC


uint16_t adc_Strom_Wert = 0;						// 16 Bit Integer Wert des ADC
float adc_Strom_Wert_gefiltert = 0;					// 16 Bit Integer Wert des ADC
float	adc_Strom_Spannung;							// Kommazahl der Spannung 0 - 5 V des Strom ADC
float adc_Strom;									//Kommazahl des Stroms

uint16_t adc_Moment_Wert = 0;						// 16 Bit Integer Wert des ADC
float adc_Moment_Wert_gefiltert = 0;				// 16 Bit Integer Wert des ADC
float adc_Moment_Spannung;							// Kommazahl der Spannung 0 - 5 V des Moment ADC
float adc_Moment;									// Kommazahl des Drehmoments

uint16_t adc_Druck_Wert = 0;						// 16 Bit Integer Wert des ADC
float adc_Druck_Wert_gefiltert = 0;					// 16 Bit Integer Wert des ADC
float adc_Druck_Spannung;							// Kommazahl der Spannung 0 - 5 V des Druck ADC
float adc_Druck;									// Kommazahl des Drucks
float adc_Windgeschwindigkeit;						// Kommazahl der Windgeschwindigkeit


uint16_t adc_Batteriespannung_Wert = 0;				// 10 Bit Integer Wert des ADC
float adc_Batteriespannung_Spannung;				// Kommazahl der Spannung 0 - 2.56 V des ADC

uint8_t Diagnosestatus=0;							// Variable zur Ausgabe des Diagnosestatuses
uint8_t Modus=0;									// Modus=0 ist Messen / Modus = 1 ist Test

uint8_t	Modus_Messen=0;								// Starten des Messmodus
uint8_t	Modus_Test=0;								// Starten des Testmodus

uint8_t	Testfunktionaufrufen=0;						// Startvariable zum Aufrufen der Testunterfunktion

uint8_t ZaehlvariableExtInt0=0;
uint8_t ZaehlvariableExtInt1=0;
uint8_t ZeitueberlaufRad=0;
uint8_t ZeitueberlaufPedale=0;
int32_t t_Rad=0;
int32_t t_Pedale=0;
float	t_RadSekunden=0;
float	t_PedaleSekunden=0;
float	t_PedaleMinuten=0;
uint8_t NeuerImpulsRad=0;
uint8_t NeuerImpulsPedale=0;

float Fahrradgeschwindigkeit=0;
float Fahrradgeschwindigkeit_max=0;
float Fahrradgeschwindigkeittemp=0;
uint8_t dbike =0;

float	Pedalgeschwindigkeit=0;
float	Pedalgeschwindigkeit_max=0;
float	Pedalgeschwindigkeittemp=0;
uint8_t dped=0;

float	Radumfang=0;
uint8_t	Reifendurchmesser=28;
uint8_t	AnzahlMagnete=1;



/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					Funktionen initialisieren																	|*/
/*+-------------------------------------------------------------------------------------------------------------+*/



volatile char uart_string_3[UART_MAXSTRLEN + 1] = "";	// String 3 (Bluetooth)
volatile uint8_t uart_str_count_3 = 0;					// Zaehlvariable der Laenge des String 3 (Bluetooth)
volatile uint8_t uart_str_complete_3 = 0;				// String 3 (Bluetooth) komplett empfangen



void Ausgabeinterrupton();								// Unterfunktion zum Einschalten des Timerinterrupts f�r die UART Ausgabe
void Ausgabeinterruptoff();								// Unterfunktion zum Ausschalten des Timerinterrupts f�r die UART Ausgabe
void TimeinOCR();										// Unterfunktion zum setzen des Timer Output Compare Registers f�r die UART Ausgabe

void Messinterrupton();									// Unterfunktion zum Einschalten des Timerinterrupts f�r die Messung
void Messinterruptoff();								// Unterfunktion zum Ausschalten des Timerinterrupts f�r die Messung


int time;												// Periodenzeit f�r Werteausgabe, max time=65535ms=65ses
uint8_t count_buffer=0;									// Zaehlvariable fuer Buffer-Array
uint8_t count_string=0;									// Zaehlvariable fuer String
unsigned char string_ende=0;							// Hilfsvariable zur Signalisierung das der String zu Ende ist
unsigned char nextChar=0;								// Hilfsvariable f�r naechstes Zeichen
char buffer[UART_MAXSTRLEN + 1] = "";					// String zur Zwischenspeicherung der Zeit fuer verzoegertes senden an APP
char buffer_magnet[2 + 1] = "";							// String zur Zwischenspiecherung der Magnetanzahl des Tachoimpulsmessger�tes
char buffer_diameter[2 + 1] = "";						// String zur Zwischenspiecherung des Raddurchmessers in  Zoll (bsp. 28)
volatile uint16_t millisekunden;









void quick_sort (uint16_t *a, uint16_t n);				// Unterfunktion zur Sortierung eines Arrays der Gr��e nach
uint16_t median(uint16_t a[],uint16_t n);				// Unterfunktion zur Median Filterung
float rekFilter(uint16_t u,float v[]);					// Unterfunktion zur Rekursiven Filterung



float Messwerte_ADC_Spannung(uint16_t Ordnung_Median);	//Auslesen der Spannungsmessung sowie schreiben der Werte in Ringspeicher
float Messwerte_ADC_Drehmoment(uint16_t Ordnung_Median);//Auslesen der Strommessung sowie schreiben der Werte in Ringspeicher
float Messwerte_ADC_Strom(uint16_t Ordnung_Median);		//Auslesen der Drehmomentmessung sowie schreiben der Werte in Ringspeicher
float Messwerte_ADC_Druck(uint16_t Ordnung_Median);		//Auslesen der Druckmessung sowie schreiben der Werte in Ringspeicher

float UmrechnungSpannung(float adc_Spannung_Wert_gefiltert);			// Umrechnung in Spannungswert
float UmrechnungStrom(float adc_Strom_Wert_gefiltert);					// Umrechnung in Stromwert
float UmrechnungDrehmoment(float adc_Moment_Wert_gefiltert);			// Umrechnung in Drehmomentwert
float UmrechnungDruck(float adc_Druck_Wert_gefiltert);					// Umrechnung in die Windgeschwindigkeit
float UmrechnungBatteriespannung(uint16_t adc_Batteriespannung_Wert);	// Umrechnung in die Batteriespannung



float UmfangRad(float d_zoll);							// Unterfunktion zur Berechnen des Radumfangs



/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					Hauptprogramm																				|*/
/*+-------------------------------------------------------------------------------------------------------------+*/

int main(void)
{
		uart_init();										// USART konfigurieren

		DDRH |= (1<<PH4);									// PH4 als Ausgang konfigurieren zum enablen der linken Schnittstelle an PC
		DDRH |= (1<<PH5);									// PH5 als Ausgang konfigurieren zum enablen der rechten Schnittstelle an PC
		DDRA |= (1<<PA6);									// PA6 als Ausgang konfigurieren zum Einschalten des Bluetooth Modules

		DDRA |= (1<<PA0);									// PA0 als Ausgang konfigurieren zur Aktivierung der Versorgungsspannung

		DDRH |= (1<<PH3);									// PH3 als Ausgang konfigurieren zum Einschalten der Spannungsmessung

		DDRD &=~(1<<PD0);									// PD0 als Eingang konfigurieren f�r den Externen Interrupt des Rades
		DDRD &=~(1<<PD1);									// PD0 als Eingang konfigurieren f�r den Externen Interrupt der Pedale



/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					Globale Interrupts freigeben, GIE (Global Interrupt Enable) Bit setzen						|*/
/*+-------------------------------------------------------------------------------------------------------------+*/

sei();														// Globale Interrupt Enable (Interrupts freigeben)



/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					Hauptschleife	  																			|*/
/*+-------------------------------------------------------------------------------------------------------------+*/
	PORTA |= (1<<PA0);										//Aktivierung der Spannungsmessung der Versorgungsspannung

	PORTA |= (1<<PA6);										// Bluetooth Modul 2 einschalten

	PORTH |= (1<<PH3);										// MESSON Spannungsmessung einschalten

	RS485_send_0();											// aktiviere senden an PC (ueber RS485)
	RS485_send_2();											// aktiviere senden an PC (ueber RS485)

	SPI_init();

	TLC4545_Reset(CS1,&PORTH);								// ADC der Momentmessung initialisieren
	TLC4545_Reset(CS2,&PORTB);								// ADC der Druckmessung initialisieren
	TLC4545_Reset(CS3,&PORTB);								// ADC der Spannungsmessung initialisieren
	TLC4545_Reset(CS4,&PORTB);								// ADC der Strommessung initialisieren
	ADC_Intern_Init();										// Interner ADC initialisieren

	rekFilterSpannung[0]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterSpannung[1]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterStrom[0]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterStrom[1]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterMoment[0]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterMoment[1]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterDruck[0]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen
	rekFilterDruck[1]=0;									// Array Wert zur Berechnung mittels des rekursiven Filters auf Null setzen

	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Timer 0 zum Auslesen der ADC�s / Filterung / Umrechnung sowie der Diagnose																				|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/

	TCCR0A |= (1<<WGM01);									//CTC Mode 2 anw�hlen
	TCCR0B |= (1<<CS02) | (1<<CS00);						// Vorteiler 1024 ausw�hlen daraus ergibt sich eine Frequenz von 7200Hz
	Intervalleinlesen= 5*7.2;								// Intervall zum Einlesen und berechnen der Messwerte auf 5ms setzen
	OCR0A = Intervalleinlesen;								// Output Compare Register A mit dem berechneten Zeitwert beschreiben
	//TIMSK0 |= (1<<OCIE0A);								// Interrupt einschalten der bei dem erreichten OCCR0A Wert ausgel�st wird






	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Timer 1 zur UART Ausgabe initialisieren																					|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/

	TCCR1B |= (1<<WGM12) | (1<<CS12) | (1<<CS10);			// Vorteiler 1024 ausw�hlen daraus ergibt sich eine Frequenz von 7200Hz
	UARTTIME=324;											// Bei einer Ausgabezeit von 45ms und einer Taktfrequenz von 7200 Hz ergibt sich der Wert zu 324
	OCR1A = UARTTIME;										// Output Compare Register A mit dem berechneten Zeitwert beschreiben

	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Timer 3 initalisieren f�r den Radimpuls	  																			|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/

	OCR3A=720;												// Entprellzeit des Externen Interrupts in Output Compare Register A des Timer3 schreiben
	TIMSK3|=(1<<TOIE3);										// �berlaufinterrupt einschalten

	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Timer 4 initalisieren f�r den Pedalimpuls	  																			|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/

	OCR4A=720;												// Entprellzeit des Externen Interrupts in Output Compare Register A des Timer3 schreiben
	TIMSK4|=(1<<TOIE4);										// �berlaufinterrupt einschalten


	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Ext. Interrupt initalisieren f�r den Radimpuls	  																			|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/
	EICRA |= (1<<ISC01);									// Externer Interrupt bei fallender Flanke ausl�sen
	EIMSK |= (1<<INT0);										// Externer Interrupt enable

	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Ext. Interrupt initalisieren f�r den Radimpuls	  																			|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/
	EICRA |= (1<<ISC11);									// Externer Interrupt bei fallender Flanke ausl�sen
	EIMSK |= (1<<INT1);										// Externer Interrupt enable

	Radumfang=UmfangRad(Reifendurchmesser);					// Unterfunktion zur Berechnung des Radumfangs

while(1)
{
	   //uart_puts2("$$$ON; --> Funktioniert!!!\r\n");
	//uart_puts0("$$$ON; --> Funktioniert!!!\r\n");

	if(Modus_Messen==1)										// Wenn der Modus Messen mittels empfangenen Befehls aktiviert wurde wird dieser Programmcode abgearbeitet
	{
		if(StartMessung==1)									// Der Timer 0 zum Auslesen der ADC�s / Filterung / Umrechnung sowie der Diagnose setzt die Variable Start Messung bei jedem Timerinterrupt auf 1
		{
			adc_Spannung_Wert_gefiltert=Messwerte_ADC_Spannung(Ordnung_Median);						// ADC Wert einlesen und Filterung durchfuehren
			adc_Spannung_Spannung=UmrechnungSpannung(adc_Spannung_Wert_gefiltert);					// Umrechnung des ADC Wertes in die akuelle Spannung
			DiagnoseSpannungsmessung(adc_Spannung_Spannung);										// Diagnosefunktion aufrufen





			adc_Strom_Wert_gefiltert=Messwerte_ADC_Strom(Ordnung_Median);							// ADC Wert einlesen und Filterung durchfuehren
			adc_Strom=UmrechnungStrom(adc_Strom_Wert_gefiltert);									// Umrechnung des ADC Wertes in den aktuellen Strom
			DiagnoseStrommessung(adc_Strom);														// Diagnosefunktion aufrufen


			adc_Moment_Wert_gefiltert=Messwerte_ADC_Drehmoment(Ordnung_Median);						// ADC Wert einlesen und Filterung durchfuehren
			adc_Moment=UmrechnungDrehmoment(adc_Moment_Wert_gefiltert);								// Umrechnung des ADC Wertes in das aktuelle Drehmoment
			DiagnoseDrehmomentmessung(adc_Moment);													// Diagnosefunktion aufrufen


			adc_Druck_Wert_gefiltert=Messwerte_ADC_Druck(Ordnung_Median);							// ADC Wert einlesen und Filterung durchfuehren
			adc_Windgeschwindigkeit=UmrechnungDruck(adc_Druck_Wert_gefiltert);						// Umrechnung des ADC Wertes in die akuelle Windgeschwindigkeit
			DiagnoseWindgeschwindigkeitsmessung(adc_Windgeschwindigkeit);							// Diagnosefunktion aufrufen

			adc_Batteriespannung_Wert=ADC_Intern_Read();											// ADC 10 Bit Integer Wert auslesen
			adc_Batteriespannung_Spannung=UmrechnungBatteriespannung(adc_Batteriespannung_Wert);	// Umrechnung des ADC Wertes in die aktuelle Batteriespannung

			StartMessung=0;
		}

		//------------------------------------------------------------
		//Geschwindigkeit Fahrrad, nach neuen Impuls
		//------------------------------------------------------------
		if (ZaehlvariableExtInt0==0)	// Wenn noch nicht 1 Externer Interrupt kam soll die Geschwindigkeit des Fahrrades zu 0 gesetzt werden
		{
			Fahrradgeschwindigkeit=0;
		}
		else							// Wenn bereits 1 Externer Interrupt kam soll die Geschwindigkeit des Fahrrades zu 0 gesetzt werden
		{
			if (NeuerImpulsRad==1)		// Wenn ein neuer Impuls gekommen ist wird dieser Code abgearbeitet
			{
				t_RadSekunden = t_Rad * (0.000139); // Umrechnen in Sekunden
				Fahrradgeschwindigkeit = (Radumfang/(t_RadSekunden*AnzahlMagnete)*3.6);	// Umrechnen in Km/h

				if (dbike==0)
				{
					dbike=1;
					Fahrradgeschwindigkeit=0;
				}

				if (Fahrradgeschwindigkeit>=Fahrradgeschwindigkeit_max)					// bestimmen des Maximums
				{
					Fahrradgeschwindigkeit_max=Fahrradgeschwindigkeit;
				}

				NeuerImpulsRad=0;						// R�cksetzen Zeitdifferenz
			}
			else						// Wenn kein neuer Impuls gekommen ist wird dieser Code abgearbeitet
			{
				//- - - - - - - - - - - - - Zeitdiff berechnen - - - - - - - - - - - - -
				t_Rad=TCNT3+(ZeitueberlaufRad*65535);


				t_RadSekunden = t_Rad * (0.000139); // Umrechnen in Sekunden
				Fahrradgeschwindigkeittemp = (Radumfang/(t_RadSekunden*AnzahlMagnete)*3.6);	// Umrechnen in Km/h

				if (Fahrradgeschwindigkeittemp<=5)					// wenn sich das Fahrrad langsamer als 5 Km/h bewegt
				{
					dbike=0;
					Fahrradgeschwindigkeit=0;
				}

			}
		}


		//------------------------------------------------------------
		//Trittgeschwindigkeit, nach neuen Impuls
		//------------------------------------------------------------
		if (ZaehlvariableExtInt1==0)			// Wenn noch nicht 1 Externer Interrupt kam soll die Geschwindigkeit der Pedale zu 0 gesetzt werden
		{
			Pedalgeschwindigkeit=0;
		}
		else									// Wenn bereits 1 Externer Interrupt kam soll die Geschwindigkeit der Pedale zu 0 gesetzt werden
		{
			if (NeuerImpulsPedale==1)			// Wenn ein neuer Impuls gekommen ist wird dieser Code abgearbeitet
			{
				t_PedaleSekunden = t_Pedale * (0.000139); // Umrechnen in Sekunden
				t_PedaleMinuten = t_PedaleSekunden/60;			// Umrechnen in Minuten
				Pedalgeschwindigkeit = (1/(t_PedaleMinuten*8));		// Geschwindigkeit in Umdrehungen pro Minute
				// 8 Impulse pro Umdrehung (nicht 16!!)

				if (dped==0)
				{
					dped=1;
					Pedalgeschwindigkeit=0;
				}

				if (Pedalgeschwindigkeit>=Pedalgeschwindigkeit_max)		// bestimmen des Maximums
				{
					Pedalgeschwindigkeit_max=Pedalgeschwindigkeit;
				}

				NeuerImpulsPedale=0;						// R�cksetzen Zeitdifferenz
			}
			else								// Wenn kein neuer Impuls gekommen ist wird dieser Code abgearbeitet
			{
				//- - - - - - - - - - - - - Zeitdiff berechnen - - - - - - - - - - - - -
				t_Pedale=TCNT4+(ZeitueberlaufPedale*65535);


				t_PedaleSekunden = t_Pedale * (0.000139); // Umrechnen in Sekunden
				t_PedaleMinuten = t_PedaleSekunden/60;			// Umrechnen in Minuten
				Pedalgeschwindigkeittemp = (1/(t_PedaleMinuten*8));		// Geschwindigkeit in Umdrehungen pro Minute
				// 8 Impulse pro Umdrehung (nicht 16!!)

				if (Pedalgeschwindigkeittemp<=10)					// wenn sich das Fahrrad langsamer als 10 1/min bewegt
				{
					dped=0;
					Pedalgeschwindigkeit=0;
				}

			}
		}


	}


	/*+-------------------------------------------------------------------------------------------------------------+*/
	/*|					Ausgabe Messstring																					|*/
	/*+-------------------------------------------------------------------------------------------------------------+*/
	if(Startausgabe==1)													// Der Timer 1 zur UART Ausgabe setzt die Variable Startausgabe bei jedem Timerinterrupt auf 1
	{	if(Modus_Test==1)												// Wenn der Befehl zum Test der UART Ausgabe empfangen wurde wird die Variable Modus_Test zu 1 gesetzt
		{
			TestUebertragung();											// Unterfunktion zur Berechnung des Testwertes aufrufen
			uart_put_value3 ( adc_Moment, 4 ) ;							// Ausgabe des Drehmoments mittels UART
			uart_put_value3 ( adc_Windgeschwindigkeit, 4 ) ;			// Ausgabe der Windgeschwindigkeit mittels UART
			uart_put_value3 ( Pedalgeschwindigkeit, 4 ) ;				// Ausgabe der Pedalgeschwindigkeit in U/min mittels UART
			uart_put_value3 ( Fahrradgeschwindigkeit, 4 ) ;				// Ausgabe der Fahrradgeschwindigkeit in km/h mittels UART
			uart_put_value3 ( Modus, 0 ) ;								// Ausgabe des Modus mittels UART
			uart_put_value3 ( Diagnosestatus, 0 ) ;						// Ausgabe des Diagnosestatus mittels UART
			uart_put_value3 ( adc_Batteriespannung_Spannung, 4 ) ;		// Ausgabe der Batteriespannung in V mittels UART
			uart_put_value3 ( adc_Spannung_Spannung , 4 ) ;				// Ausgabe des Spannungswertes in V mittels UART
			uart_put_value3 ( adc_Strom, 4 ) ;							// Ausgabe des Stroms in A mittels UART
			uart_puts3("\r\n");											// sende CR+LF an PC

			if(AnzahlTest>=5)											// Wenn die Variable AnzahlTest gr��er oder gleich 5 ist wird die Variable auf 0 zur�ckgesetzt, sodass an das Unterprogramm TestUebertragung nur die Werte 0 bis 5 uebertragen werden
			{
				AnzahlTest=0;
			}
			else														// Wenn die Variable kleiner 5 ist wird sie hochgez�hlt
			{
				AnzahlTest++;
			}

		}
		else															// Wenn der Modus_Test nicht 1 ist werden die gemessen Werte mittels UART ausgegeben
		{
			uart_put_value3 ( adc_Moment, 4 ) ;							// Ausgabe des Drehmoments mittels UART
			uart_put_value3 ( adc_Windgeschwindigkeit, 4 ) ;			// Ausgabe der Windgeschwindigkeit mittels UART
			uart_put_value3 ( Pedalgeschwindigkeit, 4 ) ;				// Ausgabe der Pedalgeschwindigkeit in U/min mittels UART
			uart_put_value3 ( Fahrradgeschwindigkeit, 4 ) ;				// Ausgabe der Fahrradgeschwindigkeit in km/h mittels UART
			uart_put_value3 ( Modus, 0 ) ;								// Ausgabe des Modus mittels UART
			uart_put_value3 ( Diagnosestatus, 0 ) ;						// Ausgabe des Diagnosestatus mittels UART
			uart_put_value3 ( adc_Batteriespannung_Spannung, 4 ) ;		// Ausgabe der Batteriespannung in V mittels UART
			uart_put_value3 ( adc_Spannung_Spannung , 4 ) ;				// Ausgabe des Spannungswertes in V mittels UART
			uart_put_value3 ( adc_Strom, 4 ) ;							// Ausgabe des Stroms in A mittels UART
			uart_puts3("\r\n");											// sende CR+LF an PC

			uart_put_value0 ( adc_Moment, 4 ) ;							// Ausgabe des Drehmoments mittels UART
			uart_put_value0 ( adc_Windgeschwindigkeit, 4 ) ;			// Ausgabe der Windgeschwindigkeit mittels UART
			uart_put_value0 ( Pedalgeschwindigkeit, 4 ) ;				// Ausgabe der Pedalgeschwindigkeit in U/min mittels UART
			uart_put_value0 ( Fahrradgeschwindigkeit, 4 ) ;				// Ausgabe der Fahrradgeschwindigkeit in km/h mittels UART
			uart_put_value0 ( Modus, 0 ) ;								// Ausgabe des Modus mittels UART
			uart_put_value0 ( Diagnosestatus, 0 ) ;						// Ausgabe des Diagnosestatus mittels UART
			uart_put_value0 ( adc_Batteriespannung_Spannung, 4 ) ;		// Ausgabe der Batteriespannung in V mittels UART
			uart_put_value0 ( adc_Spannung_Spannung , 4 ) ;				// Ausgabe des Spannungswertes in V mittels UART
			uart_put_value0 ( adc_Strom, 4 ) ;							// Ausgabe des Stroms in A mittels UART
			uart_puts0("\r\n");											// sende CR+LF an PC
		}





		Startausgabe=0;
	}


		//*********************************************************************************************************
		//			Befehl vom Bluetooth Modul empfangen
		//*********************************************************************************************************

		if (uart_str_complete_3 == 1)					// wenn Daten von Bluetooth kommen
		{
			//*****************************************************************************************************
			//		Befehl "$$$ON;" von App empfangen
			//*****************************************************************************************************

			if (strcmp(uart_string_3,"$$$ON;") == 0)
			{

				Ausgabeinterrupton();					// Ausgabeinterrupt starten

			}



			//*****************************************************************************************************
			//		Befehl "$$$OFF;" von App empfangen
			//*****************************************************************************************************

			else if (strcmp(uart_string_3,"$$$OFF;") == 0)
			{
				Ausgabeinterruptoff();					// Ausgabeinterrupt stoppen

			}

			//*****************************************************************************************************
			//		Befehl "$$$RATE;xxxx" von App empfangen (vergleiche die ersten 8 Zeichen)
			//*****************************************************************************************************

			else if (strncmp(uart_string_3, "$$$RATE;", 8) == 0)
			{
				count_buffer = 0;						// Zaehlvariable fuer buffer auf null setzen
				count_string = 8;						// Zaehlvariable fuer string auf erste Zahl von Zeit setzen
				string_ende = 0;						// String ende ist noch nicht erreicht
				nextChar = uart_string_3[count_string];	// Hilsvariable mit naechstem Zeichen fuellen

				while ((nextChar != '\r') && (nextChar != '\n') && (count_string < UART_MAXSTRLEN))
				{
					buffer[count_buffer] = nextChar;
					count_buffer++;
					count_string++;
					nextChar = uart_string_3[count_string];
				}

				buffer[count_buffer] = '\0';			// fuege String-Ende Zeichen an String
				time = atoi(buffer);					// konvertiere string to int und schreibe ihn in die
														// Hilfsvariable fuer die Periodendauer zwischen
														// den Sendevorgangen
				millisekunden = time;					// schreibe Zeit in Millisekunden zum dekrementieren
				UARTTIME=millisekunden*7.2;				// Umrechnung der Zeit in ms in einen Z�hlwert f�r das Register des Timers
				TimeinOCR();							// Aufrufen der Unterfunktion zum beschreiben des Wertes in das Timer Register

			}

			//*****************************************************************************************************
			//		Befehl "$$$calib;" von App empfangen (Kalibriere Staudrucksensor)
			//*****************************************************************************************************

			else if ( strcmp(uart_string_3, "$$$calib;") == 0 )
			{

			}


			//*****************************************************************************************************
			//		Befehl "$$$get config;" von App empfangen (Eingestellte Magnetanzahl und Fahrradgr��e in Zoll)
			//*****************************************************************************************************

			else if(strcmp(uart_string_3,"$$$get config;") == 0)
			{
				uart_put_value0 ( Reifendurchmesser, 0 ) ;	// Ausgabe des Drehmoments mittels UART
				uart_put_value0 ( AnzahlMagnete, 0 ) ;	// Ausgabe des Drehmoments mittels UART
				uart_puts0("\r\n");									// sende CR+LF an PC
			}



			//*****************************************************************************************************
			//		Befehl "$$$set diameter;xx" von App empfangen (einstellen Fahrradgr��e in Zoll)
			//*****************************************************************************************************

			else if(strncmp(uart_string_3, "$$$set diameter;", 16) == 0)
			{
				count_buffer = 0;						// Zaehlvariable fuer buffer auf null setzen
				count_string = 16;						// Zaehlvariable fuer string auf erste Zahl von Zeit setzen
				string_ende = 0;						// String ende ist noch nicht erreicht
				nextChar = uart_string_3[count_string];	// Hilsvariable mit naechstem Zeichen fuellen

				while ((nextChar != '\r') && (nextChar != '\n') && (count_string < UART_MAXSTRLEN))
				{
					buffer_diameter[count_buffer] = nextChar;
					count_buffer++;
					count_string++;
					nextChar = uart_string_3[count_string];
				}

				buffer_diameter[count_buffer] = '\0';	// fuege String-Ende Zeichen an String
				Reifendurchmesser = atoi(buffer_diameter);					// konvertiere string to int
				Radumfang=UmfangRad(Reifendurchmesser);
			}



			//*****************************************************************************************************
			//		Befehl "$$$set magnet;xx" von App empfangen (festlegen Anzahl Magnete fuer Impulse pro Umdreh.)
			//*****************************************************************************************************

			else if(strncmp(uart_string_3, "$$$set magnet;", 14) == 0)
			{
				count_buffer = 0;						// Zaehlvariable fuer buffer auf null setzen
				count_string = 14;						// Zaehlvariable fuer string auf erste Zahl von Zeit setzen
				string_ende = 0;						// String ende ist noch nicht erreicht
				nextChar = uart_string_3[count_string];	// Hilsvariable mit naechstem Zeichen fuellen

				while ((nextChar != '\r') && (nextChar != '\n') && (count_string < UART_MAXSTRLEN))
				{
					buffer_magnet[count_buffer] = nextChar;
					count_buffer++;
					count_string++;
					nextChar = uart_string_3[count_string];
				}

					buffer_magnet[count_buffer] = '\0';	// fuege String-Ende Zeichen an String
					AnzahlMagnete = atoi(buffer_magnet);					// konvertiere string to int

			}

			//*****************************************************************************************************
			//		Befehl "$$$Messen;" von App empfangen
			//*****************************************************************************************************

			else if (strcmp(uart_string_3,"$$$Messen;") == 0)
			{
					Modus_Test=0;
					Messinterrupton();						// Messinterrupt starten
					Modus_Messen=1;
					Modus=0;
			}

			//*****************************************************************************************************
			//		Befehl "$$$Test;" von App empfangen
			//*****************************************************************************************************

			else if (strcmp(uart_string_3,"$$$Test;") == 0)
			{
					Modus_Messen=0;
					Messinterruptoff();						// Messinterrupt stoppen
					Modus_Test=1;
					Modus=1;
					AnzahlTest=0;
			}






		uart_str_complete_3 = 0;
		}

}

}







/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					Interruptserviceroutine		/ diese koennten in eine C Datei mit dem Namen Interruptserviceroutinen ausgelagert werden				|*/
/*+-------------------------------------------------------------------------------------------------------------+*/
ISR(USART3_RX_vect)
{
	unsigned char nextChar;
	nextChar = UDR3;									// Daten aus dem Puffer lesen

	if( uart_str_complete_3 == 0 )						// mache etwas wenn der String des Bluetoothmoduls
	{													// noch nicht fertig empfangen wurde
		// Daten werden in uart_string_3 geschrieben, bis String-Ende/max Zeichenl�nge erreicht ist
		if((nextChar != '\n') && (nextChar != '\r') && (uart_str_count_3 < UART_MAXSTRLEN))
		{
			uart_string_3[uart_str_count_3] = nextChar;	// naechstes Zeichen in String schreiben
			uart_str_count_3++;							// Stringzaehler um 1 hochzaehlen
		}

		// mache etwas wenn String Ende empfangen oder max Zeichenlaenge erreicht wurde
		else
		{
			uart_string_3[uart_str_count_3] = '\0';		// String Ende mit '\0' markieren
			uart_str_count_3 = 0;						// uart_string_3 Zaehlvariable zuruecksetzen
			uart_str_complete_3 = 1;					// String von Bluetooth fertig empfangen
		}
	}

}

ISR (TIMER0_COMPA_vect)
{
	StartMessung=1;										// Start der Messung/Filterung/Umrechnung
}



ISR (TIMER1_COMPA_vect)
{
	Startausgabe=1;										// Start der UART Ausgabe
}


ISR (INT0_vect)
{
	EIMSK &=~(1<<INT0);								// Externer Interrupt 0 disabled
	if(ZaehlvariableExtInt0==0)						// Z�hlvariable des Externen Interrupt 0
	{
		TCNT3=0;									// Z�hlregister des Timer 3 auf Null setzen
		TCCR3B|= (1<<CS32) | (1<<CS30);				// Timer 3 starten
	}
	else
	{
		TCCR3B &=~((1<<CS32) | (1<<CS30));			// Timer 3 stoppen
		t_Rad=TCNT3+(ZeitueberlaufRad*65535);		// Z�hlregister des Timer 3 plus der Faktor des Timer�berlaufes mal 65535 in die Variable t_Rad schreiben
		TCNT3=0;									// Z�hlregister des Timer 3 auf Null setzen
		TCCR3B |=(1<<CS32) | (1<<CS30);				// Timer 3 starten
		ZeitueberlaufRad=0;							// Faktor des Timer�berlaufes auf Null setzen
		NeuerImpulsRad=1;							// Variable zu 1 setzen weil ein neuer Impuls gekommen ist

	}
	ZaehlvariableExtInt0=1;							// Z�hlvariable des Externen Interrupt 0 auf 1 setzen
	TIMSK3|= (1<<OCIE3A);							// Output Compare Register A Interrupt enabled zum Entprellen des Externen Interrupts


}

ISR (INT1_vect)
{
	EIMSK &=~(1<<INT1);								// Externer Interrupt 1 disabled
	if(ZaehlvariableExtInt1==0)						// Z�hlvariable des Externen Interrupt 1
	{
		TCNT4=0;									// Z�hlregister des Timer 4 auf Null setzen
		TCCR4B|= (1<<CS42) | (1<<CS40);				// Timer 4 starten
	}
	else
	{
		TCCR4B &=~((1<<CS42) | (1<<CS40));			// Timer 4 stoppen
		t_Pedale=TCNT4+(ZeitueberlaufPedale*65535);	// Z�hlregister des Timer 4 plus der Faktor des Timer�berlaufes mal 65535 in die Variable t_Pedale schreiben
		TCNT4=0;									// Z�hlregister des Timer 4 auf Null setzen
		TCCR4B |=(1<<CS42) | (1<<CS40);				// Timer 4 starten
		ZeitueberlaufPedale=0;						// Faktor des Timer�berlaufes auf Null setzen
		NeuerImpulsPedale=1;						// Variable zu 1 setzen weil ein neuer Impuls gekommen ist

	}
	ZaehlvariableExtInt1=1;							// Z�hlvariable des Externen Interrupt 1 auf 1 setzen
	TIMSK4|= (1<<OCIE4A);							// Output Compare Register A Interrupt enabled zum Entprellen des Externen Interrupts


}

ISR (TIMER3_COMPA_vect)
{
	TIMSK3&=~(1<<OCIE3A);							// Output Compare Register A Interrupt disabled
	EIMSK |=(1<<INT0);								// Externer Interrupt 0 enabled
	EIFR |= (1<<INT0);								// Interrupt Flag l�schen
}

ISR (TIMER4_COMPA_vect)
{
	TIMSK4&=~(1<<OCIE4A);							// Output Compare Register A Interrupt disabled
	EIMSK |=(1<<INT1);								// Externer Interrupt 1 enabled
	EIFR |= (1<<INT1);								// Interrupt Flag l�schen
}


ISR(TIMER3_OVF_vect)
{
	ZeitueberlaufRad++;								// Variable hochz�hlen um jeden Timer�berlauf zu detektieren
	TIMSK3	|= (1<<OCIE3A);							// Output Compare Register A Interrupt enabled
	if(ZeitueberlaufRad>=2)							// Wenn der Timer >=2 �berlaufen ist soll die Variable zu 0 gesetzt werden und der Timer gestoppt werden
	{
		ZeitueberlaufRad=0;
		ZaehlvariableExtInt0=0;
		TCCR3B &=~((1<<CS32) | (1<<CS30));
	}
}

ISR(TIMER4_OVF_vect)
{
	ZeitueberlaufPedale++;							// Variable hochz�hlen um jeden Timer�berlauf zu detektieren
	TIMSK4	|= (1<<OCIE4A);							// Output Compare Register A Interrupt enabled
	if(ZeitueberlaufPedale>=2)						// Wenn der Timer >=2 �berlaufen ist soll die Variable zu 0 gesetzt werden und der Timer gestoppt werden
	{
		ZeitueberlaufPedale=0;
		ZaehlvariableExtInt1=0;
		TCCR4B &=~((1<<CS42) | (1<<CS40));
	}
}




/*+-------------------------------------------------------------------------------------------------------------+*/
/*|					Funktionen			 																		|*/
/*+-------------------------------------------------------------------------------------------------------------+*/
//*****************************************************************************************************************
//					Die Funktionen zum Auslesen der ADC Werte sowie der Filterung koennten in eine C Datei mit dem Namen ADC_Messung_Filterung ausgelagert werden
//*****************************************************************************************************************

//*****************************************************************************************************************
//					Spannungswerte auslesen und in Ringspeicher schreiben sowie die Filterung der Werte
//*****************************************************************************************************************

float Messwerte_ADC_Spannung(uint16_t Ordnung_Median)
{
	uint16_t Anzahl;
	uint16_t medgefiltert;
	float gesamtgefiltert;
	if(kSp==0)
	{
		Anzahl= Ordnung_Median;
	}
	else
	{
		Anzahl = 1;
	}

	for(uint16_t i=0;i<Anzahl;i++)
	{
		if(kSp<Ordnung_Median)
		{
			adc_Spannung_Wert=TLC4545_Read(CS3,bit_reso,&PORTB);		// ADC 16 Bit Integer Wert auslesen
			bSp[kSp]=adc_Spannung_Wert;									// aktueller ADC Wert in Array schreiben
			kSp++;
		}
		else
		{	adc_Spannung_Wert=TLC4545_Read(CS3,bit_reso,&PORTB);		// ADC 16 Bit Integer Wert auslesen
			bSp[0]=adc_Spannung_Wert;									// aktueller ADC Wert in Array schreiben
			kSp = 1;
		}
	}

	for(uint16_t c=0;c<Ordnung_Median;c++)								// For Schleife zum kopieren des Arrays
	{
		Spsort[c]=bSp[c];

	}

	medgefiltert=median(Spsort,Ordnung_Median);							// bestimme den Medianwert
	gesamtgefiltert=rekFilter(medgefiltert,rekFilterSpannung);			// Rekursive Filterfunktion aufrufen

	return gesamtgefiltert;
}


//*****************************************************************************************************************
//					Stromwerte auslesen und in Ringspeicher schreiben sowie die Filterung der Werte
//*****************************************************************************************************************

float Messwerte_ADC_Strom(uint16_t Ordnung_Median)
{
	uint16_t Anzahl;
	uint16_t medgefiltert;
	float gesamtgefiltert;

	if(kStrom==0)
	{
		Anzahl= Ordnung_Median;
	}
	else
	{
		Anzahl = 1;
	}

	for(uint16_t i=0;i<Anzahl;i++)
	{
		if(kStrom<Ordnung_Median)
		{
			adc_Strom_Wert=TLC4545_Read(CS4,bit_reso,&PORTB);		// ADC 16 Bit Integer Wert auslesen
			bStrom[kStrom]=adc_Strom_Wert;							// aktueller ADC Wert in Array schreiben
			kStrom++;
		}
		else
		{	adc_Strom_Wert=TLC4545_Read(CS4,bit_reso,&PORTB);		// ADC 16 Bit Integer Wert auslesen
			bStrom[0]=adc_Strom_Wert;								// aktueller ADC Wert in Array schreiben
			kStrom = 1;
		}
	}

	for(uint16_t c=0;c<Ordnung_Median;c++)							// For Schleife zum kopieren des Arrays
	{
		Stromsort[c]=bStrom[c];

	}

	medgefiltert=median(Stromsort,Ordnung_Median);					// bestimme den Medianwert
	gesamtgefiltert=rekFilter(medgefiltert,rekFilterStrom);			// Rekursive Filterfunktion aufrufen


	return gesamtgefiltert;
}


//*****************************************************************************************************************
//					Drehmomentwerte auslesen und in Ringspeicher schreiben sowie die Filterung der Werte
//*****************************************************************************************************************

float Messwerte_ADC_Drehmoment(uint16_t Ordnung_Median)
{
	uint16_t Anzahl;
	uint16_t medgefiltert;
	float gesamtgefiltert;

	if(kDrehm==0)
	{
		Anzahl= Ordnung_Median;
	}
	else
	{
		Anzahl = 1;
	}

	for(uint16_t i=0;i<Anzahl;i++)
	{
		if(kDrehm<Ordnung_Median)
		{
			adc_Moment_Wert=TLC4545_Read(CS1,bit_reso,&PORTH);		// ADC 16 Bit Integer Wert auslesen
			bDrehm[kDrehm]=adc_Moment_Wert;							// aktueller ADC Wert in Array schreiben
			kDrehm++;
		}
		else
		{	adc_Moment_Wert=TLC4545_Read(CS1,bit_reso,&PORTH);		// ADC 16 Bit Integer Wert auslesen
			bDrehm[0]=adc_Moment_Wert;								// aktueller ADC Wert in Array schreiben
			kDrehm = 1;
		}
	}

	for(uint16_t c=0;c<Ordnung_Median;c++)							// For Schleife zum kopieren des Arrays
	{
		Drehmsort[c]=bDrehm[c];

	}

	medgefiltert=median(Drehmsort,Ordnung_Median);					// bestimme den Medianwert
	gesamtgefiltert=rekFilter(medgefiltert,rekFilterMoment);		// Rekursive Filterfunktion aufrufen


	return gesamtgefiltert;
}


//*****************************************************************************************************************
//					Druckwerte auslesen und in Ringspeicher schreiben sowie die Filterung der Werte
//*****************************************************************************************************************

float Messwerte_ADC_Druck(uint16_t Ordnung_Median)
{
	uint16_t Anzahl;
	uint16_t medgefiltert;
	float gesamtgefiltert;

	if(kDruck==0)
	{
		Anzahl= Ordnung_Median;
	}
	else
	{
		Anzahl = 1;
	}

	for(uint16_t i=0;i<Anzahl;i++)
	{
		if(kDruck<Ordnung_Median)
		{
			adc_Druck_Wert=TLC4545_Read(CS2,bit_reso,&PORTB);		// ADC 16 Bit Integer Wert auslesen
			bDruck[kDruck]=adc_Druck_Wert;							// aktueller ADC Wert in Array schreiben
			kDruck++;
		}
		else
		{	adc_Druck_Wert=TLC4545_Read(CS2,bit_reso,&PORTB);		// ADC 16 Bit Integer Wert auslesen
			bDruck[0]=adc_Druck_Wert;								// aktueller ADC Wert in Array schreiben
			kDruck = 1;
		}
	}

	for(uint16_t c=0;c<Ordnung_Median;c++)							// For Schleife zum kopieren des Arrays
	{
		Drucksort[c]=bDruck[c];

	}

	medgefiltert=median(Drucksort,Ordnung_Median);					// bestimme den Medianwert
	gesamtgefiltert=rekFilter(medgefiltert,rekFilterDruck);			// Rekursive Filterfunktion aufrufen


	return gesamtgefiltert;
}

//*****************************************************************************************************************
//					Unterfunktionen zur Filterung der gemessenen Rohwerte
//*****************************************************************************************************************

//Funktion: "quick_sort":
//Quelle:http://rosettacode.org/wiki/Sorting_algorithms/Quicksort#C
void quick_sort (uint16_t *a, uint16_t n)
{
	uint16_t i, j, p, t;

	if (n < 2)
	return;
	p = a[n / 2];

	for (i = 0, j = n - 1;; i++, j--)
	{
		while (a[i] < p)
		i++;

		while (p < a[j])
		j--;

		if (i >= j)
		break;

		t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
	quick_sort(a, i);
	quick_sort(a + i, n - i);
}

// Funktion: "median":
// Ermittelt den Mittleren Wert aus einer Datenreihe
// wenn die Anzahl der Elemente gerade ist,
// wird das Mittel aus den beiden "mittleren" Werten gebildet
uint16_t median(uint16_t a[],uint16_t n)
{
	int16_t med;
	quick_sort(a, n);	// sortiere Elemente nach Gr��e

	if(n % 2 == 0)		// Anzahl der Elemente gerade
	{
		med =(a[n/2]+a[(n/2)-1])/2;
	}

	else				// Anzahl der Elemente ungerade
	{
		med =a[(n-1)/2];
	}
	return med;			// R�ckgeben Medianwert
}

float rekFilter(uint16_t u,float v[])
{
	float AusgangFilter;

	AusgangFilter=0.04*u + 1.6*v[0]-0.64*v[1];
	v[1]=v[0];
	v[0]=AusgangFilter;


	return AusgangFilter;

}

//*****************************************************************************************************************
//					Die nachfolgenden Umrechnungsfunktionen koennten in eine C Datei mit dem Namen Umrechnungen ausgelagert werden
//*****************************************************************************************************************


//*****************************************************************************************************************
//					Umrechnung der gefilterten Spannungswerte
//*****************************************************************************************************************
float UmrechnungSpannung(float adc_Spannung_Wert_gefiltert)
{
	adc_Spannung_Spannung = (adc_Spannung_Wert_gefiltert*Faktor_ADC);		// Integer Wert in Spannung umrechnen
	adc_Spannung_Spannung= (adc_Spannung_Spannung*FaktorSpannungsteiler);	// Umrechnen der Spannung zur Eingangsspannung am Eingang des Spannungsteilers ( ADC_Spannung * (48200/4000)=SpannungEingangSpannungsteiler)

	return adc_Spannung_Spannung;
}

//*****************************************************************************************************************
//					Umrechnung der gefilterten Stromwerte
//*****************************************************************************************************************
float UmrechnungStrom(float adc_Strom_Wert_gefiltert)
{
	adc_Strom_Spannung = (adc_Strom_Wert_gefiltert*Faktor_ADC);				// Integer Wert in Spannung umrechnen
	adc_Strom=((adc_Strom_Spannung-VrefLEM)*FaktorSpannungStrom);			// Umrechnung des Spannungswertes in einen Strom der durch den LEM Wandler flie�t ( (adc_Spannung - VrefLEM)*21,7391304 A/V=StromLem )
	return adc_Strom;
}

//*****************************************************************************************************************
//					Umrechnung der gefilterten Drehmomentwerte
//*****************************************************************************************************************
float UmrechnungDrehmoment(float adc_Moment_Wert_gefiltert)
{
	adc_Moment_Spannung = (adc_Moment_Wert_gefiltert*Faktor_ADC);			// Integer Wert in Spannung umrechnen
	adc_Moment=((adc_Moment_Spannung-OffsetDrehmoment)*FaktorSpannungMoment);	// Umrechnung des Spannungswertes in ein Drehmoment ( (adc_Spannung- 2,5V)*250 Nm/V= Drehmoment)

	return adc_Moment;
}

//*****************************************************************************************************************
//					Umrechnung der gefilterten Druckwerte
//*****************************************************************************************************************
float UmrechnungDruck(float adc_Druck_Wert_gefiltert)
{
	adc_Druck_Spannung = (adc_Druck_Wert_gefiltert*Faktor_ADC);				// Integer Wert in Spannung umrechnen
	adc_Druck=((adc_Druck_Spannung-OffsetDruck)*FaktorSpannungDruck);		// Umrechnung der Spannung in einen Differenzdruck (Pa)  ( (adc_Spannung- 2,5V) * 1000 Pa/V = Differenzdruck in Pa)
	adc_Windgeschwindigkeit= sqrt((2*adc_Druck)/DichteLuft);				// Umrechnung des Differenzdrucks in die Windgeschwindigkeit ( V=s*?((s*?p)/p) )

	return adc_Windgeschwindigkeit;
}

//*****************************************************************************************************************
//					Umrechnung der Batteriespannungswerte
//*****************************************************************************************************************
float UmrechnungBatteriespannung(uint16_t adc_Batteriespannung_Wert)
{
	adc_Batteriespannung_Spannung=(adc_Batteriespannung_Wert*Faktor_ADC_Intern);	// Integer Wert in Spannung umrechnen
	adc_Batteriespannung_Spannung=(adc_Batteriespannung_Spannung*FaktorSpannungsteilerBatteriespannung);	// Umrechnen der Spannung zur Eingangsspannung am Eingang des Spannungsteilers( ADC_Spannung * (122000/22000)=SpannungEingangSpannungsteiler)


	return adc_Batteriespannung_Spannung;
}
//*****************************************************************************************************************
//					Unterfunktionen zur Verarbeitung der empfangenen Befehle / diese koennten in eine C Datei mit dem Namen Unterfunktionen_Befehle ausgelagert werden
//*****************************************************************************************************************
void Ausgabeinterrupton()				// Unterfunktion zum Einschalten des Timer1 Interrupts f�r die UART Ausgabe
{
	cli();
	TIMSK1 |= (1<<OCIE1A);
	sei();
}
void Ausgabeinterruptoff()				// Unterfunktion zum Ausschalten des Timer1 Interrupts f�r die UART Ausgabe
{
	cli();
	TIMSK1 &= ~(1<<OCIE1A);
	sei();
}

void TimeinOCR()						// Unterfunktion zum setzen des Timer1 f�r die UART Ausgabe
{
	cli();
	TCCR1B &=~((1<<CS12) | (1<<CS10));
	TCNT1=0;
	OCR1A = UARTTIME;
	TCCR1B |= (1<<CS12) | (1<<CS10);
	sei();
}


void Messinterrupton()
{
	TIMSK0 |= (1<<OCIE0A);				// Interrupt einschalten der bei dem erreichten OCCR0A Wert ausgel�st wird
}

void Messinterruptoff()
{
	TIMSK0 &=~(1<<OCIE0A);				// Interrupt ausschalten der bei dem erreichten OCCR0A Wert ausgel�st wird
}




//*****************************************************************************************************************
//					Unterfunktion zum Berechnen des Radumfanges
//*****************************************************************************************************************

float UmfangRad(float d_zoll)
{
	float u_meter =0;
	u_meter = (d_zoll * 2.54 * 0.01 * Pi );//Umfang in Metern
	return u_meter;
}
