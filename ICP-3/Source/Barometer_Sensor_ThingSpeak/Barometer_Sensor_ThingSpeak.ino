#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial esp8266(9, 10);
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <SoftwareWire.h>

// SoftwareWire constructor.
// Parameters:
//   (1) pin for the software sda
//   (2) pin for the software scl
//   (3) use internal pullup resistors. Default true. Set to false to disable them.
//   (4) allow the Slave to stretch the clock pulse. Default true. Set to false for faster code.
//
// Using pin 2 (software sda) and 3 (software scl) in this example.

SoftwareWire myWire( 2, 3);

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value

Adafruit_BMP280 bmp; // I2C

#define SSID "Pixel"     // "SSID-WiFiname" 
#define PASS "123456789"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=JLDS1INR85CS2FJ2"; //change it with your api key like "GET /update?key=Your Api Key"

float tempBaro;
void setup()
{
 

  Serial.println(F("BMP280 test"));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("Group 5");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  Serial.begin(9600); //or use default 115200.
  esp8266.begin(115200);
  Serial.println("AT");
  esp8266.println("AT");
  delay(5000);
  if (esp8266.find("OK")) {
    connectWiFi();
  }
}

void loop() {
  
  lcd.clear();

  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Temperature=");
  lcd.print(tempBaro);
  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  lcd.print("Pressure=");
  lcd.print(bmp.readPressure());

  delay(1000);

   lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Approx altitude ==");
   lcd.setCursor(0, 1);
  lcd.print(bmp.readAltitude(1013.25));
  lcd.print("m");
  delay(1000);

  updateTempHum();


  delay(1000);
}

void updateTempHum() {
tempBaro = bmp.readTemperature();
  String TempCmd = "AT+CIPSTART=\"TCP\",\"";
  TempCmd += IP;
  TempCmd += "\",80";
  Serial.println(TempCmd);
  esp8266.println(TempCmd);
  if (esp8266.find("Error")) {
    return;
  }
  TempCmd = msg ;
  TempCmd += "&field6=";
  TempCmd += tempBaro;

  TempCmd += "&field7=";
  TempCmd += bmp.readPressure();

    TempCmd += "&field8=";
  TempCmd += bmp.readAltitude(1013.25);

  TempCmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(TempCmd.length());
  esp8266.println(TempCmd.length());
  Serial.print(TempCmd);
  esp8266.print(TempCmd);

   delay(2000);
}

boolean connectWiFi() {
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(5000);
  if (esp8266.find("OK")) {
    Serial.println("OK");
    return true;
  } else {
    return false;
  }
}
