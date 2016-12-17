#include <SoftwareSerial.h>
int signalpin = 6;
float distance;
float pulseduration = 0;
float distanceData[] = {3.27, 2.86, 4.17, 5.73, 6.66, 11.35, 16.84, 21.30, 26.78, 30.58, 36.10, 42.79, 50.46, 53.27, 56.47, 60.26, 67.75, 72.62, 77.22, 81.65, 88.79, 93.06, 97.20, 102.78, 108.60, 112.87, 117.43, 122.32, 126.56, 132.47, 137.67, 141.85, 147.53, 151.15, 155.77, 160.33, 165.15, 169.56, 176.48, 181.68, 187.42, 190.23, 194.12, 199.01, 205.43, 212.75, 213.73, 219.47, 225.22, 225.08, 232.89, 237.60, 243.23, 250.65, 257.53, 261.97, 262.16, 267.93, 273.83, 278.04, 282.36, 288.68, 290.11, 290.99, 291.55, 292.30, 293.83, 294.09, 295.55, 297.24, 297.60, 299.37, 301.18, 303.04, 307.40, 305.35, 306.02, 307.24, 307.00, 308.84, 309.12, 310.29, 311.36, 311.72, 313.79, 313.49, 313.79, 313.79, 313.79, 313.79, 313.79, 313.79};
float errorPercentageData[] = {227.0000, 43.0000, 39.0000, 43.2500, 33.2000, 13.5000, 12.2667, 6.5000, 7.1200, 1.9333, 3.1429, 6.9750, 12.1333, 6.5400, 2.6727, 0.4333, 4.2308, 3.7429, 2.9600, 2.0625, 4.4588, 3.4000, 2.3158, 2.7800, 3.4286, 2.6091, 2.1130, 1.9333, 1.2480, 1.9000, 1.9778, 1.3214, 1.7448, 0.7667, 0.4968, 0.2062, 0.0909, 0.2588, 0.8457, 0.9333, 1.3081, 0.1211, 0.4513, 0.4950, 0.2098, 1.3095, 0.5907, 0.2409, 0.0978, 2.1391, 0.8979, 1.0000, 0.7224, 0.2600, 0.9922, 0.7577, 1.0717, 0.7667, 0.4255, 0.7000, 0.9263, 0.4552, 0.3058, 0.3459, 0.4949, 0.5782, 0.3966, 0.6453, 0.4882, 0.2550, 0.4682, 0.2100, 0.0598, 0.3444, 1.4521, 0.4441, 0.3344, 0.4052, 0.0000, 0.2727, 0.0388, 0.0935, 0.1158, 0.0897, 0.2524, 0.1624, 0.3841, 0.6994, 1.0126, 1.3239, 1.6332, 1.9406};
int actualDistance[] = {1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255, 260, 265, 270, 275, 280, 285, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320};
float realDistance = 0;

unsigned char Data[10];
unsigned char i;

void setup() {
  
  Serial.begin(9600); //set it up
  pinMode(signalpin, OUTPUT);
  delay(1000);

}


void measureDistance() {

  for (int i = 0; i < 5; i++) {
    pinMode(signalpin, OUTPUT);
    digitalWrite(signalpin, LOW); //make sure its low
    delayMicroseconds(3);
    digitalWrite(signalpin, HIGH);
    delayMicroseconds(3);
    digitalWrite(signalpin, LOW);
    pinMode(signalpin, INPUT);
    pulseduration += pulseIn(signalpin, HIGH);
    delay(200);
  }
  pulseduration = pulseduration / 5;

}


void checkDistance() {
  if (distance < 4) {
    realDistance = 0;
  }
  else {
    int i = 0;
    while (distanceData[i] < distance) {
      i++;
    }
    float x3 = distanceData[i + 1], x1 = distanceData[i];
    int y3 = actualDistance[i + 1], y1 = actualDistance[i];

    realDistance = (distance - x1) * (y3 - y1) / (x3 - x1) + y1;

    for (int j = 3; j < 92; j++) {
      if (distance == distanceData[j]) {
        realDistance = actualDistance[j];
      }
    }

    if (realDistance == 0) {
      for (int k = 3; k < 92; k++) {
        if (distance < distanceData[k]) {
          realDistance = actualDistance[k - 1] + (actualDistance[k] - actualDistance[k - 1]) / (distanceData[k] - distanceData[k - 1]) * (distance - distanceData[k - 1]); //interpolation
        }
      }
    }
  }
}

void obstacle() {
  if (realDistance < 100.0) {
    //obstacle avoidance code
  } else if (realDistance < 200) {
    //normal code
  } else {
    //stop code
  }
}

void loop() {
  measureDistance();
  pulseduration = pulseduration / 2.0;
  distance = (pulseduration / 29.034); //in cm
  checkDistance();
  obstacle();

  Serial.print("Measured Distance is: ");
  Serial.print(distance);
  Serial.print("\n");
  Serial.print("But the actual distance is: ");
  Serial.print(realDistance);
  Serial.print("+/-0.5cm");
  Serial.print("\n");
  delay(200);

  realDistance = 0;

}
