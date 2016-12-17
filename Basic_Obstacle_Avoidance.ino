//constant integers:
const int p_fl = 2;
const int p_fr = 3;
const int p_l = 4;
const int p_r = 5;
const int lMotor = 9;
const int rMotor = 10;
const int analogPin = 0;

//variable floating points:
float correctDuration; //time required to turn a given number of degrees
float turnAngle; //calculated # of degrees to turn based on difference b/t fl and fr sensors
float analogInput; //proportional to duty cycle, input for analogWrite()
float dutyCycle; //duty cycle at 9V
float scaledDC; //duty cycle after accounting for battery drain
float fl = 0.0; //most recently found distance to front left sensor
float fr = 0.0; //most recently found distance to front right sensor
float l = 0.0; //most recently found distance to left sensor
float r = 0.0; //most recently found distance to right sensor

//booleans:
boolean frontObstacle = false; //Is a front obstacle detected?
boolean sideObstacle = false; //Is a side obstacle detected?

//temporary
float safeDist = 20.0;

void setup() {
  //pin setup:
  pinMode(lMotor, OUTPUT);
  pinMode(rMotor, OUTPUT);
}

void loop() {
  detectOb(); //look for obstacle
  if (frontObstacle == true) { //if object in front within safety distance
    //pause();
    turn();
  } else if (sideObstacle == true) {
    correct();
  }
  forward(); //move forward
}

//determine if object within safety distance:
void detectOb() {
  pingSensors(); //update sensor readings
  frontObstacle = (fl <= safeDist) || (fr <= safeDist); //true if object in front within safety distance
  sideObstacle = (l <= safeDist) || ( r <= safeDist); //true if object on either side within safety distance
}

//get values from all sensors:
void pingSensors() {
  fl = get_dist(p_fl); //get distance between fl sensor and nearest object
  fr = get_dist(p_fr); //get distance between fr sensor and nearest object
  l = get_dist(p_l); //get distance between l sensor and nearest object
  r = get_dist(p_r); //get distance between r sensor and nearest object
}

//move forward in as close to a straight line as possible:
void forward() {
  dutyCycle = 60.0; //both motors will move at half speed (ideally)
  scaledDC = scaleDutyCycle(dutyCycle); //increase duty cycle if supplied voltage is less than ideal
  analogInput = scaledDC * 255.0 / 100.0; //convert duty cycle to analog input
  analogWrite(lMotor, analogInput / 1.11); //turn left motor at slightly less than calculated input to compensate for difference in motor quality
  analogWrite(rMotor, analogInput); //turn right motor at calculated input
}

//void pause() {
//  clearLCD();
//  LCD_Serial.print("PAUSING");
//  analogWrite(lMotor, 0);
//  analogWrite(rMotor, 0);
//  delay(2000);
//}

//decide which direction to turn based on most recent sensor readings:
void turn() {
  if (l <= safeDist && r <= safeDist) { //if object within safety distance on both sides
    if (r > l) { //if object on right is further away than object on left
      correctToRight(170); //turn right (away from the closer object)
    } else if (l > r) { //if object on left is further away than object on right
      correctToLeft(180); //turn left
    } else { //if objects are equidistant
      correctToRight(170); //turn right (because left motor is better quality)
    }
  } else if (l <= safeDist) { //if only object on left side is within safety distance
    correctToRight(80); //turn right (away from object)
  } else if (r <= safeDist) { //if only object on right side is within safety distance
    correctToLeft(90); //turn left
  } else if (fr > fl) { //if no side obstacles and angled slightly facing right
    correctToRight(80); //turn right (because already partway turned in that direction)
  } else if (fl > fr) { //if no side obstacles and angled slightly facing left
    correctToLeft(90); //turn left
  } else { //if no side obstacles and approaching perpendicular to front obstacle
    correctToRight(80); //turn right (because left motor is better quality)
  }
}

//turn right a given number of degrees:
void correctToRight(float angle) {
  dutyCycle = 80.0; //both motors will move at 70% speed (ideally)
  scaledDC = scaleDutyCycle(dutyCycle); //increase duty cycle if supplied voltage is less than ideal
  analogInput = scaledDC * 255.0 / 100.0; //convert duty cycle to analog input
  //calculate appropriate amount of time to turn in ms (equation found from experimentation):
  correctDuration = (angle / (2.665 * scaledDC - 122.5)) * 1000.0;
  analogWrite(lMotor, analogInput / 1.11); //turn left motor at slightly less than calculated input to compensate for difference in motor quality
  analogWrite(rMotor, 0); //right motor off
  delay(correctDuration); //turn for calculated duration
  analogWrite(lMotor, 0); //both motors are now off
}

