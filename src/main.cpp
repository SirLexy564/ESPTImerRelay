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
#define RELAY_PIN 10

int counter = 0;
int state;
int prevState;
const int MAX_SEC = 300;
bool update = true;
int eeAdr = 0;

void displayTimer(){
    display.clearDisplay();
    display.print("test");
    display.display();
    update=false;
}

void setup(){
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(1);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(AMBER_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(AMBER_LED, LOW); //5 is default high cos JTAG
  prevState = digitalRead(ENCODER_CLK);

  prefs.begin("timer", false);
  counter = prefs.getInt("timer",0);

  displayTimer();
}

void loop(){
  state = digitalRead(ENCODER_CLK); //should do interupts

  if (state != prevState && state == LOW){
    if (digitalRead(ENCODER_DT) != state && counter <= MAX_SEC) {
      counter++;
    } else if (counter > 0){
      counter--;
    }
    displayTimer();
    prefs.putInt("timer",counter);
    
  }
}