# 1 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
# 2 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 3 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2

# 5 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 6 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 7 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 8 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 9 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 10 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 11 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 12 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 13 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 14 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 15 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2




// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128 /* OLED display width, in pixels*/, 64 /* OLED display height, in pixels*/, &Wire, -1);
unsigned long MoistureSensor::currentMillis = 0;
std::vector<MoistureSensor> sensors;
std::vector<int> sensorPins = {54, 55, 56, 57, 58};
std::vector<String> subMenuWaterPlantsItems = {"Plant #1", "Plant #2", "Plant #3", "Plant #4", "Plant #5"};
std::vector<String> subMenuHistoryItems = {"Past Day", "Past Week", "Past Month"};
std::vector<String> subMenuSettingsItems = {"Enable/Disable Pump", "Vacation Mode"};
bool isDisplaySetup = false;

enum MILLIS_FROM {
  MILLIS = 1,
  SECONDS = 1000,
  MINUTES = 60000,
  QUARTER_HOURS = 900000,
  HOURS = 3600000,
  DAYS = 86400000
};

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

const int MOTOR_1_PIN = 30, MOTOR_2_PIN = 34, MOTOR_3_PIN = 38, MOTOR_4_PIN = 42, MOTOR_5_PIN = 46;
const int BLUE_LED_BLINK_INTERVAL_MS = 575;

SelectWheel selectWheel;
volatile int iterationsSinceLastClick = 0;
volatile bool isSelectWheelClicked = false, isSelectWheelDoubleClicked = false;
SelectMenu mainMenu, waterLevelsMenu, waterPlantsMenu, historyMenu, settingsMenu;
SelectMenu* currentMenu;
WaterPumpBox pumpBox({
  WaterPump(MOTOR_1_PIN),
  WaterPump(MOTOR_2_PIN),
  WaterPump(MOTOR_3_PIN),
  WaterPump(MOTOR_4_PIN),
  WaterPump(MOTOR_5_PIN)
});

enum SCREEN {
  OFF,
  HOME,
  MENU,
  PUMP_CONTROL,
  MOISTURE_LEVELS,
  MOISTURE_GOALS,
  HISTORY
};
int currentScreen = SCREEN::HOME;

int lastPosition = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  selectWheel = SelectWheel(SELECT_WHEEL_ENCODER_PIN_A, SELECT_WHEEL_ENCODER_PIN_B);
  pumpBox.setup();

  int addressOffset = 0;
  for (int i = 0; i < sensorPins.size(); i++) {
    MoistureSensor newSensor(sensorPins.at(i), MoistureSensor::addressOffset * i);
    sensors.push_back(newSensor);
    sensors.at(i).loadData();
    sensors.at(i).writeSerial();
  }

  attachInterrupt(((selectWheel.pinA) == 2 ? 0 : ((selectWheel.pinA) == 3 ? 1 : ((selectWheel.pinA) >= 18 && (selectWheel.pinA) <= 21 ? 23 - (selectWheel.pinA) : -1))), doEncoderPinARising, 3);
  attachInterrupt(((selectWheel.pinB) == 2 ? 0 : ((selectWheel.pinB) == 3 ? 1 : ((selectWheel.pinB) >= 18 && (selectWheel.pinB) <= 21 ? 23 - (selectWheel.pinB) : -1))), doEncoderPinBFalling, 2);


  doDisplaySetup();
  // display.clearDisplay();
  // display.display();

  waterPlantsMenu = SelectMenu(String("RUN PUMPS"), display);
  historyMenu = SelectMenu(String("HISTORY"), subMenuHistoryItems, display);
  settingsMenu = SelectMenu(String("SETTINGS"), subMenuSettingsItems, display);
  mainMenu = SelectMenu(String("MENU"), display);

  historyMenu.addItemAction("Past Hour", goToHistoryPastHour);

  settingsMenu.addItemAction("Set Water Levels", goToMoistureGoalScreen);

  waterPlantsMenu.addItemAction("Plant #1", togglePump1);
  waterPlantsMenu.addItemAction("Plant #2", togglePump2);
  waterPlantsMenu.addItemAction("Plant #3", togglePump3);
  waterPlantsMenu.addItemAction("Plant #4", togglePump4);
  waterPlantsMenu.addItemAction("Plant #5", togglePump5);

  VoidFunction fGoToMainMenu = goToMainMenu;
  waterLevelsMenu.addItemAction("Go Back", fGoToMainMenu);
  waterPlantsMenu.addItemAction("Go Back", fGoToMainMenu);
  historyMenu.addItemAction("Go Back", fGoToMainMenu);
  settingsMenu.addItemAction("Go Back", fGoToMainMenu);


  mainMenu.addItemAction("Water Levels", goToMoistureScreen);
  mainMenu.addItemSubMenu("Water Plants Now", waterPlantsMenu);
  mainMenu.addItemSubMenu("History", historyMenu);
  mainMenu.addItemSubMenu("Settings", settingsMenu);
  currentMenu = &mainMenu;

  pinMode(EMERGENCY_STOP_BUTTON_PIN, 0x0);
  pinMode(BLUE_BUTTON_PIN, 0x0);
  pinMode(RED_BUTTON_PIN, 0x0);
  pinMode(KEY_SWITCH_PIN, 0x0);
  pinMode(SELECT_WHEEL_BUTTON_PIN, 0x0);

  pinMode(BLUE_BUTTON_LED_PIN, 0x1);
  pinMode(RED_BUTTON_LED_PIN, 0x1);

  lastPosition = selectWheel.getClockwiseTurns();
  currentScreen = SCREEN::OFF;
}

