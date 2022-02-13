#include <Servo.h>
#define dirA 4
#define dirB 7
#define pwmleft 5
#define pwmright 6
float volts, dist, z, f = 1;

Servo servo;


void setup() {
  // put your setup code here, to run once:
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(7, OUTPUT); 
  Serial.begin(9600);
  servo.attach(7);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (z < 3) {
    move_line(- 75, 1);
    dis();
    if (dist < 20) {
      z += f;
      f = 0;
    } else {
      f = 1;
    }
    Serial.println(z);
  }
  movee(0, 0);
  servo.write(0);
  delay(1000);
  servo.write(90);
  delay(100000);
}

void movee(int left, int right) {
  digitalWrite(dirA, left < 0);
  digitalWrite(dirB, right > 0);

  left = abs(constrain(left, -100, 100));
  right = abs(constrain(right, -100, 100));

  left = map(left, 0, 100, 0, 255);
  right = map(right, 0, 100, 0, 255);

  analogWrite(pwmleft, left);
  analogWrite(pwmright, right);
}

void move_line(int power, int k) {
  int err, a, b;
  a = analogRead(A0);
  b = analogRead(A1);
  if (a > 75 and b > 75) {
    movee(-75, -75);
    delay(350);
  }
  err = (a - b) * k;
  movee(power - err, power + err);
}

void dis() {
  volts = analogRead(A2) * (5.0 / 1024);
  dist = 32 * pow(volts, -1.10);
}
