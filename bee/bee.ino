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
#define NUM_SAMPLES 10

//speed measurement
float start, finished, elapsed, time, speedk;
float circMetric=2.093; // wheel circumference (in meters)

//battery measurement
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage

//turn signal
int redPin= 3;
int greenPin = 6;
int bluePin = 9;
boolean buttonStateLeft, buttonStateRight;

//headlight
const int pResistor = A0; // Photoresistor at Arduino analog pin A0
const int headLightPin=12;       // Led pin at Arduino pin 12
int value;

//bootsplash image
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

void setup() {
  Serial.begin(115200);
  Serial.print("BEE Starting...");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  bootSplash();
  digitalWrite(headLightPin, HIGH);
  delay(500);
  digitalWrite(headLightPin, LOW);
  delay(500);
  attachInterrupt(digitalPinToInterrupt(2), speedCalc, RISING); // interrupt called when sensors sends digital 2 high (every wheel rotation)
  start=millis();
  Serial.println("BEE started."); 
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonLeft,INPUT_PULLUP);
  pinMode(buttonRight,INPUT_PULLUP);
  daytimeRunningLight();
}

void loop() {
  display.clearDisplay();
  speedValue(int(speedk));    // Print the initial value of speed
  while (sample_count < NUM_SAMPLES) {
        sum += analogRead(A1);
        sample_count++;
        delay(10);
    }
  voltage = ((float)sum / (float)NUM_SAMPLES * 5.015);
  batteryStatus(voltage); // Print the battery status
  sample_count = 0;
  sum = 0;
  display.display();
  
  buttonStateLeft = digitalRead(buttonLeft);
  if (buttonStateLeft == 0){
    turnLeft();
  }
  
  buttonStateRight = digitalRead(buttonRight);
  if (buttonStateRight == 0){
    turnRight();
  }
  
  headLight();
}


///subroutines

void setColor(int redValue, int greenValue, int blueValue) {
analogWrite(redPin, redValue);
analogWrite(greenPin, greenValue);
analogWrite(bluePin, blueValue);
}

void daytimeRunningLight() {
  setColor(255,255,255);
}

void turnLeft() {
  setColor(0,0,0);
  delay(200);
  for (int i = 0; i <= 5; i++) {
  setColor(0, 255, 0);
  delay (750);
  setColor(0, 0, 0);
  delay (750);
  }
  daytimeRunningLight();
}

void turnRight() {
  setColor(0,0,0);
  delay(200);
  for (int i = 0; i <= 5; i++) {
  setColor(0, 0, 255);
  delay (750);
  setColor(0, 0, 0);
  delay (750);
  }
  daytimeRunningLight();
}

void headLight() {
  value = analogRead(pResistor);
  if (value < 250){
    digitalWrite(headLightPin, HIGH);  //Turn led off
  }
  else{
    digitalWrite(headLightPin, LOW); //Turn led on
  }
  delay(500); //Small delay
}

void bootSplash(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(2000);
}

void speedCalc()
{
  if((millis()-start)>100) // 100 millisec debounce
    {
    elapsed=millis()-start;
    start=millis();
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
  if(voltage <=3396 ){display.drawRect(121,20,7,12,WHITE);}
}
