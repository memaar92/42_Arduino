#include <ESP8266WiFi.h>
#include <FastLED.h>
#define NUM_LEDS 64
#define DATA_PIN 5
CRGB leds[NUM_LEDS];

unsigned long previousMillis = 0;
int count = 0;
int loops = 0;

// Replace with your network credentials
const char* ssid     = "test";
const char* password = "test";

WiFiServer server(80);
String header;
String outputState[64];
String animationState = "off";
const int output5 = 5;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 25000;

void setup() {
  Serial.begin(115200);
  int i = 0;
  while (i < 64)
    outputState[i++] = "off";
  pinMode(output5, OUTPUT);
  digitalWrite(output5, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  FastLED.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
  FastLED.setBrightness(50);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();         
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turn animation on and off
            if (header.indexOf("GET /animate/on") >= 0) {
                Serial.println("animate: on");
                animationState = "on";
                } else if (header.indexOf("GET /animate/off") >= 0) {
                  Serial.println("animate: off");
                  animationState = "off";
                  }

            // turns the LEDs on and off
            if (animationState == "off")
            {
              for (int i = 0; i < 64; i++)
              {
                  if (header.indexOf("GET /" + String(i) + "/on") >= 0) {
                  Serial.println("GPIO" + String(i) + "on");
                  outputState[i] = "on";
                  } else if (header.indexOf("GET /" + String(i) + "/off") >= 0) {
                    Serial.println("GPIO" + String(i) + "off");
                    outputState[i] = "off";
                    }
              }
            }
            
            client.println("<!DOCTYPE html><html lang=\"en\">");
            client.println("<head>");
            client.println("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("  <link rel=\"icon\" href=\"data:,\">");
            client.println("  <style>");
            client.println("    html { font-family: Helvetica; margin: 10px auto; text-align: center;}");
            client.println("    .button-container { width: 100%; display: flex; flex-wrap: wrap; justify-content: space-around;}");
            client.println("    .button-row { display: flex; justify-content: space-around; width: 100%; margin: 15px}");
            client.println("    .button { display: inline-block; background-color: #195B6A; border: none; color: white;");
            client.println("      padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println("    .button2 { display: inline-block; background-color: #77878A;}");
            client.println("  </style>");
            client.println("</head>");

            client.println("<body>");
            client.println("  <h1>THE MACHINE THAT GOES PING</h1>");

            client.println("  <div class=\"button-container\">");
            for (int i = 0; i < 64; i++) {
              if (i % 5 == 0) {
                client.println("    <div class=\"button-row\">");
              }
              if (outputState[i] == "off") {
                client.println("      <div><a href=\"/" + String(i) + "/on\"><button class=\"button\">" + String(i) + " ON</button></a></div>");
              } else {
                client.println("      <div><a href=\"/" + String(i) + "/off\"><button class=\"button\">" + String(i) + " OFF</button></a></div>");
              }
              if ((i + 1) % 5 == 0 || i == 63) {
                client.println("    </div>");
              }
            }
            client.println("  </div>");

            if (animationState == "off") {
                client.println("      <div><a href=\"/animate/on\"><button class=\"button\">Animation: ON</button></a></div>");
              } else {
                client.println("      <div><a href=\"/animate/off\"><button class=\"button\">Animation: OFF</button></a></div>");
              }
            client.println("</body></html>");
            client.println();

            if (animationState == "off")
            {
              for (int i = 0; i < 64; i++)
              {
                if (outputState[i] == "on")
                  leds[i]=CRGB::Blue;
                else if (outputState[i] == "off")
                  leds[i]=CRGB::Black;
                FastLED.show();
              }
            }
            else
            {
              while (loops < 640)
              {
                shootingStarAnimation(255, 255, 255, random(10, 60), random(5, 40), random(2000, 8000), 1);
                loops++;
              }
              loops = 0;
              for (int i = 0; i < 64; i++)
              {
                leds[i]=CRGB::Black;
                FastLED.show();
              }
            }
            
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void shootingStarAnimation(int red, int green, int blue, int tail_length, int delay_duration, int interval, int direction){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    count = 0;
  }
  if (direction == -1) {
    if (count < NUM_LEDS) {
      leds[NUM_LEDS - (count % (NUM_LEDS+1))].setRGB(red, green, blue);
      count++;
    }
  }
  else {
    if (count < NUM_LEDS) {
      leds[count % (NUM_LEDS+1)].setRGB(red, green, blue);
      count++;
    }
  }
  fadeToBlackBy(leds, NUM_LEDS, tail_length);
  FastLED.show();
  delay(delay_duration);
}
