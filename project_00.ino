struct led
{
  const int pin;
  int state;
  unsigned long blinkDuration;
  unsigned long previousMillis;
};

struct led green = {11, HIGH, 0, 0};
struct led red = {9, LOW, 1000, 0};
struct led yellow = {12, LOW, 1000, 0};

const int buttonPin = 10;
bool blinkState = 0;

unsigned long debounceDuration = 500; // preventing false positives by delaying the reading of the button state; time in millisec
unsigned long lastTimeButtonStateChanged = 0;

void setup() 
{
  pinMode(yellow.pin, OUTPUT);
  pinMode(red.pin, OUTPUT);
  pinMode(green.pin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() 
{
  if (millis() - lastTimeButtonStateChanged > debounceDuration)
  {
    bool buttonState = digitalRead(buttonPin);
    if (buttonState != 1)
    {
      reset_vars();
      delay(500);
      buttonState = digitalRead(buttonPin);
      if (buttonState != 1)
      {
        red.state = HIGH;
      }
      delay(500);
      buttonState = digitalRead(buttonPin);
      if (buttonState != 1)
      {
        red.state = LOW;
        red.blinkDuration = 500;
        yellow.blinkDuration = 2000;
      }
      lastTimeButtonStateChanged = millis();
      if (green.state == LOW)
      {
        green.state = HIGH;
        blinkState = 0;
      }
      else
      {
        green.state = LOW;
        blinkState = 1;
      }
    }
    digitalWrite(green.pin, green.state);
    blink(blinkState, &yellow);
    blink(blinkState, &red);
  }
}

void  reset_vars(void)
{
  red.state = LOW;
  yellow.state = LOW;
  red.blinkDuration = 1000;
  yellow.blinkDuration = 1000;
}

void  blink(bool state, struct led *led_type)
{
  if (state == 1)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - led_type->previousMillis >= led_type->blinkDuration)
    {
      led_type->previousMillis = currentMillis;
      if (led_type->state == LOW)
        led_type->state = HIGH;
      else
        led_type->state = LOW;
      digitalWrite(led_type->pin, led_type->state);
    }
  }
  else
    digitalWrite(led_type->pin, LOW);
}