//turn left a given number of degrees:
void correctToLeft(float angle) {
  dutyCycle = 80.0; //both motors will move at 70% speed (ideally)
  scaledDC = scaleDutyCycle(dutyCycle); //increase duty cycle if supplied voltage is less than ideal
  analogInput = scaledDC * 255.0 / 100.0; //convert duty cycle to analog input
  //calculate appropriate amount of time to turn in ms (equation found from experimentation):
  correctDuration = (angle / (2.665 * scaledDC - 122.5)) * 1000.0;
  analogWrite(rMotor, analogInput); //turn right motor at calculated input
  analogWrite(lMotor, 0); //left motor off
  delay(correctDuration); //turn for calculated duration
  analogWrite(rMotor, 0); //both motors are now off
}

//determine distance between nearest object and given sensor:
float get_dist(int pin) {
  //establish variables for duration of the ping and the distance results in inches
  unsigned long timeout = 18500; //microseconds to wait before deciding object out of range
  float pingDuration; //time required to send and receive sound wave
  float dist_inches; //inches between nearest object and given sensor
  //The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  //Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  pingDuration = pulseIn(pin, HIGH, timeout); //determine how long it took to send and receive sound wave
  dist_inches = pingDuration / 73.746 / 2.0; //convert time to inches based on speed of sound

  delay(100); //required delay before next measurement
  return dist_inches;
}

//account for battery drain to determine appropriate duty cycle:
float scaleDutyCycle(float dc) {
  const int numSamples = 10; //number of analog readings taken to determine more accurate supply voltage
  const float divFactor = 1.9; //voltage divider constant found from experimentation
  float voltage; //supply voltage
  float sdc; //scaled duty cycle
  int analogVal; //supply voltage converted to analog value by Arduino
  int sum; //sum of sampled voltage readings
  //take 10 analog samples:
  for (int i = 0; i < numSamples; i++) {
    analogVal = analogRead(analogPin); //read value on analog pin
    sum += analogVal; //add values until all samples taken (sum averaged later)
    delay(5); //avoid potentially jumbled readings
  }

  //calculate voltage based on analog samples
  voltage = ((float)sum / (float)numSamples * 5.0 / 1023.0) * divFactor;
  const float dcToVFactor = 1.067; //proportionality constant found from experimentation
  sdc = constrain(dcToVFactor * 9.0 * dc / voltage, 0.0, 100.0); //calculate scaled duty cycle (limited to between 0 and 100)
  sum = 0; //reset sum to zero for next calculation

  return sdc;
}

void correct() {
  int margin = 1; //mm, will set the tolerance on difference between the first and second ping.
  //There might be noise so don't want to get stuck in endless loop of correcting due to noise.
  int correctingTime = 500; //ms , time spent with one engine off to adjust angle.
  int checkTime = 2; //ms , time between distance checks, used to see if were approaching side or not.,

  if (get_dist(p_r) < safeDist || get_dist(p_l) < safeDist) {
    if ( get_dist(p_l) > get_dist(p_r) ) { //means right side is closer CORRECTS TO THE LEFT
      int check1 = get_dist(p_r);
      delay(100);
      int check2 = get_dist(p_r); //check 2 shouldnt be too much closer than check 1, otherwise we are quickly approaching a wall to the side.
      while (check2 < (check1)) {
        analogWrite(lMotor, 0);
        delay(correctingTime);
        analogWrite(lMotor, analogInput / 1.1);
        check1 = get_dist(p_r);
        delay(checkTime);
        check2 = get_dist(p_r);
      }
    }
    if (get_dist(p_r) > get_dist(p_l)) { //means left side is closer CORRECTS TO THE LEFT
      int check1 = get_dist(p_l);
      delay(100);
      int check2 = get_dist(p_l); //check 2 shouldnt be too much closer than check 1, otherwise we are quickly approaching a wall to the side.
      while (check2 < (check1)) {
        analogWrite(rMotor, 0);
        delay(correctingTime);
        analogWrite(rMotor, analogInput);
        check1 = get_dist(p_l);
        delay(checkTime);
        check2 = get_dist(p_l);
      }
    }
  }
}