void togglePump1() {
  pumpBox.stopAllPumpsExcept(0);
  pumpBox.togglePump(0);
}
void togglePump2() {
  pumpBox.stopAllPumpsExcept(1);
  pumpBox.togglePump(1);
}
void togglePump3() {
  pumpBox.stopAllPumpsExcept(2);
  pumpBox.togglePump(2);
}
void togglePump4() {
  pumpBox.stopAllPumpsExcept(3);
  pumpBox.togglePump(3);
}
void togglePump5() {
  pumpBox.stopAllPumpsExcept(4);
  pumpBox.togglePump(4);
}
void goToMainMenu() {
  currentScreen = SCREEN::MENU;
  currentMenu = &mainMenu;
}
void doEncoderPinARising() {
  selectWheel.readPinB();
  selectWheel.setPinARising();
}
void doEncoderPinBFalling() {
  selectWheel.readPinA();
  selectWheel.setPinBFalling();
}
void goToMoistureScreen() {
  currentScreen = SCREEN::MOISTURE_LEVELS;
}
void goToMoistureGoalScreen() {
  currentScreen = SCREEN::MOISTURE_GOALS;
}
void goToHistoryPastHour() {
  currentScreen = SCREEN::HISTORY;
}

bool wasSelectWheelPressed = false, wasBluePressed = false, wasRedPressed = false, wasKeyUnlocked = false;
bool isMoistureLevelsVisible = false;
unsigned long timeoutMS = 0, isBlueLedOn = true, isMenuVisible = false;

