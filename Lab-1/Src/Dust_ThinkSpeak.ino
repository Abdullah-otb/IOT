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


#define SSID "Pixel"     // "SSID-WiFiname" 
#define PASS "123456789"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=JLDS1INR85CS2FJ2"; //change it with your api key like "GET /update?key=Your Api Key"

int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup()
{

  pinMode(8,INPUT);
  starttime = millis();   
  dht.begin();

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
  lcd.print("Dust Concentration=");

  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2

  lcd.print(concentration);
  delay(1000);

  updateDust();


  delay(1000);
}

void updateDust() {

duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
    Serial.print("Concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
  }

  String DustCmd = "AT+CIPSTART=\"TCP\",\"";
  DustCmd += IP;
  DustCmd += "\",80";
  Serial.println(DustCmd);
  esp8266.println(DustCmd);
  delay(2000);
  if (esp8266.find("Error")) {
    return;
  }
  DustCmd = msg ;
  DustCmd += "&field5=";
  DustCmd += concentration;


  DustCmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(DustCmd.length());
  esp8266.println(DustCmd.length());
  Serial.print(DustCmd);
  esp8266.print(DustCmd);
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
