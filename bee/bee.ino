#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT   32
#define LOGO_WIDTH    54
#define buttonLeft 5
#define buttonRight 4

float start, finished;
float elapsed, time;
float circMetric=2.093; // wheel circumference (in meters)
float speedk;    // holds calculated speed vales in metric and imperial
long voltage;
boolean buttonStateLeft, buttonStateRight;


long readVcc() { long result; // Read 1.1V reference against AVcc 
ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); delay(2); // Wait for Vref to settle 
ADCSRA |= _BV(ADSC); // Convert 
while (bit_is_set(ADCSRA,ADSC)); 
result = ADCL; 
result |= ADCH<<8; 
result = 1125300L / result; // Back-calculate AVcc in mV 
return result; 
}

int redPin= 3;
int greenPin = 6;
int bluePin = 9;

static const unsigned char PROGMEM logo_bmp[] = {
  B00000000, B00000011, B00000000, B00000000, B00000000, B00000000, B000000,
  B00000000, B00000011, B11111000, B00000000, B00000000, B00000000, B000000,
  B00000000, B00000011, B11111000, B00000001, B11111000, B00000000, B000000,
  B00000000, B00000000, B11000000, B00000000, B11000000, B00000000, B000000,
  B00000000, B00000000, B01000000, B00000000, B01000000, B00000000, B000000,
  B00000000, B00000000, B01000000, B00000000, B01100000, B00000000, B000000,
  B00000000, B00000000, B01111111, B11111111, B11100000, B00000000, B000000,
  B00000000, B00000000, B11100000, B00000000, B00110000, B00000000, B000000,
  B00000000, B00000000, B10100000, B00000000, B00010000, B00000000, B000000,
  B00000000, B00000000, B10110000, B00000000, B00111000, B00000000, B000000,
  B00000000, B00000001, B10010000, B00000000, B01101000, B00000000, B000000,
  B00000000, B01110001, B00010000, B00000000, B11001001, B11000000, B000000,
  B00000011, B11111111, B00011000, B00000000, B10001111, B11111000, B000000,
  B00001110, B00000011, B10001000, B00000001, B10111100, B00001110, B000000,
  B00011000, B00000010, B11001000, B00000011, B01100110, B00000011, B000000,
  B00110000, B00000110, B01100100, B00000110, B11000010, B00000001, B100000,
  B01100000, B00000100, B00100100, B00000100, B10000011, B00000000, B110000,
  B01000000, B00001100, B00110110, B00001101, B10000001, B00000000, B010000,
  B11000000, B00001000, B00010010, B00011001, B00000001, B10000000, B011000,
  B11000000, B00001000, B00010010, B00110001, B00000000, B10000000, B011000,
  B11000000, B00011000, B00011011, B01100011, B00000000, B11000000, B011000,
  B10000000, B01111111, B10011001, B01100011, B00000000, B11100000, B001000,
  B10000000, B01110011, B11111111, B11000011, B00000000, B11000000, B001000,
  B11000000, B00000000, B00011000, B11000011, B00000000, B00000000, B011000,
  B11000000, B00000000, B00010000, B11000001, B00000000, B00000000, B011000,
  B01000000, B00000000, B00110001, B11100001, B10000000, B00000000, B010000,
  B01100000, B00000000, B00110000, B00000001, B10000000, B00000000, B110000,
  B00110000, B00000000, B01100000, B00000000, B11000000, B00000001, B100000,
  B00011000, B00000000, B11000000, B00000000, B01100000, B00000011, B000000,
  B00001100, B00000001, B10000000, B00000000, B00110000, B00000110, B000000,
  B00000111, B10011111, B00000000, B00000000, B00011111, B00111100, B000000,
  B00000001, B11111100, B00000000, B00000000, B00000111, B11110000, B000000
};

void setColor(int redValue, int greenValue, int blueValue) {
analogWrite(redPin, redValue);
analogWrite(greenPin, greenValue);
analogWrite(bluePin, blueValue);
}

void turnLeft() {
  for (int i = 0; i <= 5; i++) {
  setColor(0, 255, 0);
  delay (750);
  setColor(0, 0, 0);
  delay (750);
  }
  setColor(0, 0, 0);
}

void turnRight() {
  for (int i = 0; i <= 5; i++) {
  setColor(0, 0, 255);
  delay (750);
  setColor(0, 0, 0);
  delay (750);
  }
  setColor(0, 0, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.print("BEE Starting...");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  bootSplash();
  attachInterrupt(digitalPinToInterrupt(2), speedCalc, RISING); // interrupt called when sensors sends digital 2 high (every wheel rotation)
  start=millis();
  Serial.println("BEE started."); 
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonLeft,INPUT_PULLUP);
  pinMode(buttonRight,INPUT_PULLUP);
}

void loop() {
  float sensorValue = analogRead(A0);
  int voltage = round(sensorValue * 5);
  //Serial.print("Speed:");
  //Serial.println(int(speedk));
  //Serial.print("Voltage: ");
  //Serial.println(long(readVcc));
  //Serial.print("Battery: ");
  //Serial.print(voltage);
  display.clearDisplay();
  speedValue(int(speedk));    // Print the initial value of speed
  batteryStatus(readVcc); // Print the battery status
  display.display();
  buttonStateLeft = digitalRead(buttonLeft);
  if (buttonStateLeft == 0){
    turnLeft();
    Serial.print("Turn Left");
  }
  buttonStateRight = digitalRead(buttonRight);
  if (buttonStateRight == 0){
    turnRight();
    Serial.print("Turn Right");
  }
}

void bootSplash(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(4000);
}

void speedCalc()
{
  //Function called by the interrupt

  if((millis()-start)>100) // 100 millisec debounce
    {
    //calculate elapsed
    elapsed=millis()-start;

    //reset start
    start=millis();
  
    //calculate speed in km/h
    speedk=(3600*circMetric)/elapsed; 

    }
}

void speedValue(int speed) {
  display.setTextSize(1);
  display.setFont(&FreeSansBold18pt7b);
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,31);             // Start at top-left corner
  display.println(String(speed));
  display.setTextSize(1);
  display.setFont();
  display.setCursor(60,24);
  display.println(F("km/h"));
  display.setCursor(60,0);
}

void batteryStatus(long voltage) {
  display.fillRect(123,19,3,1,WHITE); //Draw battery end button
  if(voltage > 3700){display.fillRect(121,20,7,12,WHITE);} //full
  if(voltage > 3662 && voltage <=3700){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,22,5,9,WHITE);}
  if(voltage > 3624 && voltage <=3662){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,23,5,8,WHITE);}
  if(voltage > 3586 && voltage <=3624){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,24,5,7,WHITE);}
  if(voltage > 3548 && voltage <=3586){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,25,5,6,WHITE);}
  if(voltage > 3510 && voltage <=3548){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,26,5,5,WHITE);}
  if(voltage > 3472 && voltage <=3510){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,27,5,4,WHITE);}
  if(voltage > 3434 && voltage <=3472){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,28,5,3,WHITE);}
  if(voltage > 3396 && voltage <=3434){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,29,5,2,WHITE);}
  if(voltage > 3434 && voltage <=3396){display.drawRect(121,20,7,12,WHITE); display.fillRect(122,30,5,1,WHITE);}
}
