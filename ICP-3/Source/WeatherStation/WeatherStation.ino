#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>
SoftwareSerial esp8266(9, 10);
#include <LiquidCrystal_PCF8574.h>
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

float Vsig;

int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

Adafruit_BMP280 bmp; // I2C
float tempB;
float pressure;
float altitude;

#define SSID "Pixel"     // "SSID-WiFiname" 
#define PASS "123456789"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=JLDS1INR85CS2FJ2"; //change it with your api key like "GET /update?key=Your Api Key"


void setup()
{
  dht.begin();
  pinMode(A0, INPUT);

  pinMode(8, INPUT);
  starttime = millis();

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
  lcd.print(temp);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  lcd.print("Humidity=");
  lcd.print(hum);
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light=");
  lcd.print(light);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  lcd.print("UV=");
  lcd.print(Vsig);
  delay(1000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Concentra=");
  lcd.print(concentration);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  lcd.print("Temp Baro=");
  lcd.print(tempB);
  lcd.print(" *C");
  delay(1000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pressure=");
  lcd.print(pressure);
  lcd.print(" Pa");
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  lcd.print("Altitude = ");
  lcd.print(altitude);
  lcd.print(" m");

  updateWeather();



}

void updateWeather() {

  temp = dht.readTemperature();
  hum = dht.readHumidity();

  light = analogRead(A0);

  int sensorValue;
  long  sum = 0;
  for (int i = 0; i < 1024; i++)
  {
    sensorValue = analogRead(A0);
    sum = sensorValue + sum;
    delay(2);
  }
  sum = sum >> 10;
  Vsig = sum * 4980.0 / 1023.0; // Vsig is the value of voltage measured from the SIG pin of the Grove interface
  Serial.print("The voltage value: ");
  Serial.print(Vsig);
  Serial.print(" mV    --   ");

  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;
  if ((millis() - starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0);
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
    Serial.print("Concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
  }

  tempB = bmp.readTemperature();
  pressure= bmp.readPressure();
  altitude= bmp.readAltitude(1013.25);

  
  String WeaCmd = "AT+CIPSTART=\"TCP\",\"";
  WeaCmd += IP;
  WeaCmd += "\",80";
  Serial.println(WeaCmd);
  esp8266.println(WeaCmd);
  if (esp8266.find("Error")) {
    return;
  }
  WeaCmd = msg ;
  WeaCmd += "&field1=";
  WeaCmd += temp;

  WeaCmd += "&field2=";
  WeaCmd += hum;

  WeaCmd += "&field3=";
  WeaCmd += light;

  WeaCmd += "&field4=";
  WeaCmd += Vsig;

  WeaCmd += "&field5=";
  WeaCmd += concentration;

  WeaCmd += "&field6=";
  WeaCmd += tempB;
  WeaCmd += "&field7=";
  WeaCmd += pressure;
  WeaCmd += "&field8=";
  WeaCmd += altitude;

  WeaCmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(WeaCmd.length());
  esp8266.println(WeaCmd.length());
  Serial.print(WeaCmd);
  esp8266.print(WeaCmd);
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
