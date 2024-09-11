#include <Servo.h>
#include <math.h>

int ldr1 = A0;
int ldr2 = A1;
int ldr3 = A2;
int lastPosition = 500;
Servo motor1;
Servo motor2;

void setup()
{
  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  pinMode(ldr3, INPUT);
  motor1.attach(3);
  motor2.attach(11);
  Serial.begin(9600);
}

void loop()
{
  int reading1 = analogRead(ldr1);
  int reading2 = analogRead(ldr2);
  int reading3 = analogRead(ldr3);

  moveMotorX(reading1, reading2, reading3);
  moveMotorY(reading2);
}

void  moveMotorX(int reading1, int reading2, int reading3)
{
  int result;
  int diffA = reading1 - reading2;
  int diffB = reading3 - reading2;

  if (abs(abs(diffA) - abs(diffB)) > 10)
  {
    if (abs(diffA) > abs(diffB) && abs(diffA) > 20)
    {
      result = map(diffA, -900, 900, 90, 0);
    }
    else if (abs(diffA) <= abs(diffB) && abs(diffB) > 20)
    {
      result = map(diffB, -900, 900, 90, 180);
    }
  }
  if (abs(result - lastPosition) > 10 && (result > 100 || result < 80))
  {
    lastPosition = result;
    motor1.write(result);
  }
}

void  moveMotorY(int reading2)
{
  motor2.write(map(reading2, 0, 1023, 0, 180));
}