void loop() {
  MoistureSensor::currentMillis = millis();
  readAllSensors();
  bool isRedPressed = (digitalRead(RED_BUTTON_PIN) == 0x0);
  bool isBluePressed = (digitalRead(BLUE_BUTTON_PIN) == 0x0);
  bool isKeyUnlocked = (digitalRead(KEY_SWITCH_PIN) == 0x1);
  bool isSelectWheelPressed = (digitalRead(SELECT_WHEEL_BUTTON_PIN) == 0x0);
  bool isEmergencyStopPressed = (digitalRead(EMERGENCY_STOP_BUTTON_PIN) == 0x1);

  if (!isKeyUnlocked && wasKeyUnlocked) {
    currentScreen = SCREEN::OFF;
  } else if (isKeyUnlocked) {
    if (!wasKeyUnlocked) {
      currentScreen = SCREEN::HOME;
    }
    if (!isDisplaySetup) {
      doDisplaySetup();
    } else {
      display.clearDisplay();
    }
  }

  if (isEmergencyStopPressed) {
    pumpBox.stopAllPumps();
  }

  switch (currentScreen) {
    case SCREEN::OFF:
      isDisplaySetup = false;
      display.clearDisplay();
      display.display();
      digitalWrite(BLUE_BUTTON_LED_PIN, 0x0);
      digitalWrite(RED_BUTTON_LED_PIN, 0x0);
      break;
    case SCREEN::HOME:
      drawHomescreen();

      if ((isBluePressed && !wasBluePressed)
        || (isRedPressed && !wasRedPressed)
        || (isSelectWheelPressed && !wasSelectWheelPressed)) {
          currentScreen = SCREEN::MENU;
      }
      digitalWrite(BLUE_BUTTON_LED_PIN, 0x1);
      digitalWrite(RED_BUTTON_LED_PIN, 0x1);
      break;
    case SCREEN::MOISTURE_GOALS:
      showMoistureGoals();
      if ((isBluePressed && !wasBluePressed)
        || (isRedPressed && !wasRedPressed)
        || (isSelectWheelPressed && !wasSelectWheelPressed)) {
          currentScreen = SCREEN::MENU;
      }
      digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? 0x1 : 0x0);
      digitalWrite(RED_BUTTON_LED_PIN, 0x1);
      break;
    case SCREEN::PUMP_CONTROL:
        if (isBluePressed && !wasBluePressed) {
          if (pumpBox.isPumpRunning()) {
            pumpBox.stopAllPumps();
          } else if (currentMenu->isActionSelected()) {
            (currentMenu->getSelectedAction())();
          }
        }
    case SCREEN::MENU:
      if (selectWheel.getClockwiseTurns()-SCROLLING_SPEED > lastPosition) {
        selectWheel.resetTurns();
        lastPosition = selectWheel.getClockwiseTurns();
        currentMenu->selectNextItem();
      } else if (selectWheel.getClockwiseTurns()+SCROLLING_SPEED < lastPosition) {
        selectWheel.resetTurns();
        lastPosition = selectWheel.getClockwiseTurns();
        currentMenu->selectPreviousItem();
      }

      if (isSelectWheelPressed && !wasSelectWheelPressed) {
        if (currentMenu->isActionSelected()) {
          Serial.print("Action selected. ");
          (currentMenu->getSelectedAction())();
        } else if (currentMenu->isSubMenuSelected()) {
          Serial.print("SubMenu selected. ");
          currentMenu = &(currentMenu->getSelectedSubMenu());
          currentMenu->selectedItemIndex = 0;
        } else {
          Serial.print("Unknown selection at position "+String(currentMenu->childMenus.size())+" of "+String(currentMenu->actions.size())+". ");
        }
      }
      if (currentMenu == &waterPlantsMenu) {
        currentScreen = SCREEN::PUMP_CONTROL;
      }

      digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? 0x1 : 0x0);
      digitalWrite(RED_BUTTON_LED_PIN, 0x1);

      currentMenu->doDisplay();
      break;
    case SCREEN::MOISTURE_LEVELS:
      showMoistureLevels();

      if ((isBluePressed && !wasBluePressed)
        || (isRedPressed && !wasRedPressed)
        || (isSelectWheelPressed && !wasSelectWheelPressed)) {
          currentScreen = SCREEN::MENU;
      }
      digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? 0x1 : 0x0);
      digitalWrite(RED_BUTTON_LED_PIN, 0x1);
      break;
    case SCREEN::HISTORY:
      String selectedMenuItemDisplayName = currentMenu->getSelectedItemDisplayName();
      selectedMenuItemDisplayName.toUpperCase();
      if (selectedMenuItemDisplayName.length() > 10) {
        displayTitle(selectedMenuItemDisplayName.substring(0, 10));
      } else {
        displayTitle(selectedMenuItemDisplayName);
      }
      if (selectedMenuItemDisplayName.indexOf("SECOND") >= 0) {
        showMoistureHistory(MILLIS_FROM::SECONDS, 100);
      } else if (selectedMenuItemDisplayName.indexOf("MINUTE") >= 0) {
        showMoistureHistory(MILLIS_FROM::MINUTES, 100);
      } else if (selectedMenuItemDisplayName.indexOf("QUARTER HOUR") >= 0) {
        showMoistureHistory(MILLIS_FROM::QUARTER_HOURS, 100);
      } else if (selectedMenuItemDisplayName.indexOf("HOUR") >= 0) {
        showMoistureHistory(MILLIS_FROM::HOURS, 100);
      } else {
        showMoistureHistory(MILLIS_FROM::DAYS, 100);
      }
      if ((isBluePressed && !wasBluePressed)
        || (isRedPressed && !wasRedPressed)
        || (isSelectWheelPressed && !wasSelectWheelPressed)) {
          currentScreen = SCREEN::MENU;
      }
      digitalWrite(BLUE_BUTTON_LED_PIN, (isBlueLedOn) ? 0x1 : 0x0);
      digitalWrite(RED_BUTTON_LED_PIN, 0x1);
      break;
    default:
      Serial.print("Unknown SCREEN:: "+String(currentScreen));
  }

  if (currentScreen != SCREEN::OFF) {
    if (pumpBox.isPumpRunning()) {
      if (timeoutMS + BLUE_LED_BLINK_INTERVAL_MS < millis()) {
        isBlueLedOn = !isBlueLedOn;
        timeoutMS = millis();
      }
    } else {
      isBlueLedOn = true;
    }
    display.display();
  }

  wasSelectWheelPressed = isSelectWheelPressed;
  wasBluePressed = isBluePressed;
  wasRedPressed = isRedPressed;
  wasKeyUnlocked = isKeyUnlocked;
}


