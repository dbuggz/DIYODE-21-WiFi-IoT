/*
 * Dave
 * This sketch is based on the ESP8266 WiFiWebServer example with functions from 
 * DIYODE #21 "iot_server_web_app_sequencing.ino"
 * refer to article at:
 * https://diyodemag.com/education/fundamentals_part_3_web_app_building_blocks
 * 
 * it controls three LEDs in conjunction with a web app (javascript) that sends GET requests
 * 
 * USE: 
 *     run with Serial Monitor and note the address,
 *     open the WWW app_web_package/index.html  and enter the adddress in 'settings'
 *     Choose an action (e.g RED ON) and press submit
 * your LED should fade up or down
 * 
 * Motivation: I wanted a WiFi Connection instead of Wired Ethernet
 */

#include <ESP8266WiFi.h>

const char* ssid = "SOMETHING";
const char* password = "SOMEPASSWORD";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

// LEDs to control
const int red = 5;  // Red LED, D1 on NodeMCU/WeMos
const int yel = 4;  // Yellow LED, D2
const int grn = 2;  // Green LED, D4
// declararions for main loop
char linebuf[80];
int charcount = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

// dave output some info about what program is running
// this is usefull if you come back and wonder what you last programmed into your chip
  String compiled = "";
  String filenm = "";

  filenm = __FILE__ ;
  Serial.println(filenm);
  
  compiled = __DATE__ ;
  compiled = compiled + " " + __TIME__ ;
  Serial.print("Complied at ");
  Serial.println(compiled);
  Serial.println();
  // end outputing file info

  // prepare GPIOs, I'm not sure this is needed.. but for completeness
  pinMode(red, OUTPUT);
  digitalWrite(red, 0);
  pinMode(yel, OUTPUT);
  digitalWrite(yel, 0);
  pinMode(grn, OUTPUT);
  digitalWrite(grn, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // perform some LED actions to indicate program is running and wiring is OK
  fadeMyLED(red);
  fadeMyLED(yel);
  fadeMyLED(grn);

}

// loop copied from DIYODE code, only change is WiFiClient instead of EthernetClient
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available(); //  
  if (client) {
    memset(linebuf, 0, sizeof(linebuf));
    charcount = 0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //read char by char HTTP request
        linebuf[charcount] = c;
        if (charcount < sizeof(linebuf) - 1) charcount++;
        if (c == '\n' && currentLineIsBlank) {
          break;
        }
        if (c == '\n') {
          if (strstr(linebuf, "GET /") > 0) {
          // send standard response headers, so our browser knows what to do
          client.println("HTTP/1.1 200 OK");
          client.println("Access-Control-Allow-Origin: *");

          if (strstr(linebuf, "GET /red1") > 0) { fadeIn(red); }
          if (strstr(linebuf, "GET /red0") > 0) { fadeOut(red); }
          if (strstr(linebuf, "GET /yel1") > 0) { fadeIn(yel); }
          if (strstr(linebuf, "GET /yel0") > 0) { fadeOut(yel); }
          if (strstr(linebuf, "GET /grn1") > 0) { fadeIn(grn); }
          if (strstr(linebuf, "GET /grn0") > 0) { fadeOut(grn); }

//          client.println("<!DOCTYPE HTML><html><head></head><body>Arduino Server!</body></html>");
          }

          currentLineIsBlank = true;
          memset(linebuf, 0, sizeof(linebuf));
          charcount = 0;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

// LED functions from DIYODE article

void fadeMyLED(int ledPin) {
  Serial.print("fading ");
  Serial.println(ledPin);
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

void fadeIn(int ledPin) {
  Serial.print("fading in ");
  Serial.println(ledPin);
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    analogWrite(ledPin, fadeValue);
    delay(30);
  }
}

void fadeOut(int ledPin) {
  Serial.print("fading out ");
  Serial.println(ledPin);
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    analogWrite(ledPin, fadeValue);
    delay(30);
  }
}
