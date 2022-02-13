#include <Servo.h>
#define dirA 4
#define dirB 7
#define pwmA 5
#define pwmB 6
int dist, volts, timee, errDist;
Servo servo;

void setup() {
  // put your setup code here, to run once: // определение конфигурации работы порта
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  servo.attach(10);
}

void loop() { // основной цикл
  // put your main code here, to run repeatedly:
  servo.write(90);
  lineWithPer(-100, 0.4, 1);
  movee(-100, -100);
  delay(100);
  servo.write(170);
  movee(-100, 100);
  delay(500);
  movee(0, 0);
  while (analogRead(A1) < 80) {
    movee(50, -50);
  }
  movee(0, 0);
  timee = millis();
  while (timee + 1000 > millis()) {
    moveLine(-100, 0.4);
  }
  movee(0, 0);
  while (analogRead(A1) < 85 or analogRead(A0) < 85) {
    dis();
    errDist = (float)(dist-10) * 0.5;
    movee(-100 - errDist, -100 + errDist);
  }
  movee(-100, -100);
  delay(350);

  while (analogRead(A1) < 80) {
    movee(50, -50);
  }
  servo.write(90);
  lineWithPer(-100, 0.4, 1);
  movee(-100, -100);
  delay(250);
  while (analogRead(A0) < 80) {
    movee(-50, 50);
  }
  servo.write(170);
  lineWithPer(-100, 0.4, 1);
  movee(0, 0);
  servo.write(90);
  delay(100000);
}

void movee(int left, int right) {//функция управления моторами
  digitalWrite(dirA, left < 0);
  digitalWrite(dirB, right > 0);

  left = abs(constrain(left, -100, 100));
  right = abs(constrain(right, -100, 100));

  left = map(left, 0, 100, 0, 255);
  right = map(right, 0, 100, 0, 255);

  analogWrite(pwmA, left);
  analogWrite(pwmB, right);

}

void lineWithPer(int power, float k, int per) {// функция движения по линии по двм датчикам до перекрестка
  int err = 0, a, b,  perNow = 0;
  while (perNow < per) {
    a = analogRead(A1);
    b = analogRead(A0);
    err = (float)(a - b) * k;
    movee(power - err, power + err);
    if (b > 80 and a > 80) {
      perNow++;
      movee(power, power);
      delay(200);
    }
  }
}

void moveLine(int power, float k) { //функция движения по линии по двум датчикам
  int err, a, b;
  a = analogRead(A1);
  b = analogRead(A0);
  err = (float)(a - b) * k;
  movee(power - err, power + err);
}

void dis() { // считывание значений с атчика расстояния и переводв см 
  volts = analogRead(A2) * (5.0 / 1024);
  dist = 32 * pow(volts, -1.10);
}