void readAllSensors() {
  //Serial.print("Reading from all sensors, length: "+String(sensors.size()));
  for (int i = 0; i < sensors.size(); i++) {
    bool doSaveData = false;
    sensors.at(i).read();
    if (sensors.at(i).isNextMinute()) {
      sensors.at(i).pushLastMinuteReadings();
      doSaveData = true;
    }
    if (sensors.at(i).isNextQuarterHour()) {
      sensors.at(i).pushLastQuarterHourReadings();
      doSaveData = true;
    }
    if (doSaveData) {
      sensors.at(i).saveData();
    }
  }
}

void doDisplaySetup() {
  if(!display.begin(0x02 /*|< Gen. display voltage from 3.3V*/, 0x3C)) { // Address 0x3D for 128x64
    Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 368 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 368 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
                  "SSD1306 allocation failed"
# 368 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 3
                  ); &__c[0];}))
# 368 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\hydro-9000-automatic-plant-watering\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
                  )));
    for(;;);
  }
  isDisplaySetup = true;
}

void drawHomescreen() {
  displayTitle("HYDRO 9000");
  //displaySensorReadings();
  display.drawBitmap(47, 20, LEAF_LOGO_BMP, 33, 44, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
}

void displayTitle(String title) {
  display.setTextSize(2);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  display.setCursor(0, 0);
  display.println(title);
}

void showMoistureLevels() {
  displayTitle("MOISTURE");
  display.setTextSize(1);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  for (int i = 0; i < sensors.size(); i++) {
    drawMoistureBar(sensors.at(i).getMoistureLevel(), i);
  }
  display.setCursor(10, 56);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
}

void showMoistureHistory(MILLIS_FROM durationMS, int intervalCount) {
  display.setCursor(0, 20);
  const int CHART_HEIGHT = 48;

  MoistureSensor currentSensor = sensors.at(0);

  int startY = display.height();
  int startX = display.width() - intervalCount;
  int intervalSizeMS = durationMS / intervalCount;
  int maxReading = currentSensor.maxReading, minReading = currentSensor.minReading;
  int readingInterval = (maxReading - minReading) / CHART_HEIGHT;

  if (true || durationMS <= MoistureSensor::HISTORY_SIZE * MILLIS_FROM::MINUTES) {
    for (int i = 0; i < intervalCount; i++) {
      display.drawPixel(startX + i, startY - (currentSensor.readingHistoryInMinutes[i] / readingInterval), 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
    }
  } else {

  }
}

void drawMoistureBar(float levelPercent, int16_t barIndex) {
  const int BAR_WIDTH = 23;
  const int BAR_MAX_HEIGHT = 48;
  const int BAR_MIN_HEIGHT = 1;
  const int BAR_SPACING = 1;
  const int START_X = 128-(BAR_SPACING + BAR_WIDTH) * 5;
  const int START_Y = 0;
  const int CHAR_WIDTH = 6, TEXT_HEIGHT = 7, TEXT_PADDING_Y = 3;

  int height = std::max(float(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
  int startX = barIndex * (BAR_WIDTH + BAR_SPACING) + START_X;
  int startY = display.height() - height;

  display.fillRect(startX, startY, BAR_WIDTH, height, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);

  int percentage = levelPercent * 100;
  int textY, textX;
  int textWidth, percentCharCount;

  if (percentage < 10) {
    percentCharCount = 2;
  } else if (percentage < 100) {
    percentCharCount = 3;
  } else if (percentage == 100) {
    percentCharCount = 4;
  }
  textWidth = percentCharCount * CHAR_WIDTH;
  textX = getCenterAlignPosition(startX, startX + BAR_WIDTH, textWidth);
  textY = startY + TEXT_PADDING_Y;

  if (textY + TEXT_HEIGHT + TEXT_PADDING_Y > display.height()) {
    textY = startY - TEXT_PADDING_Y - TEXT_HEIGHT;
    display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  } else {
    display.setTextColor(0 /*|< Draw 'off' pixels*/ /*|< Draw 'off' pixels*/);
  }
  display.setCursor(textX, textY);
  display.setTextSize(1);
  display.println(String(percentage)+"%");
}

int getCenterAlignPosition(int startPosition, int endPosition, int thingWidth) {
  int width = endPosition - startPosition + 1;
  return (width - thingWidth) / 2 + startPosition;
}

void showMoistureGoals() {
  displayTitle("MOISTURE");
  display.setTextSize(1);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  for (int i = 0; i < sensors.size(); i++) {
    drawMoistureGoal(sensors.at(i).getGoalPercentage(), i);
  }
  display.setCursor(10, 56);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
}

void drawMoistureGoal(float levelPercent, int16_t barIndex) {
  const int BAR_WIDTH = 23;
  const int BAR_MAX_HEIGHT = 48;
  const int BAR_MIN_HEIGHT = 1;
  const int BAR_SPACING = 1;
  const int START_X = 128-(BAR_SPACING + BAR_WIDTH) * 5;
  const int START_Y = 0;

  int height = std::max(float(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
  int startX = barIndex * (BAR_WIDTH + BAR_SPACING) + START_X;
  int startY = display.height() - height;


  display.fillRect(startX, startY, BAR_WIDTH, height, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);

  int textY, textPaddingX = 3;
  if (height < 13) {
    textY = fmin(startY - 10, display.height() - 10);
    display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  } else {
    textY = startY + 3;
    display.setTextColor(0 /*|< Draw 'off' pixels*/ /*|< Draw 'off' pixels*/);
  }
  int percentage = levelPercent * 100;
  if (percentage < 10) {
    textPaddingX += 3;
  }
  display.setCursor(startX+textPaddingX, textY);
  display.setTextSize(1);
  display.println(String(percentage)+"%");
}
