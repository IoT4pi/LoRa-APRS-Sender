// NEU: mit Callausgabe gem. Programmzeile 77 ; hier: OE1KEB 
// NEU: GPS-Eingangspins geaendert auf In:A5, Out:A4  
// NEU: mit #A1-Batteriespannungseinlesung

// http://www.rcgroups.com/forums/showthread.php?t=2341299

/*
/*
iot4pi.com
26/06/2017
added two new functions
recalcEncodedGPS() now you can send encoded GPS and altidude data
recalcGPS() with this function you can send the GPS Data in APRS style 
Works together with LoRa APRS Gateway

*/
/*
**************************************************************************************************
ProMiniLoRaTracker_V1 Programs

Copyright of the author Stuart Robinson - 02/07/2015 15:00

These programs may be used free of charge for personal, recreational and educational purposes only.

This program, or parts of it, may not be used for or in connection with any commercial purpose without
the explicit permission of the author Stuart Robinson.

The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
intended purpose and free from errors.
**************************************************************************************************
*/

//Hardware definitions
const byte lora_PNSS = 10;	//pin number where the NSS line for the LoRa device is connected.
const byte PLED1 = 8;           //pin number for LED on Tracker
const byte lora_PReset = 9;	//pin where LoRa device reset line is connected
const byte lora_PPWMCH = 3;    //pin number for tone generation, connects to LoRa device DIO2.
static const int RXPin = A5, TXPin = A4;  //pins for soft serial
static const uint32_t GPSBaud = 9600; //GPS

String InputString = "";     //data on buff is copied to this string
String Outputstring = "";
String outString="";         //The new Output String with GPS Conversion RAW
String Tcall="OE1KEB-12";   //your Call Sign
String sSymbol=">";         //Symbol Code
float BattVolts; 
//char charVal[20];

#include <SPI.h>
#include "LoRaTX.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <math.h>

TinyGPSPlus gps;   // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);   // The serial connection to the GPS device


///////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
      displayInfo();
    }

  if (millis() > 200000 && gps.charsProcessed() < 10)
  {
    digitalWrite(PLED1, HIGH);
    Serial.println("No GPS detected!");
    while (true);
  }

}
//////////////////////////////////////////////////////////////////////////////////////
//@APA calc encoded GPS Position
void recalcEncodedGPS(){
  Serial.println("------------------Encode Start---");
  String Ns, Ew;
  float Tlat, Tlon;
  int Talt;
  long Lat;
  long Lon;
  int Alt1, Alt2;
  /*
  Serial.print("Lat is ");
  Serial.println(gps.location.lat(),5);
  Serial.print("Lon is ");
  Serial.println(gps.location.lng(),5);
  */
  Tlat=gps.location.lat();
  Tlon=gps.location.lng();
  Talt=gps.altitude.feet();
  Serial.print("Tlat is ");
  Serial.println(Tlat,5);
  Serial.print("Tlon is ");
  Serial.println(Tlon,5);
  Serial.print("Talt is ");
  Serial.println(Talt);
  if(Tlat<0) { Ns = "S"; } else { Ns = "N"; } 
  if(Tlon<0) { Ew = "W"; } else { Ew = "E"; } 
  Lat=380926*(90-Tlat);
  Lon=190463*(180+Tlon);
  /*
  Serial.print("Lat is ");
  Serial.println(Lat);
  Serial.print("Lon is ");
  Serial.println(Lon);
  */
  outString = (Tcall);
  outString += ">APRS:!";
  outString +="/";
  //Calc Alt
  if(Talt>0){
    double ALT=log(Talt)/log(1.002);
    //Serial.println(ALT);
    Alt1= ALT/91;
    Alt2=(int)ALT%91;
  }else{
    Alt1=0;
    Alt2=0;
  }
  //Calc Lan
  int ilanFirst=Lat/pow(91,3);
  //Serial.println(ilanFirst);
  long iTemp=Lat%(long)pow(91,3);
  //Serial.println(iTemp);
  int ilanSecond=iTemp/pow(91,2);
  //Serial.println(ilanSecond);
  iTemp=Lat%(long)pow(91,2);
  //Serial.println(iTemp);
  int ilanThird=iTemp/91;
  //Serial.println(ilanThird);
  int ilanFour=Lat%(long)91;
  //Serial.println(ilanFour);
  /*
  Serial.print("Lan = ");
  Serial.print(char(ilanFirst+33));
  Serial.print(char(ilanSecond+33));
  Serial.print(char(ilanThird+33));
  Serial.println(char(ilanFour+33));
  */
  outString +=char(ilanFirst+33);
  outString +=char(ilanSecond+33);
  outString +=char(ilanThird+33);
  outString +=char(ilanFour+33);
  

  //Calc Lon
  int ilonFirst=Lon/pow(91,3);
  //Serial.println(ilonFirst);
  iTemp=Lon%(long)pow(91,3);
  //Serial.println(iTemp);
  int ilonSecond=iTemp/pow(91,2);
  //Serial.println(ilonSecond);
  iTemp=Lon%(long)pow(91,2);
  //Serial.println(iTemp);
  int ilonThird=iTemp/91;
  //Serial.println(ilonThird);
  int ilonFour=Lon%(long)91;
  /*Serial.println(ilonFour);
  Serial.print("Lon = ");
  Serial.print(char(ilonFirst+33));
  Serial.print(char(ilonSecond+33));
  Serial.print(char(ilonThird+33));
  Serial.println(char(ilonFour+33));
  */
  outString +=char(ilonFirst+33);
  outString +=char(ilonSecond+33);
  outString +=char(ilonThird+33);
  outString +=char(ilonFour+33);
  outString +=sSymbol;
  //outString +=" ";  //No Spped and Course
  outString +=char(Alt1+33); //Altidude 1
  outString +=char(Alt2+33); //Altidude 2
  outString +=char(0x30+33);
  //outString += " /A=";    
  //outString += Talt; 
  //outString += "m Batt=";
  outString += "Batt="; 
  outString += String(BattVolts,2); 
  outString += ("V");

  Serial.println("------------------Encode End---");
}

