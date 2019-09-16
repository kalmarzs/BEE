/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

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

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

unsigned long prev, interval = 100; //Variables for display/clock update rate
byte flash = 0; //Flag for display flashing - toggle once per update interval
long voltage; //Variable for Vcc (battery) voltage in millivolts

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...
  
  display.clearDisplay();
  speedValue();    // Print the value of speed
  speedValueDecimal();    // Print the value of speed
  speedUnit();     // Print the unit of speed
  batteryStatus(); // Print the battery status
  display.display();
  delay(1000); 
}

void loop() {
}

void speedValue(void) {
  

  display.setTextSize(1);
  display.setFont(&FreeSansBold18pt7b);
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,31);             // Start at top-left corner
  display.println(F("25"));
}

void speedValueDecimal(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);        // Draw white text
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(37,31);             // Start at top-left corner
  display.println(F(".4"));
}

void speedUnit(void) {
  display.setTextSize(1);
  display.setFont();
  display.setCursor(60,24);
  display.println(F("km/h"));
}

void batteryStatus(void) {
  //unsigned long now = millis(); //Local variable set to current value of Arduino internal millisecond run-time timer

//Draw and update display every refresh period (100ms)
//if ((now - prev > interval)) { //Determine whether to start a voltage and screen update

  //if(flash == 0){flash = 1;}else{flash = 0;} //Toggle flash flag for icon blinking later
  voltage = 3800;
  //display.setCursor(100, 0); //Position text cursor for voltage display
  //display.print(voltage); //Write voltage to display buffer
  //display.print(" milliVolts");
  
  //Display 4-level battery gauge - flashes at lowest level
  display.fillRect(116,8,6,2,WHITE); //Draw battery end button
  //if(voltage > 3700){display.fillRect(114,10,10,22,WHITE);}
  if(voltage > 3700){display.drawRect(114,10,10,22,WHITE); display.fillTriangle(116,23,119,23,119,19,WHITE); display.fillTriangle(116,21,116,28,116,21,WHITE);}
  if(voltage > 3600 && voltage <=3700){display.drawRect(114,10,10,22,WHITE); display.fillRect(114,10,10,22,WHITE);}
  if(voltage > 3500 && voltage <=3600){display.drawRect(114,10,10,22,WHITE); display.fillRect(114,10,12,12,WHITE);}
  if(voltage > 3400 && voltage <=3500){display.drawRect(114,10,10,22,WHITE);}
  //if(voltage <=3400 && flash){display.drawRect(100,0,120,8,WHITE);} //Flash battery icon at lowest level
  //if(voltage <=3400 && flash){display.fillRect(120,3,102,2,WHITE);} //Flash battery end button
  
  //prev = now; //Reset variables for display and time update rate
  //}
}
