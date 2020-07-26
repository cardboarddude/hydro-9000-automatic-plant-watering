#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <ArduinoSTL.h>
#include <vector>
#include <math.h> 
#include <EEPROM.h>
#include "Hydro9000.h"
#include "MoistureSensor.h"
#include "SelectWheel.h"
#include "SelectMenu.h"
#include "WaterPump.h"
// #include "WaterPumps/WaterPumpBox.h"
#include "PlantController.h"
// #include "Hydro9000/Assets.h"
#include "ControlPanel.h"
#include "Button.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 Screen::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_SSD1306 DisplayText::display = Screen::display;
unsigned long Hydro9000::currentMillis = 0;
unsigned long ControlPanel::currentMillis = 0;
unsigned long MoistureSensor::currentMillis = 0;
unsigned long Button::currentMillis = 0;

bool Screen::isSetup = false;
// sensorPins, pumpPins, and plantNames should be the same size()
std::vector<int> sensorPins = {54, 55, 56, 57, 58};
std::vector<int> pumpPins = {30, 34, 38, 42, 46};
std::vector<String> plantNames = {"Basil", "Cilantro", "Oregano", "Thyme", "Rosemary"};
enum MILLIS_FROM {
  MILLIS = 1,
  SECONDS = 1000,
  MINUTES = 60000,
  QUARTER_HOURS = 900000,
  HOURS = 3600000,
  DAYS = 86400000
};

Hydro9000 hydro;
std::vector<String> subMenuWaterPlantsItems = {"Plant #1", "Plant #2", "Plant #3", "Plant #4", "Plant #5"};
std::vector<String> subMenuHistoryItems = {"Past Day", "Past Week", "Past Month"};
std::vector<String> subMenuSettingsItems = {"Enable/Disable Pump", "Vacation Mode"};
bool isDisplaySetup = false;

const int BLUE_BUTTON_PIN = 12;
const int RED_BUTTON_PIN = 13;
const int KEY_SWITCH_PIN = 8;
const int SELECT_WHEEL_BUTTON_PIN = 7;
const int SELECT_WHEEL_ENCODER_PIN_A = 18;
const int SELECT_WHEEL_ENCODER_PIN_B = 19;
const int EMERGENCY_STOP_BUTTON_PIN = 2;

const int BLUE_BUTTON_LED_PIN = 52;
const int RED_BUTTON_LED_PIN = 53;

const int DOUBLE_CLICK_SPEED_LOOP_COUNT = 100;
const int SCROLLING_SPEED = 2; // [0, n] where 0 is the fastest and n is the slowest 

const int BLUE_LED_BLINK_INTERVAL_MS = 575;

SelectWheel selectWheel;
// volatile int iterationsSinceLastClick = 0;
// volatile bool isSelectWheelClicked = false, isSelectWheelDoubleClicked = false;
SelectMenu mainMenu, waterLevelsMenu(), waterPlantsMenu(), historyMenu(), settingsMenu();
SelectMenu* currentMenu;