//////////////////////////////////////////////////////////////////////////////////////
//@APA Recalc GPS Position
void recalcGPS(){
  
  String Ns, Ew;
  float Tlat, Tlon;
  int Talt;
  float Lat;
  float Lon;
  /*
  Serial.print("Lat is ");
  Serial.println(gps.location.lat(),5);
  Serial.print("Lon is ");
  Serial.println(gps.location.lng(),5);
  */
  Tlat=gps.location.lat();
  Tlon=gps.location.lng();
  Talt=gps.altitude.meters();
  Serial.print("Tlat is ");
  Serial.println(Tlat,5);
  Serial.print("Tlon is ");
  Serial.println(Tlon,5);
  Serial.print("Talt is ");
  Serial.println(Talt);
  if(Tlat<0) { Ns = "S"; } else { Ns = "N"; } 
  if(Tlon<0) { Ew = "W"; } else { Ew = "E"; } 
  if(Tlat < 0) { Tlat= -Tlat; }
  unsigned int Deg_Lat = Tlat; 
  Lat = 100*(Deg_Lat) + (Tlat - Deg_Lat)*60; 
  
  if(Tlon < 0) { Tlon= -Tlon; }
  unsigned int Deg_Lon = Tlon; 
  Lon = 100*(Deg_Lon) + (Tlon - Deg_Lon)*60; 
  /*
  Serial.print("Lat is ");
  Serial.println(Lat,5);
  Serial.print("Lon is ");
  Serial.println(Lon,5);
  */
  outString = (Tcall);
  outString += ">APRS:!";
  if(Tlat<10) {outString += "0"; }
  outString += String(Lat,2);    
  outString += Ns;    
  outString += char(92);
  if(Tlon<100) {outString += "0"; }
  if(Tlon<10) {outString += "0"; }
  outString += String(Lon,2); 
  outString += Ew;
  outString +=sSymbol;
  outString += " /A=";    
  outString += Talt; 
  outString += "m Batt="; 
  outString += String(BattVolts,2); 
  outString += ("V");   

  Serial.print("New Position ");
  Serial.println(outString);
  //Soll
  // OE1KEB-12>APRS:!4802.38N/01617.23E> /A=  0m SNR+9dB RSSI=-61db Batt=3.27V
  //Ist
  //<ï¿½48.03962,16.28727,  0,OE1KEB-12,3.28,*
}

