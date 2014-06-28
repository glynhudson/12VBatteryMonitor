/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SD.h>
#include <stdio.h>
#include <DS1302.h>
#include <JeeLib.h>


ISR(WDT_vect) { Sleepy::watchdogEvent(); } 

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 5;

const int kCePin   = 6;  // Chip Enable
const int kIoPin   = 4;  // Input/Output
const int kSclkPin = 3;  // Serial Clock

const int greenLEDpin=2;        //LED file system / SD card syncing activity 
const int ADC_PIN=1;         //ADC1

const int LOG_INTERVAL=2000; // mills between entries
long unsigned int last_log;
//long unsigned int log_time=300000; //log every 5min
long unsigned int log_time=90000; //log every 5min

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

int charge; 

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

void setup()
{
  
  pinMode(greenLEDpin, OUTPUT); //set SD card activity LED pin to output mode
  digitalWrite(greenLEDpin,HIGH);
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
 
  Serial.print("?B1F");    // set backlight to zero
   delay(1000);                // pause to allow LCD EEPROM to program
    
    // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  //rtc.writeProtect(false);
  rtc.writeProtect(true);
  //rtc.halt(false);

  // Make a new time object to set the date and time.
  //Time t(2014, 6, 27, 22, 50, 00, Time::kFriday);

  // Set the time and date on the chip.
 //rtc.time(t);
  


  //Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  Serial.print("?c0");                  // turn cursor off
  Serial.print("?f"); //clear LCD
  delay(500);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed");
  }
  Serial.println("card initialized.");
Serial.print("?n"); // new line 


delay(1000);
digitalWrite(greenLEDpin,LOW);
}

void loop()
{
  
  
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d", t.yr, t.mon, t.date, t.hr, t.min);
  
  
 int voltage = (analogRead(ADC_PIN))* 0.161290323;   //890k on top and 470k below - 16.5V = 1023 ADC
    // 16.5/1023 = 0.016129032 *10

  
  float volts=voltage/10.0;
  
  if (volts>12.7) charge = 100; 
  if ((volts < 12.7) && (volts > 12.58)) charge = 90;
 if ((volts < 12.58) && (volts > 12.46)) charge = 80;
   if ((volts < 12.46) && (volts > 12.36)) charge = 70;
    if ((volts < 12.36) && (volts > 12.28)) charge = 60;
     if ((volts < 12.28) && (volts > 12.2)) charge = 50;
      if ((volts < 12.2) && (volts > 12.12)) charge = 40;
       if ((volts < 12.12) && (volts > 12.04)) charge = 30;
        if ((volts < 12.04) && (volts > 11.98)) charge = 20;
         if ((volts < 11.98) && (volts > 11.94)) charge = 10;
          if ((volts < 11.94))  charge = 0;
  
  // make a string for assembling the data to log:
  String dataString = "";
   dataString += String(voltage);
   dataString += ","; 
   dataString += buf;


 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  
  if (millis()> (last_log+log_time))
  {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
    last_log=millis();
  }  
  }


Serial.print("?f"); //clear LCD
//delay(100);
//Serial.print("?a"); // home curser 
Serial.print(voltage/10.0); Serial.println("V "); Serial.print(charge); Serial.print("%");
Serial.print("?n");
Serial.print(buf);
  //digitalWrite(greenLEDpin,HIGH);
  //delay(200);
    //digitalWrite(greenLEDpin,LOW);
delay(200);
   Sleepy::loseSomeTime(5*1000);
   //delay(5000);
}











