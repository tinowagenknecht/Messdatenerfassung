#include "DiagnoseTest.h"
#include "main.h"


//*****************************************************************************************************************
//					Diagnosefunktion der Spannungsmessung
//*****************************************************************************************************************

void DiagnoseSpannungsmessung (float adc_Spannung_Spannung)
{
	if(adc_Spannung_Spannung<0 || adc_Spannung_Spannung>obereMessbereichsgrenzeSpannung )
	{
		Diagnosestatus|=(1<<2);						// 2. Bit der Variable setzen								
	}
	else
	{
		Diagnosestatus&=~(1<<2);					// 2. Bit der Variable rücksetzen
	}
}


//*****************************************************************************************************************
//					Diagnosefunktion der Strommessung
//*****************************************************************************************************************

void DiagnoseStrommessung (float adc_Strom)
{
	if(adc_Strom< untereMessbereichsgrenzeStrom || adc_Strom> obereMessbereichsgrenzeStrom )
	{
		Diagnosestatus|=(1<<3);						// 3. Bit der Variable setzen							
	}
	else
	{
		Diagnosestatus&=~(1<<3);					// 3. Bit der Variable rücksetzen
	}
}


//*****************************************************************************************************************
//					Diagnosefunktion der Drehmomentmessung
//*****************************************************************************************************************

void DiagnoseDrehmomentmessung (float adc_Moment)
{
	if(adc_Moment<untereMessbereichsgrenzeMoment || adc_Moment>obereMessbereichsgrenzeMoment)
	{
			Diagnosestatus|=(1<<0);					// 0. Bit der Variable setzen				
	}
	else
	{
		Diagnosestatus&=~(1<<0);					// 0. Bit der Variable rücksetzen
	}
}


//*****************************************************************************************************************
//					Diagnosefunktion der Windgeschwindigkeitsmessung
//*****************************************************************************************************************

void DiagnoseWindgeschwindigkeitsmessung (float adc_Windgeschwindigkeit)
{
	if(adc_Windgeschwindigkeit<0 || adc_Windgeschwindigkeit>obereMessbereichsgrenzeWindgeschwindigkeit )
	{
		Diagnosestatus|=(1<<1);						// 1. Bit der Variable setzen				
	}
	else
	{
		Diagnosestatus&=~(1<<1);					// 1. Bit der Variable rücksetzen
	}
}

//*****************************************************************************************************************
//					Testfunktion zum Testen der Datenübertragung mittels UART
//*****************************************************************************************************************
void TestUebertragung()
{
	
		adc_Moment=50+AnzahlTest; 					// Testwert zum Test der Datenübertragung berechnen
		adc_Windgeschwindigkeit=5+AnzahlTest; 		// Testwert zum Test der Datenübertragung berechnen
		Pedalgeschwindigkeit=10+AnzahlTest;
		Fahrradgeschwindigkeit=10+AnzahlTest;
		adc_Batteriespannung_Spannung=AnzahlTest;	// Testwert zum Test der Datenübertragung berechnen
		adc_Spannung_Spannung=10+AnzahlTest;		// Testwert zum Test der Datenübertragung berechnen
		adc_Strom=AnzahlTest;						// Testwert zum Test der Datenübertragung berechnen
	
	if(Diagnosestatus==1)
	{
		Diagnosestatus=0;							// Testwert zum Test der Datenübertragung
	}
	else
	{
		Diagnosestatus=1;							// Testwert zum Test der Datenübertragung 
	}
}