/////////////////////////////////////////////////////////////////////////////////////////
void displayInfo()
{
  //Serial.print("Location: ");
  byte i;
  byte ltemp;
  batt_read();
  Outputstring = "";

  if (gps.location.isValid())
  {
    //New System
    recalcEncodedGPS();
    /*
    Serial.print("Outputstring= ");
    Serial.println(outString);
    delay(10000);
    return;
    
    recalcGPS();
    */
    Outputstring =outString;
    /*
    addtostring(gps.location.lat(), 5, 5, ",");
    addtostring(gps.location.lng(), 5, 5, ",");
// replace ident-call according to your requirements
    addtostring(gps.altitude.meters(), 3, 0, ",OE1KEB-12,");
    addtostring(BattVolts, 4, 2, ",*");
    */
    // at this point Outputstring has the LoRa Telemetry data to send
    Serial.print("OutputString is ");
    Serial.println(Outputstring);
    ltemp = Outputstring.length();
    //Old system
    //lora_SetModem(lora_BW41_7, lora_SF12, lora_CR4_5, lora_Explicit, lora_LowDoptON);		//Setup the LoRa modem parameters
    
    
    //new System
    lora_SetModem(lora_BW125, lora_SF12, lora_CR4_5, lora_Explicit, lora_LowDoptON);    //Setup the LoRa modem parameters
    lora_PrintModem();                    //Print the modem parameters
    lora_TXStart = 0;
    lora_TXEnd = 0;

    for (i = 0; i <= ltemp; i++)
    {
    lora_TXBUFF[i] = Outputstring.charAt(i);
    }
    i--;
    lora_TXEnd = i;
    
    digitalWrite(PLED1, HIGH);       //LED on during packet
    lora_Send(lora_TXStart, lora_TXEnd, 60, 255, 1, 10, 10);	//send the packet, data is in TXbuff from lora_TXStart to lora_TXEnd
    digitalWrite(PLED1, LOW);
    lora_TXPKTInfo();			//print packet information
    lora_TXBuffPrint(0);
    Serial.println();
    delay(20000);
  }
  else
  {
 // at this point Outputstring has the LoRa Telemetry data to send
    digitalWrite(PLED1, HIGH);
    Outputstring = "No GPS-Fix";
    Serial.println(Outputstring);
    
    ltemp = Outputstring.length();
    lora_SetModem(lora_BW125, lora_SF12, lora_CR4_5, lora_Explicit, lora_LowDoptON);    //Setup the LoRa modem parameters
    lora_PrintModem();                    //Print the modem parameters
    lora_TXStart = 0;
    lora_TXEnd = 0;

    for (i = 0; i <= ltemp; i++)
    {
    lora_TXBUFF[i] = Outputstring.charAt(i);
    }
    i--;
    lora_TXEnd = i;

    digitalWrite(PLED1, HIGH);       //LED on during packet
    lora_Send(lora_TXStart, lora_TXEnd, 60, 255, 1, 10, 10);  //send the packet, data is in TXbuff from lora_TXStart to lora_TXEnd
    digitalWrite(PLED1, LOW);
    lora_TXPKTInfo();     //print packet information
    lora_TXBuffPrint(0);
    Serial.println();
    delay(20000);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
void addtostring(double lFloat, byte lmin, byte lprecision, String Stuff) // from dtostrf3
{
  char charVal[10];               //temporarily holds data from vals
  memset(charVal, 0, sizeof(charVal)); //clear array
  InputString = "";
  dtostrf(lFloat, lmin, lprecision, charVal);  //lmin is mininum width, lprecision is precision
  for (int i = 0; i < sizeof(charVal); i++)
  {
    if  (charVal[i] == 0)
    {
      break;
    }
    InputString += charVal[i];
  }
  //return InputString;
  Outputstring = Outputstring + InputString + Stuff;
}

/////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);                       //Serial console ouput
  Serial.println("ProMiniLoRaTracker_V1");
  Serial.println("Stuart Robinson - July 2015");
  Serial.println();
  pinMode(lora_PReset, OUTPUT);			// RFM98 reset line
  digitalWrite(lora_PReset, LOW);		// Reset RFM98
  pinMode (lora_PNSS, OUTPUT);			// set the slaveSelectPin as an output:
  digitalWrite(lora_PNSS, HIGH);
  pinMode(PLED1, OUTPUT);			// for shield LED
  SPI.begin();					// initialize SPI:
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  lora_ResetDev();			//Reset the device
  lora_Setup();				//Do the initial LoRa Setup
  lora_SetFreq(108, 105, 153);		//Set the LoRa frequency, 434.400 Mhz lora_SetFreq(byte lora_LFMsb, byte lora_LFMid, byte lora_LFLsb)
  lora_Tone(1000, 1000, 10);             //Transmit an FM tone, 1000hz, 100ms
  ss.begin(GPSBaud);                          //Startup soft serial for GPS
}

///////////////////////////////////////////////////////////////////////////////////////
void batt_read()
{
  int BattRead = analogRead(A1);
  lora_TXBUFF[1] = (BattRead / 256);                     //MSB of battery volts
  lora_TXBUFF[0] = (BattRead - (lora_TXBUFF[1] * 256));  //LSB of battery volts

  BattVolts = (BattRead * (6.6 / 1023.0)); 

  Serial.print("lora_TXBUFF[0]  ");
  Serial.println(lora_TXBUFF[0]);
  Serial.print("lora_TXBUFF[1]  ");
  Serial.println(lora_TXBUFF[1]);
  Serial.println("Battery ");
  Serial.print(BattVolts, 2);
  Serial.println("V");
}

