#line 1 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <MoistureSensor.h>
#include <Wire.h>
#include <ArduinoSTL.h>
#include <vector>
#include <math.h> 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MoistureSensor sensor1(A0);
std::vector<MoistureSensor> sensors;
std::vector<int> sensorPins = {54, 55, 56, 57, 58};
bool isDisplaySetup = false;


#line 21 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void setup();
#line 31 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void loop();
#line 41 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void readAllSensors();
#line 49 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void doDisplaySetup();
#line 57 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void drawDisplay();
#line 63 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void displayTitle(String title);
#line 70 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void displaySensorReadings();
#line 81 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void testdrawrect(void);
#line 97 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void drawMoistureBar(float levelPercent, int16_t barIndex);
#line 21 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  for (int i = 0; i < sensorPins.size(); i++) {
    sensors.push_back(new MoistureSensor(sensorPins[i]));
  }
  doDisplaySetup();
}

void loop() {
  if (!isDisplaySetup) {
    doDisplaySetup();
  }
  readAllSensors();
  drawDisplay();
  delay(250);
  display.clearDisplay();
}

void readAllSensors() {
  Serial.println("Reading from all sensors, length: "+String(sensors.size()));
  for (int i = 0; i < sensors.size(); i++) {
    Serial.println("Reading from sensor "+String(i)+".");
    sensors[i].read();
  }
}

void doDisplaySetup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  isDisplaySetup = true;
}

void drawDisplay() {
  displayTitle("HYDRO 9000");
  displaySensorReadings();
  display.display();
}

void displayTitle(String title) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(title);
}

void displaySensorReadings() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  for (int i = 0; i < sensors.size(); i++) {
    Serial.println(String(i)+": ("+String((i/5)*50)+", "+String(10*i%50+20)+")");
    display.setCursor((i/5)*50, 10*i%50+20);
    display.println(sensors.at(i).currentReading);
    drawMoistureBar(sensors.at(i).getMoistureLevel(), i);
  }
}

void testdrawrect(void) {

  for(int16_t i=0; i<display.height(); i++) {
    display.drawRect(i, 0, 2, display.height()-i, WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
  }
  for(int16_t i=0; i<display.height(); i++) {
    display.drawRect(display.width()/2+i, 0, 2, i, WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
  display.clearDisplay();
}

void drawMoistureBar(float levelPercent, int16_t barIndex) {
  const int BAR_WIDTH = 5;
  const int BAR_MAX_HEIGHT = 48;
  const int BAR_MIN_HEIGHT = 1;
  const int BAR_SPACING = 1;
  const int START_X = 90;
  const int START_Y = 0;

  int height = std::max(float(BAR_MIN_HEIGHT), levelPercent * BAR_MAX_HEIGHT);
  int startX = barIndex * (BAR_WIDTH + BAR_SPACING) + START_X;
  int startY = display.height() - height;
  
  display.fillRect(startX, startY, BAR_WIDTH, height, WHITE);
}