void doEncoderPinARising() {
  selectWheel.readPinB();
  selectWheel.setPinARising();
}
void doEncoderPinBFalling() {
  selectWheel.readPinA();
  selectWheel.setPinBFalling();
}
// void goToMoistureScreen() {
//   //currentScreen = SCREEN::MOISTURE_LEVELS;
// }
// void goToMoistureGoalScreen() {
//   //currentScreen = SCREEN::MOISTURE_GOALS;
// }
// void goToHistoryPastHour() {
//   // currentScreen = SCREEN::HISTORY;
// }

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  
  Hydro9000::setCurrentMillis(0);
  for (unsigned int i = 0; i < pumpPins.size(); i++) {
    PlantController plant(
      MoistureSensor(sensorPins[i], MoistureSensor::addressOffset * i), 
      WaterPump(pumpPins[i])
    );
    plant.name = "plant_"+String(i);
    plant.displayName = plantNames.at(i);
    hydro.addPlantController(plant);
  }
  
  selectWheel = SelectWheel(SELECT_WHEEL_ENCODER_PIN_A, SELECT_WHEEL_ENCODER_PIN_B);
  ControlPanel controlPanel;

  /* Add buttons to control panel */
  Button blueButton(BLUE_BUTTON_PIN, BLUE_BUTTON_LED_PIN);
  Button redButton(RED_BUTTON_PIN, RED_BUTTON_LED_PIN);
  Button selectWheelButton(SELECT_WHEEL_BUTTON_PIN);
  blueButton.stateWhenButtonPressed = LOW;
  redButton.stateWhenButtonPressed = LOW;
  selectWheelButton.stateWhenButtonPressed = LOW;
  controlPanel.addSelectWheel(selectWheel);
  controlPanel.addButton(Button::Name::BLUE, blueButton);
  controlPanel.addButton(Button::Name::RED, redButton);
  controlPanel.addButton(Button::Name::SELECT_WHEEL, selectWheelButton);
  controlPanel.addButton(Button::Name::EMERGENCY_STOP, Button(EMERGENCY_STOP_BUTTON_PIN));
  controlPanel.addButton(Button::Name::KEY_SWITCH, Button(KEY_SWITCH_PIN));
  

  hydro.addControlPanel(controlPanel);
  hydro.setup();

  attachInterrupt(digitalPinToInterrupt(selectWheel.pinA), doEncoderPinARising, RISING);
  attachInterrupt(digitalPinToInterrupt(selectWheel.pinB), doEncoderPinBFalling, FALLING);


  // doDisplaySetup();
  // display.clearDisplay();
  // display.display();

  // waterPlantsMenu = SelectMenu(String("RUN PUMPS"), Screen::display);
  // historyMenu = SelectMenu(String("HISTORY"), subMenuHistoryItems, Screen::display);
  // settingsMenu = SelectMenu(String("SETTINGS"), subMenuSettingsItems, Screen::display);
  mainMenu = SelectMenu(String("MENU"), Screen::display);
  
  // historyMenu.addItemAction("Past Hour", goToHistoryPastHour);

  // settingsMenu.addItemAction("Set Water Levels", goToMoistureGoalScreen);

  // waterPlantsMenu.addItemAction("Plant #1", togglePump1);
  // waterPlantsMenu.addItemAction("Plant #2", togglePump2);
  // waterPlantsMenu.addItemAction("Plant #3", togglePump3);
  // waterPlantsMenu.addItemAction("Plant #4", togglePump4);
  // waterPlantsMenu.addItemAction("Plant #5", togglePump5);

  // VoidFunction fGoToMainMenu = goToMainMenu;
  // waterLevelsMenu.addItemAction("Go Back", fGoToMainMenu);
  // waterPlantsMenu.addItemAction("Go Back", fGoToMainMenu);
  // historyMenu.addItemAction("Go Back", fGoToMainMenu);
  // settingsMenu.addItemAction("Go Back", fGoToMainMenu);
  

  // mainMenu.addItemAction("Water Levels", goToMoistureScreen);
  // mainMenu.addItemSubMenu("Water Plants Now", waterPlantsMenu);
  // mainMenu.addItemSubMenu("History", historyMenu);
  // mainMenu.addItemSubMenu("Settings", settingsMenu);
  // currentMenu = &mainMenu;

  // lastPosition = selectWheel.getClockwiseTurns();
  // currentScreen = SCREEN::OFF;
}


// bool wasSelectWheelPressed = false, wasBluePressed = false, wasRedPressed = false, wasKeyUnlocked = false;
// bool isMoistureLevelsVisible = false;
// unsigned long timeoutMS = 0, isBlueLedOn = true, isMenuVisible = false;

void loop() {
  hydro.update();

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

// void drawMoistureBar(float levelPercent, int16_t barIndex) {
//   const int BAR_WIDTH = 23;
//   const int BAR_MAX_HEIGHT = 48;
//   const int BAR_MIN_HEIGHT = 1;
//   const int BAR_SPACING = 1;
//   const int START_X = 128-(BAR_SPACING + BAR_WIDTH) * 5;
//   const int START_Y = 0;
//   const int CHAR_WIDTH = 6, TEXT_HEIGHT = 7, TEXT_PADDING_Y = 3;

//   int height = std::max(float(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
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
//   std::vector<float> goals = hydro.getGoals();

//   for (int i = 0; i < PLANT_COUNT; i++) {
//     drawMoistureGoal(goals[i], i);
//   }
//   display.setCursor(10, 56);
//   display.setTextColor(WHITE);
// }

// void drawMoistureGoal(float levelPercent, int16_t barIndex) {
//   const int BAR_WIDTH = 23;
//   const int BAR_MAX_HEIGHT = 48;
//   const int BAR_MIN_HEIGHT = 1;
//   const int BAR_SPACING = 1;
//   const int START_X = 128-(BAR_SPACING + BAR_WIDTH) * 5;
//   const int START_Y = 0;

//   int height = std::max(float(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
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