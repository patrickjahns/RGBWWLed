#include <ESP8266WiFi.h>
#include <RGBWWLed.h>

#define BLUEPIN 14
#define GREENPIN 12
#define REDPIN 13
#define WWPIN 5
#define CWPIN 4

RGBWWLed rgbwwctrl;

void saveRGBWW(RGBWWLed* rgbwwctrl) {

  HSVK c = rgbwwctrl->getCurrentColor();
  Serial.println("Current color");
  Serial.print("H:");
  Serial.print(c.h);
  Serial.print("  S:+");
  Serial.print(c.s);
  Serial.print("  V:");
  Serial.print(c.v);
  Serial.print("  K:");
  Serial.println(c.k);
  
}

void setup() {
  Serial.println("Setup");
  //initialize 
  rgbwwctrl.init(REDPIN, GREENPIN, BLUEPIN, WWPIN, CWPIN);
    
  //set callback function
  rgbwwctrl.setAnimationCallback(saveRGBWW);
  
  //change colormmode to RGBWWCW
  rgbwwctrl.colorutils.setColorMode(RGBWW_COLORMODE::RGBWWCW);
    
  //show red as initial color
  HSVK color = HSVK(0, 100, 100);
  rgbwwctrl.setHSV(color);
  
  
}

void loop() {
    rgbwwctrl.show();
    delay(10);
}