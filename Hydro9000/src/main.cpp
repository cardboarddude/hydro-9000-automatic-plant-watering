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
// // volatile int iterationsSinceLastClick = 0;
// // volatile bool isSelectWheelClicked = false, isSelectWheeldoubleClicked = false;
// // ScreenMenu mainMenu, waterLevelsMenu(), waterPlantsMenu(), historyMenu(), settingsMenu();
// // ScreenMenu* currentMenu;


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
// // void goToMoistureScreen() {
// //   //currentScreen = SCREEN::MOISTURE_LEVELS;
// // }
// // void goToMoistureGoalScreen() {
// //   //currentScreen = SCREEN::MOISTURE_GOALS;
// // }
// // void goToHistoryPastHour() {
// //   // currentScreen = SCREEN::HISTORY;
// // }

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
  // delay(1000);
  // Screen::display->clearDisplay();
  // Screen::display->setTextColor(WHITE);
  // Screen::display->setTextSize(1);
  // Screen::display->setCursor(64, 32);
  // Screen::display->println("test1");
  // Screen::display->display();
  // delay(1000);
  // Screen::display->clearDisplay();
  // Screen::display->setRotation(0);
  // Screen::display->setCursor(64, 32);
  // Screen::display->print("test2");
  // Screen::display->display();
  // delay(1000);
  // Screen::display->clearDisplay();
  // Screen::display->setRotation(0);
  // Screen::display->setCursor(64, 32);
  // Screen::display->print("test3");
  // Screen::display->display();
  // delay(1000);
  // Screen::display->clearDisplay();
  // Screen::display->setRotation(1);
  // Screen::display->setCursor(64, 32);
  // Screen::display->print("test4");
  // Screen::display->display();
  // delay(1000);
  // switch (currentScreen) {
  //   case SCREEN::OFF:
  //     isDisplaySetup = false;
  //     display.clearDisplay();
  //     display.display();
  //     digitalWrite(BLUE_BUTTON_LED_PIN, LOW);
  //     digitalWrite(RED_BUTTON_LED_PIN,  LOW);
  //     break;
  //   case SCREEN::HOME:
  //     drawHomescreen();

  //     if ((isBluePressed && !wasBluePressed)
  //       || (isRedPressed && !wasRedPressed)
  //       || (isSelectWheelPressed && !wasSelectWheelPressed)) {
  //         currentScreen = SCREEN::MENU;
  //     }
  //     digitalWrite(BLUE_BUTTON_LED_PIN, HIGH);
  //     digitalWrite(RED_BUTTON_LED_PIN,  HIGH);
  //     break;
  //   case SCREEN::MOISTURE_GOALS:
  //     showMoistureGoals();
  //     if ((isBluePressed && !wasBluePressed)
  //       || (isRedPressed && !wasRedPressed)
  //       || (isSelectWheelPressed && !wasSelectWheelPressed)) {
  //         currentScreen = SCREEN::MENU;
  //     }
  //     digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? HIGH : LOW);
  //     digitalWrite(RED_BUTTON_LED_PIN, HIGH);
  //     break;
  //   case SCREEN::PUMP_CONTROL:
  //       if (isBluePressed && !wasBluePressed) {
  //         hydro.startSelectedPump();
  //         if (pumpBox.isPumpRunning()) {
  //           pumpBox.stopAllPumps();
  //         if (currentMenu->isActionSelected()) {
  //           (currentMenu->getSelectedAction())();
  //         }
  //       }
  //   case SCREEN::MENU:
  //     if (selectWheel.getClockwiseTurns()-SCROLLING_SPEED > lastPosition) {
  //       selectWheel.resetTurns();
  //       lastPosition = selectWheel.getClockwiseTurns();
  //       currentMenu->selectNextItem();
  //     } else if (selectWheel.getClockwiseTurns()+SCROLLING_SPEED < lastPosition) {
  //       selectWheel.resetTurns();
  //       lastPosition = selectWheel.getClockwiseTurns();
  //       currentMenu->selectPreviousItem();
  //     }
      
  //     if (isSelectWheelPressed && !wasSelectWheelPressed) {
  //       if (currentMenu->isActionSelected()) {
  //         Serial.print("Action selected. ");
  //         (currentMenu->getSelectedAction())();
  //       } else if (currentMenu->isSubMenuSelected()) {
  //         Serial.print("SubMenu selected. ");
  //         currentMenu = &(currentMenu->getSelectedSubMenu());
  //         currentMenu->selectedItemIndex = 0;
  //       } else {
  //         Serial.print("Unknown selection at position "+String(currentMenu->childMenus.size())+" of "+String(currentMenu->actions.size())+". ");
  //       }
  //     }
  //     if (currentMenu == &waterPlantsMenu) {
  //       currentScreen = SCREEN::PUMP_CONTROL;
  //     }
      
  //     digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? HIGH : LOW);
  //     digitalWrite(RED_BUTTON_LED_PIN, HIGH);

  //     currentMenu->doDisplay();
  //     break;
  //   case SCREEN::MOISTURE_LEVELS:
  //     showMoistureLevels();

  //     if ((isBluePressed && !wasBluePressed)
  //       || (isRedPressed && !wasRedPressed)
  //       || (isSelectWheelPressed && !wasSelectWheelPressed)) {
  //         currentScreen = SCREEN::MENU;
  //     }
  //     digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? HIGH : LOW);
  //     digitalWrite(RED_BUTTON_LED_PIN, HIGH);
  //     break;
  //   case SCREEN::HISTORY:
  //     String selectedMenuItemDisplayName = currentMenu->getSelectedItemDisplayName();
  //     selectedMenuItemDisplayName.toUpperCase();
  //     if (selectedMenuItemDisplayName.length() > 10) {
  //       displayTitle(selectedMenuItemDisplayName.substring(0, 10));
  //     } else {
  //       displayTitle(selectedMenuItemDisplayName);
  //     }
  //     if (selectedMenuItemDisplayName.indexOf("SECOND") >= 0) {
  //       showMoistureHistory(MILLIS_FROM::SECONDS, 100);
  //     } else if (selectedMenuItemDisplayName.indexOf("MINUTE") >= 0) {
  //       showMoistureHistory(MILLIS_FROM::MINUTES, 100);
  //     } else if (selectedMenuItemDisplayName.indexOf("QUARTER HOUR") >= 0) {
  //       showMoistureHistory(MILLIS_FROM::QUARTER_HOURS, 100);
  //     } else if (selectedMenuItemDisplayName.indexOf("HOUR") >= 0) {
  //       showMoistureHistory(MILLIS_FROM::HOURS, 100);
  //     } else {
  //       showMoistureHistory(MILLIS_FROM::DAYS, 100);
  //     }
  //     if ((isBluePressed && !wasBluePressed)
  //       || (isRedPressed && !wasRedPressed)
  //       || (isSelectWheelPressed && !wasSelectWheelPressed)) {
  //         currentScreen = SCREEN::MENU;
  //     }
  //     digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? HIGH : LOW);
  //     digitalWrite(RED_BUTTON_LED_PIN, HIGH);
  //     break;
  //   default:
  //     Serial.print("Unknown SCREEN:: "+String(currentScreen));
  // }

  // if (currentScreen != SCREEN::OFF) {
  //   if (pumpBox.isPumpRunning()) {
  //     if (timeoutMS + BLUE_LED_BLINK_INTERVAL_MS < millis()) {
  //       isBlueLedOn = !isBlueLedOn;
  //       timeoutMS = millis();
  //     }
  //   } else {
  //     isBlueLedOn = true;
  //   }
  //   display.display();
  // }

  // wasSelectWheelPressed = isSelectWheelPressed;
  // wasBluePressed = isBluePressed;
  // wasRedPressed = isRedPressed;
  // wasKeyUnlocked = isKeyUnlocked;
}


