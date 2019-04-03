#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 3, TXPin = 4; /*connect the board's RX to Pin 3, TX to Pin 4 on Ardunino Uno*/
static const uint32_t GPSBaud = 9600; /*GY-NEO6MV2 requires 9600, if using a GPS module with a diffrent boud rate change it.*/

TinyGPSPlus gps; /*TingGPS++ library object*/
SoftwareSerial ss(RXPin, TXPin); /*TinyGPS++ requires serial connection to the GPS device*/

void setup() {
  
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop() {
  
  /* isValid() can be used to check if the object has a valid value received from GPS. 
  isUpdated() also indicates that objects value has a valid value and been updated 
  after you queried it (can be updated with the same value) */
  
  if (gps.satellites.isValid())
  {
    
    Serial.print(F("Number of Satellites: "));
    
    Serial.println(gps.satellites.value(), 5);
  
  }

  if (gps.location.isValid())
  {
    
    Serial.print(F("Latitude: "));
    
    Serial.println(gps.location.lat(), 6); /*Latitude in double degrees*/

    Serial.print(F("Longitude: "));

    Serial.println(gps.location.lat(), 6);/*Longitude in double degrees*/
  
  }

  if (gps.altitude.isValid())
  {
    
    Serial.print(F("Altitude: "));
    
    Serial.println(gps.altitude.meters()); /* Altitude in centimeters*/
  
  }

  if (gps.course.isValid())
  {
    
    Serial.print(F("Course: "));
    
    Serial.println(gps.course.deg()); /* Course in double degrees*/
  
  }

  if (gps.speed.isValid())
  {
    
    Serial.print(F("Speed: "));
    
    Serial.println(gps.speed.mps()); /* Speed in double meters per second*/
  
  }

  if (gps.date.isValid())
  {

    Serial.print(F("Date (Year/Month/Day): "));

    Serial.print(gps.date.year()); /*Year in 2000+*/

    Serial.print(F("/"));
    
    Serial.print(gps.date.month()); /*Month in 1-12*/
    
    Serial.print(F("/"));
    
    Serial.print(gps.date.day()); /*Day in 1-31*/
    
  }

  if (gps.time.isValid())
  {
    
    if (gps.time.hour() < 10) Serial.print(F("0"));
    
    Serial.print(gps.time.hour()); /*Hour in 0-24*/
    
    Serial.print(F(":"));
   
    if (gps.time.minute() < 10) Serial.print(F("0"));
    
    Serial.print(gps.time.minute()); /*Minute in 0-59*/
    
    Serial.print(F(":"));
    
    if (gps.time.second() < 10) Serial.print(F("0"));
    
    Serial.print(gps.time.second()); /*Second in 0-59*/
    
    Serial.print(F("."));
   
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    
    Serial.print(gps.time.centisecond()); /*Centisecond in 0-99*/
    
  }

}
