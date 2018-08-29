/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h> 

//#define USE_SERIAL 

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
TinyGPS gps;
SoftwareSerial gpsModule(2, 3);

// mock gps data - will be replaced with data from the  gps soon
const char str1[] PROGMEM = "$GPRMC,201547.000,A,3014.5527,N,09749.5808,W,0.24,163.05,040109,,*1A";
const char str2[] PROGMEM = "$GPGGA,201548.000,3014.5529,N,09749.5808,W,1,07,1.5,225.6,M,-22.5,M,18.8,0000*78";
const char str3[] PROGMEM = "$GPRMC,201547.000,A,3014.5527,N,09749.5808,W,0.24,163.05,040109,,*1A\r\n";//$GPGGA,201548.000,3014.5529,N,09749.5808,W,1,07,1.5,225.6,M,-22.5,M,18.8,0000*78\r\n";

const char* monthTable[12] ={"JAN","FEB","MAR","APR","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};

void show_gps_data(float latitude, float longitude, char* date);

void setup()   {              
#ifdef USE_SERIAL
  // also send the data on the serial port  
  Serial.begin(115200);
  Serial.println("MiniGPS v1.0");
  Serial.println("by N.Tsakonas (2018)");
  Serial.println();
#endif
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
// both coordinates are formatted with fixed width for fitting the screen
static void formatCoordinate(float value, char* buffer)
{
  int width = 8, precision = 4;
  if (value == TinyGPS::GPS_INVALID_F_ANGLE)
  {
    memset(buffer, '-' , width);
    buffer[width] = 0;
  }
  else
  {
    dtostrf(value, width, precision, buffer);
  }
}

// formats the latest valid date and time into the buffer 
void formatDate(char* buffer)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
  {
    memset(buffer, '-' , 8);
    buffer[8] = 0;
  }else
  {
    sprintf(buffer, "%02d%s%04d %02d:%02d:%02d", day, monthTable[month-1], year, hour, minute, second);
  }
}

  // todo : calculate QTH locator
void getLocator(float latitude, float longitude, char* buffer)
{
  memcpy(buffer,"IO91wa34", 8);
  buffer[8] = 0;
}

// shows the current gps data on screen and also sends them to the serial port
void get_gps_data()
{
  float latitude, longitude;
  unsigned long age;
  gps.f_get_position(&latitude, &longitude, &age);
  char dateBuffer[24];
  formatDate(dateBuffer);
  show_gps_data(latitude, longitude, dateBuffer);  
}

void show_gps_data(float latitude, float longitude, char* dateTime)
{  
  char buffer[24];
  formatCoordinate(fabs(latitude), buffer);
  strcat(buffer, latitude > 0.0 ? "N":"S");

  display.clearDisplay();
  // display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("T");display.println(buffer);
  #ifdef USE_SERIAL
  Serial.print("Latitude : ");Serial.println(buffer);
  #endif
  formatCoordinate(fabs(longitude), buffer);
  strcat(buffer, longitude > 0.0 ? "E":"W");
  
  display.print("N");display.println(buffer);
  #ifdef USE_SERIAL
  Serial.print("Longitude: ");Serial.println(buffer);
  #endif

  getLocator(latitude, longitude, buffer);
  display.print("L ");display.println(buffer);
  #ifdef USE_SERIAL
  Serial.print("Locator  : ");Serial.println(buffer);
  #endif
  display.setTextSize(1);
  display.println(dateTime);
  
  #ifdef USE_SERIAL
  Serial.print("Date/Time: ");Serial.println(dateTime);
  Serial.println("----------");
  #endif
  display.display();
}

/*
// this was used with hardcoded nmea messages for testing
void feed_gps_data(const PROGMEM char *str)
{
  while (true)
  {
    char c = pgm_read_byte_near(str++);
    if (!c) break;
    gps.encode(c);
  }
  gps.encode('\r');
  gps.encode('\n');
}

void loop() {
  feed_gps_data(str1);
  get_gps_data();  
  delay(1000);  
  feed_gps_data(str2);
  get_gps_data();  
  delay(1000);  
}

*/

char skata[64];
int index=0;
int rdIndex=0;
int gps_module_read()
{
    byte c = pgm_read_byte_near(str3 + rdIndex);
    //byte c = str3[rdIndex];
    rdIndex++;
    if (c==0)
    {
      rdIndex=0;
    }
    return c;
}


bool feedgps()
{
  while (gpsModule.available()) //1
  {
    //Serial.println("have data from gps");
    //Serial.println(gpsModule.read());
    char c = gpsModule.read();//gps_module_read();
    
    // put here the debu

    // if (gps.encode(gpsModule.read())) 
    //if (c ==0)
    //  return false;

    if (gps.encode(c)) 
    {
     return true;
    }
  }  
  return false;
}

// the debug
 /*
    skata[index]= c;
    if (index == 60 || skata[index] == '\r'|| skata[index] == '\n')
    {
      skata[index+1] = 0;
      Serial.print(skata);
      index = 0;
    }else{
      index = (index + 1) % 64;
    }
    */
   
void loop()
{
  bool newdata = false;
  unsigned long start = millis();
  while(millis() - start < 1000){    
    if (feedgps()){
      newdata = true;
    }
  }
  if (newdata)
  { 
    get_gps_data();
  }
}


/*

$GPRMC,,V,,,,,,,,,,N*53


$GPVTG,,,,,,,,,N*30


$GPGGA,,,,,,0,00,99.99,,,,,,*48


$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30


$GPGLL,,,,,,V,N*64
*/