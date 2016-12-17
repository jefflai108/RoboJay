int pinAIN1 = 6; //Direction
int pinAIN2 = 5; //Direction
int pinPWMA = 3; //Speed
int pinSTBY = 9; //Standby

void setup() {
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinSTBY, OUTPUT);
}

void loop() {
  digitalWrite(pinSTBY, HIGH);
  digitalWrite(pinAIN1, HIGH);
  digitalWrite(pinAIN2, LOW);
  analogWrite(pinPWMA, 225);

}
