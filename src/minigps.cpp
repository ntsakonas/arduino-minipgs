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
// SoftwareSerial gpsModule(2, 3);
// swapped because it was easier on the board
SoftwareSerial gpsModule(3, 2);

const char* monthTable[12] ={"JAN","FEB","MAR","APR","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
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
// both coordinates are formatted with fixed width for fitting the screen
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
    sprintf(buffer, "%02d%s%04d %02d:%02d:%02d", day, monthTable[month-1], year, hour, minute, second);
}

  // todo : calculate QTH locator
void getLocator(float latitude, float longitude, char* buffer)
{
   strcpy(buffer, INVALID_DATA);
}

// shows the current gps data on screen and also sends them to the serial port
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
  display.print("T");display.println(buffer);
  // show longitude
  formatCoordinate(fabs(longitude), buffer);
  strcat(buffer, longitude > 0.0 ? "E":"W");
  display.print("N");display.println(buffer);
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