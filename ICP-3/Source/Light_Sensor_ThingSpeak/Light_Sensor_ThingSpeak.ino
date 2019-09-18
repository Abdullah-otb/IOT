#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial esp8266(9, 10);
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
#include <DHT.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value

int light;

#define SSID "Pixel"     // "SSID-WiFiname" 
#define PASS "123456789"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=JLDS1INR85CS2FJ2"; //change it with your api key like "GET /update?key=Your Api Key"


void setup()
{
  dht.begin();
  pinMode(A0, INPUT);
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
  delay (100);
  lcd.print("Light=");
  lcd.print(light);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  delay(1000);

  updateLight();

  delay(1000);
}

void updateLight() {
  light = analogRead(A0);
  Serial.println("Light = " + String(light));
  String LightCmd = "AT+CIPSTART=\"TCP\",\"";
  LightCmd += IP;
  LightCmd += "\",80";
  Serial.println(LightCmd);
  esp8266.println(LightCmd);
  delay(2000);
  if (esp8266.find("Error")) {
    return;
  }
  LightCmd = msg ;
  LightCmd += "&field1=";
  LightCmd += temp;

  LightCmd += "&field3=";
  LightCmd += light;

  LightCmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(LightCmd.length());
  esp8266.println(LightCmd.length());
  Serial.print(LightCmd);
  esp8266.print(LightCmd);
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
