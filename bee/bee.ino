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

unsigned long prev, interval = 100; //Variables for display/clock update rate
byte flash = 0; //Flag for display flashing - toggle once per update interval
long voltage; //Variable for Vcc (battery) voltage in millivolts

int hall_pin = 12;
// set number of hall trips for RPM reading (higher improves accuracy)
float hall_thresh = 100.0;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }



  display.clearDisplay();
  speedValue(0);    // Print the value of speed
  //speedValueDecimal();    // Print the value of speed
  speedUnit();     // Print the unit of speed
  batteryStatus(); // Print the battery status
  display.display(); 
}

void loop() {

float hall_count = 1.0;
  float start = micros();
  bool on_state = false;
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
  while(true){
    //Serial.print(digitalRead(hall_pin));
    if (digitalRead(hall_pin)==0){
      if (on_state==false){
        on_state = true;
        hall_count+=1.0;
      }
    } else{
      on_state = false;
    }
    
    if (hall_count>=hall_thresh){
      break;
    }
  }
  
  // print information about Time and RPM
  float end_time = micros();
  float time_passed = ((end_time-start)/1000000.0);
  Serial.print("Time Passed: ");
  Serial.print(time_passed);
  Serial.println("s");
  float rpm_val = (hall_count/time_passed)*60.0;
  Serial.print(rpm_val);
  Serial.println(" RPM");
  display.clearDisplay();
  speedValue(rpm_val);
  display.display(); 
  delay(1);        // delay in between reads for stability
}

//void main(void) {
  
//}

void speedValue(int speed) {
  display.setTextSize(1);
  display.setFont(&FreeSansBold18pt7b);
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,31);             // Start at top-left corner
  display.println(String(speed));
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
  voltage = 3541;
  //display.setCursor(100, 0); //Position text cursor for voltage display
  //display.print(voltage); //Write voltage to display buffer
  //display.print(" milliVolts");
  
  //Display 3-level battery gauge - flashes at lowest level
  display.fillRect(123,19,3,1,WHITE); //Draw battery end button
  //if(voltage > 3700){display.fillRect(114,10,10,22,WHITE);}
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
  //if(voltage <=3395 && flash){display.drawRect(100,0,120,8,WHITE);} //Flash battery icon at lowest level
  //if(voltage <=3395 && flash){display.fillRect(120,3,102,2,WHITE);} //Flash battery end button
  
  //prev = now; //Reset variables for display and time update rate
  //}
}
