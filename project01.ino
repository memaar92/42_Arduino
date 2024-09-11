#include <LiquidCrystal.h>

const int analogPin = A0;
const int buttonPin = 7;
const int contrastPin = 9;
const int ledPin = 10;
const int rs = 11, en = 2, d4 = 3, d5 = 4, d6 = 5, d7 = 6;
int brightness = 0;
int sensor_val = 0;
int last_val = -5;
int flag = 0;
int lastButtonState = 0;
unsigned long debounceDuration = 500;
unsigned long lastTimeButtonChanged = 0;
const int contrastVal = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.print("Enter brightness value: ");
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.print("BRIGHTNESS:");
}

void loop()
{
  if (millis() - lastTimeButtonChanged > debounceDuration)
  {
    lcd.setCursor(0,1);
    bool buttonState = digitalRead(buttonPin);
    if (!buttonState)
    {
      if (flag == 0)
        flag = 1;
      else
        flag = 0;
    }
    if (flag)
    {
      if (Serial.available() > 0)
      {
        int data = Serial.parseInt();
        if (data)
        {
          sensor_val = data;
          brightness = map(sensor_val, 1, 1023, 0, 255);
        }
      }
    }
    else
    {
      sensor_val = analogRead(analogPin);
      brightness = map(sensor_val, 0, 1023, 0, 255);
    }
    if (brightness != last_val && brightness != last_val - 1 && brightness != last_val + 1)
    {
      analogWrite(ledPin, brightness);
      last_val = brightness;
      analogWrite(contrastPin, contrastVal);
      lcd.print(sensor_val);
      lcd.print("    ");
    }
  }
}
