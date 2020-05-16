/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
}

void loop() {
  displayTitle();
  displaySensorReadings();
  display.display(); 
  delay(250);
  display.clearDisplay();
}

void displayTitle() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("HYDRO 9000");
}

void displaySensorReadings() {
  // read the input on analog pin 0:
  int sensorValue1 = analogRead(A0);
  int sensorValue2 = analogRead(A1);
  int sensorValue3 = analogRead(A2);
  int sensorValue4 = analogRead(A3);
  int sensorValue5 = analogRead(A4);
  int sensorValue7 = analogRead(A7);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println(sensorValue1 * (5.0 / 1023.0));
  drawMoistureBar((3.4-(sensorValue1 * (5.0 / 1023.0)))/3.4, 0);
  display.setCursor(0, 30);
  display.println(sensorValue2 * (5.0 / 1023.0));
  drawMoistureBar((3.4-(sensorValue2 * (5.0 / 1023.0)))/3.4, 1);
  display.setCursor(0, 40);
  display.println(sensorValue3 * (5.0 / 1023.0));
  drawMoistureBar((3.4-(sensorValue3 * (5.0 / 1023.0)))/3.4, 2);
  display.setCursor(0, 50);
  display.println(sensorValue4 * (5.0 / 1023.0));
  drawMoistureBar((3.4-(sensorValue4 * (5.0 / 1023.0)))/3.4, 3);
  display.setCursor(50, 20);
  display.println(sensorValue5 * (5.0 / 1023.0));
  drawMoistureBar((3.4-(sensorValue5 * (5.0 / 1023.0)))/3.4, 4);
  drawMoistureBar((3.4-(sensorValue7 * (5.0 / 1023.0)))/3.4, 5);
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

void drawMoistureBar(float level, int16_t index) {
  int height = level * 50;
  int col = 0;
  switch (index) {
    case 0: col = 100; break;
    case 1: col = 104; break;
    case 2: col = 108; break;
    case 3: col = 112; break;
    case 4: col = 116; break;
    case 5: col = 120; break;
  }
  
  display.fillRect(col, display.height()-height, 4, height, WHITE);
}
