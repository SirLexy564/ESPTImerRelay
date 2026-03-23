#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
Preferences prefs;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ENCODER_CLK 4
#define ENCODER_DT 3
#define ENCODER_SW 2
#define AMBER_LED 6
#define GREEN_LED 5
#define RELAY_PIN 12

int counter = 0;
int aState;
int aLastState;
const int MAX_SEC = 300;
bool update = false;
int eeAdr = 0;


void activate(){;}

void displayCounter()
{
  if (update)
  {
  int mins = counter / 60;
  int secs = counter % 60;

  std::string time = std::to_string(mins) + ":";
  if (secs < 10)
  {
    time += "0";
  }
  time += std::to_string(secs);

  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - (time.length() * 30)) / 2, (SCREEN_HEIGHT - 40) / 2);
  display.print(time.c_str()); // i miss python
  display.display();
  update = false;
  }
}

void splashScreen(){ //todo
  displayCounter();
}

void setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(1);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  pinMode(AMBER_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  

  digitalWrite(AMBER_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);

  aLastState = digitalRead(ENCODER_CLK);

  prefs.begin("timer", false);
  counter = prefs.getInt("timer", 0);
  splashScreen();
}

void loop()
{
  aState = digitalRead(ENCODER_CLK);
  
  if (aState != aLastState && aState == LOW) {
    if (digitalRead(ENCODER_DT) != aState) {
      if (counter < MAX_SEC) counter++;
    } else {
      if (counter > 0) counter--;
    }
    
    displayCounter();
    prefs.putInt("timer", counter);
  }

  aLastState = aState;

  if (digitalRead(ENCODER_SW) == LOW) {
    delay(10);
    if (digitalRead(ENCODER_SW) == LOW) {
      while (digitalRead(ENCODER_SW) == LOW);
      activate();
    }
  }
}
