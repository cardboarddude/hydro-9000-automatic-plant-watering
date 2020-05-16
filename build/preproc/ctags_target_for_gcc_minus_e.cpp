# 1 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
# 2 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 3 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2

# 5 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 6 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 7 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 8 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2
# 9 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 2





# 13 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128 /* OLED display width, in pixels*/, 64 /* OLED display height, in pixels*/, &Wire, -1);
MoistureSensor sensor1(A0);
std::vector<MoistureSensor> sensors;
std::vector<int> sensorPins = {54, 55, 56, 57, 58};
bool isDisplaySetup = false;


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
  if(!display.begin(0x02 /*|< Gen. display voltage from 3.3V*/, 0x3C)) { // Address 0x3D for 128x64
    Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 51 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 51 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
                  "SSD1306 allocation failed"
# 51 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino" 3
                  ); &__c[0];}))
# 51 "c:\\Users\\ellio\\Documents\\Personal\\Projects\\Plant Water Pump\\arduino_mega2560_hydro9000_water_pump_controller\\arduino_mega2560_hydro9000_water_pump_controller.ino"
                  )));
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
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  display.setCursor(0, 0);
  display.println(title);
}

void displaySensorReadings() {
  display.setTextSize(1);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  for (int i = 0; i < sensors.size(); i++) {
    Serial.println(String(i)+": ("+String((i/5)*50)+", "+String(10*i%50+20)+")");
    display.setCursor((i/5)*50, 10*i%50+20);
    display.println(sensors.at(i).currentReading);
    drawMoistureBar(sensors.at(i).getMoistureLevel(), i);
  }
}

void testdrawrect(void) {

  for(int16_t i=0; i<display.height(); i++) {
    display.drawRect(i, 0, 2, display.height()-i, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
    display.display(); // Update screen with each newly-drawn rectangle
  }
  for(int16_t i=0; i<display.height(); i++) {
    display.drawRect(display.width()/2+i, 0, 2, i, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
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

  display.fillRect(startX, startY, BAR_WIDTH, height, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
}
