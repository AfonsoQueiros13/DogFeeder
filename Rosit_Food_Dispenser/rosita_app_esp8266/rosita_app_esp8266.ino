#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
/* Set these to your desired credentials. */
const char *ssid = "NOS_Internet_5533"; //Enter your WIFI ssid
const char *password = "91293068"; //Enter your WIFI password
ESP8266WebServer server(80);
void handleSave() {
 if (server.arg("pass") != "") {
   Serial.println(server.arg("pass"));
 }
}
void setup() {
 pinMode(LED_BUILTIN, OUTPUT);
 delay(3000);
 Serial.begin(115200);
 Serial.println();
 Serial.print("Configuring access point...");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
// server.on ( "/", handleRoot );
 server.on ("/save", handleSave);
 server.begin();
 Serial.println ( "HTTP server started" );

 server.on("/L", []() {
   digitalWrite(LED_BUILTIN, 0);
   Serial.println("on");
 //  handleRoot();
 });

 server.on("/D", []() {
   digitalWrite(LED_BUILTIN, 1);
   Serial.println("off");
//   handleRoot();
 });
}

void loop() {
 server.handleClient();
 Serial.println( server.print());


 
  }
