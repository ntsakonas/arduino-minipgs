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


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
TinyGPS gps;
SoftwareSerial gpsModule(2, 3);

// mock gps data - will be replaced with data from the  gps soon
const char str1[] PROGMEM = "$GPRMC,201547.000,A,3014.5527,N,09749.5808,W,0.24,163.05,040109,,*1A";
const char str2[] PROGMEM = "$GPGGA,201548.000,3014.5529,N,09749.5808,W,1,07,1.5,225.6,M,-22.5,M,18.8,0000*78";
const char str3[] PROGMEM = "$GPRMC,201547.000,A,3014.5527,N,09749.5808,W,0.24,163.05,040109,,*1A\r\n$GPGGA,201548.000,3014.5529,N,09749.5808,W,1,07,1.5,225.6,M,-22.5,M,18.8,0000*78\r\n";
//const char str3[] PROGMEM = "$GPRMC,,V,,,,,,,,,,N*53\r\n$GPGGA,,,,,,0,00,99.99,,,,,,*48\r\n";

//===============================================

const char* monthTable[12] ={"JAN","FEB","MAR","APR","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
void setup()   {                
  Serial.begin(115200);
  Serial.println("MiniGPS v0.1");
  Serial.println("by N.Tsakonas (2018)");
  Serial.println();

  gpsModule.begin(9600);


  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.setTextColor(WHITE);


  /*
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("T 51.5062N");
  display.println("N  0.0859W");
  display.println("IO91wo67");
  display.setTextSize(1);
  display.println("28AUG2018 140259");
  display.display();
  delay(15000);*/
} 

static void formatFloat(float val, float invalid, int len, int prec, char* buffer)
{
  if (val == invalid)
  {
    memset(buffer, '-',len);
    buffer[len] = 0;
  }
  else
  {
    dtostrf(val, len, prec, buffer);
  }
}

void formatDate( char* buffer)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
  {
    strcpy(buffer, "------");
  }else
  {
    sprintf(buffer, "%02d%s%04d %02d:%02d:%02d", day, monthTable[month-1], year, hour, minute, second);
  }
}


void get_gps_data()
{
  float latitude, longitude;
  unsigned long age;
  gps.f_get_position(&latitude, &longitude, &age);
  char buffer[32];
  formatFloat(fabs(latitude), TinyGPS::GPS_INVALID_F_ANGLE, 8, 4,buffer);
  strcat(buffer,latitude >0.0 ? "N":"S");

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("T");display.println(buffer);
  
  Serial.print("Latitude :");Serial.println(buffer);

  formatFloat(fabs(longitude), TinyGPS::GPS_INVALID_F_ANGLE, 8, 4,buffer);
  strcat(buffer,longitude >0.0 ? "E":"W");
  
  display.print("N");display.println(buffer);
  Serial.print("Longitude:");Serial.println(buffer);

  display.println("IO91wo67");
 
  formatDate(buffer);
  
  display.setTextSize(1);
  display.println(buffer);
  
  Serial.print("Date/Time:");Serial.println(buffer);
  display.display();
}

// this was used with hardcoded nmea messages

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

void gpsdump()
{
  get_gps_data();  
}

void loop() {
  feed_gps_data(str1);
  gpsdump();  
  delay(5000);  
  feed_gps_data(str2);
  gpsdump();  
  delay(5000);  
}


/*
char skata[64];
int index=0;
int rdIndex=0;
int gps_module_read()
{
    byte c = pgm_read_byte_near(str3 + rdIndex);
    rdIndex++;
    if (c==0){
      rdIndex=0;
    }
    return c;
}


bool feedgps()
{
  while (1)//gpsModule.available())
  {
    //Serial.println("have data from gps");
    //Serial.println(gpsModule.read());
    char c = gps_module_read();
    
    //skata[index]= c;
    //if (index == 60 || skata[index] == '\r'|| skata[index] == '\n')
    //{
    //  skata[index+1] = 0;
    //  Serial.println(skata);
    //  index = 0;
    //}else{
    //index = (index + 1) % 64;
    //}
    //
    // if (gps.encode(gpsModule.read())) 
    if (gps.encode(c)) 
    {
      //Serial.println("have full frame from gps");
     return true;
    }
  }  
  //Serial.println("no full frame from gps");
  return false;
}


void loop()
{
  Serial.println("============ LOOP");

  bool newdata = false;
  unsigned long start = millis();
  // poll the gps module evey 1 sec (that means multiple messages )

  while(millis() - start < 1000){    
 //    Serial.println("polling gps...");
    if (feedgps()){
      newdata = true;
      //break; 
    }
  }
  if (newdata)
  { 
    Serial.println("have data from gps");
    get_gps_data();
  }
}
*/
/*

$GPRMC,,V,,,,,,,,,,N*53


$GPVTG,,,,,,,,,N*30


$GPGGA,,,,,,0,00,99.99,,,,,,*48


$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30


$GPGLL,,,,,,V,N*64
*/