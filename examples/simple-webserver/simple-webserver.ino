#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RGBWWLed.h>

//RGBWW Pins
#define BLUEPIN 14
#define GREENPIN 12
#define REDPIN 13
#define WWPIN 5
#define CWPIN 4

ESP8266WebServer server;
RGBWWLed rgbled;
const char* ssid = "........";
const char* password = "........";



void handleRGBWW(){
  int rgbw[5];
  Serial.println(F("RGB request"));
  if (server.hasArg("r")){
    rgbw[0] = server.arg("r").toInt();
  }
  if (server.hasArg("g")){
    rgbw[1] = server.arg("g").toInt();
  }
  if (server.hasArg("b")){
    rgbw[2] = server.arg("b").toInt();
  }
  if (server.hasArg("ww")){
    rgbw[3] = server.arg("ww").toInt();
  }
  if (server.hasArg("cw")){
    rgbw[4] = server.arg("cw").toInt();
  }
  rgbled.setOutputRaw( rgbw[0], rgbw[1], rgbw[2], rgbw[3], rgbw[4]);
  server.send(200, "text/plain", "ok");
}

void handleHSV() {
  float hue, sat , val;
  int kelvin;
  Serial.println(F("HSV request"));
  if (server.hasArg("h") && server.hasArg("s") && server.hasArg("v")){
    hue = server.arg("h").toFloat();
    sat = server.arg("s").toFloat();
    val = server.arg("v").toFloat();
    if (server.hasArg("k")) {
      kelvin = server.arg("k").toInt();
    }
    HSVCT color = HSVCT(hue, sat, val, kelvin);
    rgbled.setHSV(color);
    server.send(200, "text/plain", "ok");
    
  } else {
  
    server.send(200, "text/plain", "missing params");
  }
}

void handleHSVtransition() {
  float hue, sat , val;
  int tm;
  bool shortway = true;
  Serial.println(F("HSV transition request"));
  if (server.hasArg("h") && server.hasArg("s") && server.hasArg("v")){
    hue = server.arg("h").toFloat();
    sat = server.arg("s").toFloat();
    val = server.arg("v").toFloat();
    HSVCT color(hue, sat, val);
    if (server.hasArg("l")) {
      shortway = false;
    }
    if (server.hasArg("tm")) {
      Serial.println("with time");
      tm = server.arg("tm").toInt();
      server.send(200, "text/plain", "ok");
      
      rgbled.setHSV(color, tm, shortway);
    } else {
      server.send(200, "text/plain", "ok");
      rgbled.setHSV(color);
    } 
  } else {
  
    server.send(200, "text/plain", "missing param");
  }
  
}



void handleswitchMode() {
  Serial.println(F("switchmode request"));
  if (server.hasArg("m")){
    String mode = server.arg("m");
    if (mode.indexOf("RGBWWCW") != -1) {
      rgbled.colorutils.setColorMode(RGBWW_COLORMODE::RGBWWCW);
    } else if (mode.indexOf("RGBCW") != -1) {
      rgbled.colorutils.setColorMode(RGBWW_COLORMODE::RGBCW);
    } else if (mode.indexOf("RGBWW") != -1) {
      rgbled.colorutils.setColorMode(RGBWW_COLORMODE::RGBWW);
    } else {
      rgbled.colorutils.setColorMode(RGBWW_COLORMODE::RGB);   
    }
  }
  server.send(200, "text/plain", "ok");
}





void setup() {
  //Serial
  Serial.begin(115200);
  Serial.println(F("Booting"));
  WiFi.begin(ssid, password);
  Serial.println("");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Init Webserver
  server.on("/rgbww", handleRGBWW);
  server.on("/hsv", handleHSV);
  server.on("/hsvt", handleHSVtransition);
  server.on("/mode", handleswitchMode);

  
  server.begin();
  
  
  //Init RGBLED
  rgbled.init(REDPIN, GREENPIN, BLUEPIN, WWPIN, CWPIN);
  Serial.println(F("RGBWW Controller ready"));

}


void loop() {
  
  server.handleClient();
  rgbled.show();
  
  //short break for esp to catch up
  delay(1);
}