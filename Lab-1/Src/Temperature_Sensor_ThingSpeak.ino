#include <SoftwareSerial.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT.h>
#include <LedFlasher.h>
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int blinkRed = 13; 
int blinkGreen = 12;

            

DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

String AP = "Pixel";      
String PASS = "123456789"; 
String API = "JLDS1INR85CS2FJ2";   
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(9,10); 
 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("Group 5");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  
  dht.begin();
}
void loop() {

    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    lcd.clear();

    unsigned long currentMillis = millis();
    if(temp>24){
       lcd.setCursor(0, 0);
       lcd.print("Caution ");
       lcd.setCursor(0, 1); // set the cursor to column 0, line 2
       lcd.print("Temp is high");

        digitalWrite(blinkRed, HIGH);  
        delay(1000);              
        digitalWrite(blinkRed, LOW);    
       }else{
            lcd.display();
            lcd.setCursor(0, 0);
            lcd.print("Temperature=");
            lcd.print(temp);
            lcd.setCursor(0, 1); // set the cursor to column 0, line 2
            lcd.print("Humidity=");
            lcd.print(hum);
            digitalWrite(blinkGreen, HIGH);  
            delay(1000);              
            digitalWrite(blinkGreen, LOW);  
            }
     
 String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+temp+"&"+ field2 +"="+hum;
sendCommand("AT+CIPMUX=1",5,"OK");
sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
esp8266.println(getData);
countTrueCommand++;
sendCommand("AT+CIPCLOSE=0",5,"OK");
    
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");

  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
