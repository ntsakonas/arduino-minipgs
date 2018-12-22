// Copyright 2018, Nick Tsakonas
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// this mini project implements a pocket size portable GPS.
// it uses a low cost Ublox NEO-7M GPS module an arduino nano and an OLED screen
// (see project description for schematic)
// only the basic information are displayed on the screen (Latitude, Longitude, Date and Time and Maidenhead locator)

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h> 

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
TinyGPS gps;
SoftwareSerial gpsModule(3, 2);

const char* LOCATOR_CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWX";
const char* INVALID_DATA = "--------";
void setup()   {
  // initialise gps module
  gpsModule.begin(9600);
  // initialise OLED screen
  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS); 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("MiniGPS v1.0");
  display.println("N.Tsakonas (2018)");
  delay(2000);
  display.println("Waiting for satellites...");
  display.display();
} 

// format a coordinate (either latitude or longitude) into the buffer
// both coordinates are formatted with fixed width for fitting on the screen
static void formatCoordinate(float value, char* buffer)
{
  int width = 8, precision = 4;
  if (value == TinyGPS::GPS_INVALID_F_ANGLE)
    strcpy(buffer, INVALID_DATA);
  else
    dtostrf(value, width, precision, buffer);
}

// formats the latest valid date and time into the buffer 
void formatDate(char* buffer)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
     strcpy(buffer, INVALID_DATA);
  else
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", day, month, year, hour, minute, second);
}

// convert lat/lon to a Maidenhead locator 
void getLocator(float latitude, float longitude, char* buffer)
{
    if (latitude == TinyGPS::GPS_INVALID_F_ANGLE || longitude == TinyGPS::GPS_INVALID_F_ANGLE)
    {
     strcpy(buffer, INVALID_DATA);
     return;
    }
    // normalise lon to south and lat to antimeridian of greenwich 
    // (this normalisation gives the Prime Meridian a false easting of 180 degrees
    // and the equator a false northing of 90 degrees.)
    double normalisedLatitude  = latitude + 90.0;
    double normalisedLongitude = longitude + 180.0;
    // calculate the first pair (the field)
    // this is made from 18 zones - A to R
    int latitudeField = (int)(normalisedLatitude / 10.0);
    int longitudeField = (int)(normalisedLongitude / 20.0);
    // calculate the second pair (the square)
    // this is made from 10 zones - 0 to 9
    int latitudeSquare = (int)(normalisedLatitude - (double)(latitudeField * 10.0));
    int longitudeSquare = (int)((normalisedLongitude - (double)(longitudeField *  20.0)) /2 );
    // calculate the third pair (the sub field)
    // this is made from 10 zones - A to W
    double latitudeMinutes = (normalisedLatitude - (double)(latitudeField * 10.0) - (double)latitudeSquare) * 60.0;
    double longitudeMinutes = (normalisedLongitude - (double)(longitudeField *  20.0) - (double)(longitudeSquare *2)) * 60.0;

    int latitudeSubField = (int)(latitudeMinutes / 2.5);
    int longitudSubField = (int)(longitudeMinutes / 5.0);
    // calculate the fourth pair (the sub square)
    // this is made from 10 zones - A to W
    double latitudeSeconds = (latitudeMinutes - (double)(latitudeSubField * 2.5)) * 60.0;
    double longitudeSeconds = (longitudeMinutes - (double)(longitudSubField * 5.0)) * 60.0;
    int latitudeSubSquare= (int)(latitudeSeconds / 15.0);
    int longitudSubSquare = (int)(longitudeSeconds / 30.0);
    sprintf(buffer,"%c%c%d%d%c%c%d%d", LOCATOR_CHARACTERS[longitudeField],LOCATOR_CHARACTERS[latitudeField],longitudeSquare,latitudeSquare,LOCATOR_CHARACTERS[longitudSubField]+32,LOCATOR_CHARACTERS[latitudeSubField]+32,longitudSubSquare,latitudeSubSquare);
}

// shows the current gps data on screen
void show_gps_data()
{
  float latitude, longitude;
  unsigned long age;
  gps.f_get_position(&latitude, &longitude, &age);
  char buffer[24];

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  // show latitude
  formatCoordinate(fabs(latitude), buffer);
  strcat(buffer, latitude > 0.0 ? "N":"S");
  display.print("\x18");display.println(buffer);
  // show longitude
  formatCoordinate(fabs(longitude), buffer);
  strcat(buffer, longitude > 0.0 ? "E":"W");
  display.print("\x1a");display.println(buffer);
  // show grid locator
  getLocator(latitude, longitude, buffer);
  display.print("L ");display.println(buffer);
  // show date and time
  display.setTextSize(1);
  formatDate(buffer);  
  display.println(buffer);
  display.display();
}

bool read_data_from_gps()
{
  while (gpsModule.available()) 
  {
    if (gps.encode(gpsModule.read())) 
     return true;
  }  
  return false;
}

void loop()
{
  bool hasFreshData = false;
  unsigned long start = millis();
  while(millis() - start < 1000){    
    if (read_data_from_gps()){
      hasFreshData = true;
    }
  }
  if (hasFreshData)
    show_gps_data();
}