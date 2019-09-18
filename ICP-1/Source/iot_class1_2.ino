int dlay = 500;
void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop(){
  digitalWrite(3,1);
  delay(dlay*5);
  digitalWrite(3,0);
  digitalWrite(5,1);
  delay(dlay*4);
  digitalWrite(5,0);
  digitalWrite(4,1);
  delay(dlay*2);
  digitalWrite(4,0);
  


}
