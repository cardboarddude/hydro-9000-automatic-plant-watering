#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <ArduinoSTL.h>
#include "Hydro9000.h"
#include "MoistureSensor.h"
#include "SelectWheel.h"
#include "ScreenMenu.h"
#include "WaterPump.h"
#include "PlantController.h"
#include "ControlPanel.h"
#include "Button.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306* Screen::display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool Screen::isSetup = false;
Adafruit_SSD1306* DisplayText::display = Screen::display;
unsigned long Hydro9000::currentMillis = 0;
unsigned long ControlPanel::currentMillis = 0;
unsigned long PlantController::currentMillis = 0;
unsigned long Button::currentMillis = 0;

// sensorPins, pumpPins, and plantNames should be the same size()
const unsigned char PLANT_COUNT PROGMEM = 5;

Hydro9000 hydro;
// std::vector<String> subMenuSettingsItems = {"Enable/Disable Pump", "Vacation Mode"};

const unsigned char  BLUE_BUTTON_PIN PROGMEM = 12;
const unsigned char RED_BUTTON_PIN PROGMEM = 13;
const unsigned char KEY_SWITCH_PIN PROGMEM = 8;
const unsigned char SELECT_WHEEL_BUTTON_PIN PROGMEM = 7;
const unsigned char SELECT_WHEEL_ENCODER_PIN_A PROGMEM = 18;
const unsigned char SELECT_WHEEL_ENCODER_PIN_B PROGMEM = 19;
const unsigned char EMERGENCY_STOP_BUTTON_PIN PROGMEM = 2;

const unsigned char BLUE_BUTTON_LED_PIN PROGMEM = 52;
const unsigned char RED_BUTTON_LED_PIN PROGMEM = 53;

const unsigned char double_CLICK_SPEED_LOOP_COUNT PROGMEM = 100;
const unsigned char SCROLLING_SPEED PROGMEM = 2; // [0, n] where 0 is the fastest and n is the slowest 

const unsigned int BLUE_LED_BLINK_INTERVAL_MS PROGMEM = 575;

SelectWheel selectWheel;


/* For Debugging */
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;
// SP will be replaced with: *((uint16_t volatile *)(0x3D))

struct __freelist {
  size_t sz;
  struct __freelist *nx;
};
extern struct __freelist *__flp;
struct __freelist *freelist_to_walk;

void memCheck() {
      Serial.println("");
  Serial.println("SRAM area borders =================");
  Serial.print(".bss end     = ");
  Serial.println((int)&__bss_end);
  Serial.print("Heap start   = ");
  Serial.println((int)&__heap_start);
  Serial.print("__brkval     = ");
  Serial.println((int)__brkval);
  Serial.print("Stack pointer= ");
  Serial.println((int)SP);
  Serial.print("Free memory  = ");
  if(__brkval == 0x00)Serial.println(SP - (int)&__bss_end);
  else Serial.println(SP - (int)__brkval);
  
  Serial.println("");
  Serial.println("Free locations in the free list ===");
  for (freelist_to_walk = __flp; freelist_to_walk; freelist_to_walk = freelist_to_walk->nx) {
    Serial.print("Address= ");
    Serial.print((int) &freelist_to_walk->nx);
    Serial.print(" Free bytes= ");
    Serial.println((int) freelist_to_walk->sz);
  }
}

/* For Debugging */
void freeBytes() {
  Serial.print("Free memory  = ");
  if(__brkval == 0x00)Serial.println(SP - (int)&__bss_end);
  else Serial.println(SP - (int)__brkval);
}

void doEncoderPinARising() {
  selectWheel.readPinB();
  selectWheel.setPinARising();
}

void doEncoderPinBFalling() {
  selectWheel.readPinA();
  selectWheel.setPinBFalling();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
            
  Hydro9000::setCurrentMillis(0);
  unsigned char sensorPins[PLANT_COUNT] = {54, 55, 56, 57, 58};
  unsigned char pumpPins[PLANT_COUNT] = {30, 34, 38, 42, 46};
  String plantNames[PLANT_COUNT] = {"Basil", "Cilantro", "Oregano", "Thyme", "Rosemary"};
  for (unsigned int i = 0; i < PLANT_COUNT; i++) {
    MoistureSensor sensor(sensorPins[i]);
    WaterPump pump(pumpPins[i]);

    PlantController plant(sensor, pump);
    plant.name = "plant_"+String(i);
    plant.displayName = plantNames[i];

    hydro.addPlantController(plant);
  }
  
  ControlPanel controlPanel;

  attachInterrupt(digitalPinToInterrupt(SELECT_WHEEL_ENCODER_PIN_A), doEncoderPinARising, RISING);
  attachInterrupt(digitalPinToInterrupt(SELECT_WHEEL_ENCODER_PIN_B), doEncoderPinBFalling, FALLING);
  selectWheel = SelectWheel(SELECT_WHEEL_ENCODER_PIN_A, SELECT_WHEEL_ENCODER_PIN_B);
  selectWheel.readPinA();
  selectWheel.readPinB();
  controlPanel.addSelectWheel(selectWheel);

  /* Add buttons to control panel */
  Button blueButton(BLUE_BUTTON_PIN, BLUE_BUTTON_LED_PIN);
  Button redButton(RED_BUTTON_PIN, RED_BUTTON_LED_PIN);
  Button selectWheelButton(SELECT_WHEEL_BUTTON_PIN);
  Button emergencyStopButton(EMERGENCY_STOP_BUTTON_PIN);
  Button keySwitch(KEY_SWITCH_PIN);
  blueButton.stateWhenButtonPressed = LOW;
  redButton.stateWhenButtonPressed = LOW;
  selectWheelButton.stateWhenButtonPressed = LOW;
  controlPanel.addButton(ControlPanel::ButtonName::BLUE, blueButton);
  controlPanel.addButton(ControlPanel::ButtonName::RED, redButton);
  controlPanel.addButton(ControlPanel::ButtonName::SELECT_WHEEL, selectWheelButton);
  controlPanel.addButton(ControlPanel::ButtonName::EMERGENCY_STOP, emergencyStopButton);
  controlPanel.addButton(ControlPanel::ButtonName::KEY_SWITCH, keySwitch);
  
  hydro.addControlPanel(controlPanel);
  hydro.setup();

  Serial.println("Hydro900 setup complete");
}

int loopItrCount = 0;
void loop() {
  // Serial.println("");
  // Serial.println("Loop count --- "+String(++loopItrCount)+" ---");
  // memCheck();
  freeBytes();
  hydro.update();
}