// void readAllSensors() {
//   //Serial.print("Reading from all sensors, length: "+String(sensors.size()));
//   for (int i = 0; i < sensors.size(); i++) {
//     bool doSaveData = false;
//     sensors.at(i).read();
//     if (sensors.at(i).isNextMinute()) {
//       sensors.at(i).pushLastMinuteReadings();
//       doSaveData = true;
//     }
//     if (sensors.at(i).isNextQuarterHour()) {
//       sensors.at(i).pushLastQuarterHourReadings();
//       doSaveData = true;
//     }
//     if (doSaveData) {
//       sensors.at(i).saveData();
//     }
//   }
// }

// void doDisplaySetup() {
//   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
//     Serial.println(F("SSD1306 allocation failed"));
//     for(;;);
//   }
//   isDisplaySetup = true;
// }

// void drawHomescreen() {
//   //displayTitle("HYDRO 9000");
//   //displaySensorReadings();
//   //display.drawBitmap(47, 20, LEAF_LOGO_BMP, 33, 44, WHITE);
// }

// void displayTitle(String title) {
//   display.setTextSize(2);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.println(title);
// }

// void showMoistureLevels() {
//   displayTitle("MOISTURE");
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   for (int i = 0; i < sensors.size(); i++) {
//     drawMoistureBar(sensors.at(i).getMoistureLevel(), i);
//   }
//   display.setCursor(10, 56);
//   display.setTextColor(WHITE);
// }

// void showMoistureHistory(MILLIS_FROM durationMS, int intervalCount) {
//   display.setCursor(0, 20);
//   const int CHART_HEIGHT = 48;
  
//   MoistureSensor currentSensor = sensors.at(0);

//   int startY = display.height();
//   int startX = display.width() - intervalCount;
//   int intervalSizeMS = durationMS / intervalCount;
//   int maxReading = currentSensor.maxReading, minReading = currentSensor.minReading;
//   int readingInterval = (maxReading - minReading) / CHART_HEIGHT;

//   if (true || durationMS <= MoistureSensor::HISTORY_SIZE * MILLIS_FROM::MINUTES) {
//     for (int i = 0; i < intervalCount; i++) {
//       display.drawPixel(startX + i, startY - (currentSensor.readingHistoryInMinutes[i] / readingInterval), WHITE);
//     }
//   } else {

//   }
// }

// void drawMoistureBar(double levelPercent, int16_t barIndex) {
//   const int BAR_WIDTH = 23;
//   const int BAR_MAX_HEIGHT = 48;
//   const int BAR_MIN_HEIGHT = 1;
//   const int BAR_SPACING = 1;
//   const int START_X = 128-(BAR_SPACING + BAR_WIDTH) * 5;
//   const int START_Y = 0;
//   const int CHAR_WIDTH = 6, TEXT_HEIGHT = 7, TEXT_PADDING_Y = 3;

//   int height = std::max(double(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
//   int startX = barIndex * (BAR_WIDTH + BAR_SPACING) + START_X;
//   int startY = display.height() - height;

//   display.fillRect(startX, startY, BAR_WIDTH, height, WHITE);

//   int percentage = levelPercent * 100;
//   int textY, textX;
//   int textWidth, percentCharCount;

//   if (percentage < 10) {
//     percentCharCount = 2;
//   } else if (percentage < 100) {
//     percentCharCount = 3;
//   } else if (percentage == 100) {
//     percentCharCount = 4;
//   }
//   textWidth = percentCharCount * CHAR_WIDTH;
//   textX = getCenterAlignPosition(startX, startX + BAR_WIDTH, textWidth);
//   textY = startY + TEXT_PADDING_Y;

//   if (textY + TEXT_HEIGHT + TEXT_PADDING_Y > display.height()) {
//     textY = startY - TEXT_PADDING_Y - TEXT_HEIGHT;
//     display.setTextColor(WHITE);
//   } else {
//     display.setTextColor(BLACK);
//   }
//   display.setCursor(textX, textY);
//   display.setTextSize(1);
//   display.println(String(percentage)+"%");
// }

// int getCenterAlignPosition(int startPosition, int endPosition, int thingWidth) {
//   int width = endPosition - startPosition + 1;
//   return (width - thingWidth) / 2 + startPosition;
// }

// void showMoistureGoals() {
//   displayTitle("MOISTURE");
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   std::vector<double> goals = hydro.getGoals();

//   for (int i = 0; i < PLANT_COUNT; i++) {
//     drawMoistureGoal(goals[i], i);
//   }
//   display.setCursor(10, 56);
//   display.setTextColor(WHITE);
// }

// void drawMoistureGoal(double levelPercent, int16_t barIndex) {
//   const int BAR_WIDTH = 23;
//   const int BAR_MAX_HEIGHT = 48;
//   const int BAR_MIN_HEIGHT = 1;
//   const int BAR_SPACING = 1;
//   const int START_X = 128-(BAR_SPACING + BAR_WIDTH) * 5;
//   const int START_Y = 0;

//   int height = std::max(double(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
//   int startX = barIndex * (BAR_WIDTH + BAR_SPACING) + START_X;
//   int startY = display.height() - height;
  

//   display.fillRect(startX, startY, BAR_WIDTH, height, WHITE);

//   int textY, textPaddingX = 3;
//   if (height < 13) {
//     textY = fmin(startY - 10, display.height() - 10);
//     display.setTextColor(WHITE);
//   } else {
//     textY = startY + 3;
//     display.setTextColor(BLACK);
//   }
//   int percentage = levelPercent * 100;
//   if (percentage < 10) {
//     textPaddingX += 3;
//   }
//   display.setCursor(startX+textPaddingX, textY);
//   display.setTextSize(1);
//   display.println(String(percentage)+"%");
// }