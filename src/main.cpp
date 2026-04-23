#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ENCODER_CLK 4
#define ENCODER_DT 21 
#define ENCODER_SW 2
#define AMBER_LED 6
#define GREEN_LED 5
#define RELAY_PIN 10
#define BUZZER 11

Preferences prefs;

volatile int counter = 0;       
volatile bool activateFlag = false; 
int state;
int prevState;
const int MAX_SEC = 300;
bool update = true;

void displayTimer()
{

  int mins = counter / 60;
  int secs = counter % 60;

  std::string time = std::to_string(mins) + ":";
  if (secs < 10)
    time += "0";

  time += std::to_string(secs);

  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - (time.length() * 30)) / 2, (SCREEN_HEIGHT - 40) / 2);
  display.print(time.c_str()); // i miss python
  display.display();
  update = false;
}

void IRAM_ATTR activate() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 250) {
    activateFlag = !activateFlag;
  }
  last_interrupt_time = interrupt_time;
}

void activateInit() {
  for (int five = 5; five > 0; five--) {
    if (!activateFlag) break; 
    
    digitalWrite(AMBER_LED, HIGH);
    delay(150);
    digitalWrite(AMBER_LED, LOW); 

    display.clearDisplay();
    display.setCursor(50, 15);
    display.print(five);
    display.display();

    for (int i = 0; i < 17; i++) {
      if (!activateFlag) break;
      delay(50);
    }
  }
}

void threeChips() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    digitalWrite(GREEN_LED, LOW);
    delay(100);
  }
}

void setup() {
  digitalWrite(RELAY_PIN, HIGH); 
  digitalWrite(AMBER_LED, LOW);
  //digitalWrite(GREEN_LED, HIGH);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
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
  pinMode(BUZZER, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), activate, FALLING);
  


  prevState = digitalRead(ENCODER_CLK);

  prefs.begin("timer", false); 
  counter = prefs.getInt("timer", 0);

  displayTimer();
}

void loop() {
  if (activateFlag) {
    display.invertDisplay(true);
    
    if (prefs.getInt("timer", 0) != counter) {
      prefs.putInt("timer", counter);
    }

    activateInit();

    if (activateFlag) {
      digitalWrite(AMBER_LED, HIGH);
      digitalWrite(RELAY_PIN, LOW); 
      
      int _counter_ = counter;
      for (; counter > 0; counter--) {
        if (!activateFlag) break;
        displayTimer();
        
        for (int i = 0; i < 20; i++) {
          if (!activateFlag) break;
          delay(50);
        }
      }
      
      digitalWrite(AMBER_LED, LOW);
      digitalWrite(RELAY_PIN, HIGH); 
      
      if (activateFlag) threeChips();
      counter = _counter_;
    }
    
    activateFlag = false;
    display.invertDisplay(false);
    displayTimer();
  } 
  else {
    state = digitalRead(ENCODER_CLK);
    if (state != prevState && state == LOW) {
      if (digitalRead(ENCODER_DT) != state) {
        if (counter < MAX_SEC) counter++;
      } else {
        if (counter > 0) counter--;
      }
      update = true;
    }
    if (update) displayTimer();
    prevState = state;
  }
}