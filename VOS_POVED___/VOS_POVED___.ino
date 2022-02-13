#include <avr/eeprom.h>

#define dirA 4
#define dirB 7
#define pwmleft 5
#define pwmright 6
int minn = 0, a, maxx = 0, timee;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

void loop() {
  //  a = analogRead(A0);
  //  if (!digitalRead(11)) {
  //    digitalWrite(13, HIGH);
  //    delay(100);
  //    eeprom_write_byte(0, analogRead(A0));
  //  }
  //  if (!digitalRead(12)) {
  //    digitalWrite(13, HIGH);
  //    delay(100);
  //    eeprom_write_byte(1, analogRead(A0));
  //  }
  //  if (!digitalRead(10)) {
  //    digitalWrite(13, HIGH);
  //    delay(100);
  //    eeprom_write_byte(2, analogRead(A1));
  //  }
  //  if (!digitalRead(9)) {
  //    digitalWrite(13, HIGH);
  //    delay(100);
  //    eeprom_write_byte(3, analogRead(A1));
  //  }
  //  digitalWrite(13, LOW);
  //  Serial.print(eeprom_read_byte(0));
  //  Serial.print(" ");
  //  Serial.print(eeprom_read_byte(1));
  //  Serial.print(" ");
  //  Serial.print(eeprom_read_byte(2));
  //  Serial.print(" ");
  //  Serial.println(eeprom_read_byte(3));
  move_line_per(75, 3, 2);
    movee(0,0);
    delay(1000000);
}

void movee( int left, int right) {
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
  err = (a-b) * k;
  movee(power-err, power + err);
}

void move_line_per(int power, int k, int per) {
  int a, b, perNow = 0;
  while (perNow < per) {
    a = analogRead(A0);
    b = analogRead(A1);
    if (a > 75 and b > 75) {
      timee = millis();
      perNow++;
      while (a > 75 and b > 75) {
        move_line(power, k);
      }
    } else {
      move_line(power, k);
    }
  }
}
