

int led = 8;
char val;

void setup()
{

pinMode(led, OUTPUT);  // Digital pin 13 set as output Pin
Serial.begin(9600);
}

void loop()
{
  while (Serial.available() > 0)
  {
  val = Serial.read();
  Serial.println(val);
  }

  if( val == '1') // Forward
    {
     
      digitalWrite(led, HIGH);
    }
  else if(val == '2') // Backward
    {
 
      digitalWrite(led, LOW);
    }

   
}
