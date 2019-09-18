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

float start, finished;
float elapsed, time;
float circMetric=2.093; // wheel circumference (in meters)
float circImperial; // using 1 kilometer = 0.621371192 miles
float speedk, speedm;    // holds calculated speed vales in metric and imperial

void setup() {
  Serial.begin(115200);
  Serial.println("BEE Starting...");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  circImperial=circMetric*.62137;
  attachInterrupt(digitalPinToInterrupt(2), speedCalc, RISING); // interrupt called when sensors sends digital 2 high (every wheel rotation)
  start=millis();
  Serial.println("BEE started."); 
}

void loop() {
  Serial.print("Speed:");
  Serial.println(int(speedk));
  display.clearDisplay();
  speedValue(int(speedk));    // Print the initial value of speed
  speedUnit();     // Print the unit of speed
  batteryStatus(3454); // Print the battery status
  display.display();
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

    //calculate speed in mph
    speedm=(3600*circImperial)/elapsed; 
    }
}

void speedValue(int speed) {
  display.setTextSize(1);
  display.setFont(&FreeSansBold18pt7b);
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,31);             // Start at top-left corner
  display.println(String(speed));
}

void speedUnit(void) {
  display.setTextSize(1);
  display.setFont();
  display.setCursor(60,24);
  display.println(F("km/h"));
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
