//___________________________________________________________   ROSITA FOOD DISPENSER 2021 JORGE QUEIROS AFONSO QUEIROS       _______________________________________________________
//___________________________________________________________   AVAILABLE AT https://github.com/AfonsoQueiros13/DogFeeder     _______________________________________________________

//___________________________________________________________                     LIBS IMPORT                                 _______________________________________________________
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Servo.h>
#include "SH1106Wire.h"   // legacy: 
#include "SH1106.h"

//___________________________________________________________                     GLOBAL VARIABLES                            _______________________________________________________
int hora_manha = 24, minuto_manha = 60, hora_noite = 24, minuto_noite = 60;

Servo myservo;
SH1106 display(0x3C, 5, 4); // For I2C
// Define NTP Client to get time
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org");

int ADC = 0;
int sensor = 0;
int currentHour;
int currentMinute;
boolean onetime = false;
boolean estado_temporizador_manha = false;
boolean estado_temporizador_noite = false;
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Assign output variables to GPIO pins
const int output12 = 12;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void server_connection(){
  
/* Set these to your desired credentials. */
const char * ssid = "ZonJQ"; //Enter your WIFI ssid
const char * password = "681994jq"; //Enter your WIFI password
// Connect to Wi-Fi network with SSID and password
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 19);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.config(local_IP, dns , gateway,subnet);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
// Print local IP address and start web server
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
Serial.print("ESP Mac Address: ");
Serial.println(WiFi.macAddress());
Serial.print("Subnet Mask: ");
Serial.println(WiFi.subnetMask());
Serial.print("Gateway IP: ");
Serial.println(WiFi.gatewayIP());
Serial.print("DNS: ");
Serial.println(WiFi.dnsIP());

server.begin();
}

void food_drop(){
  myservo.attach(4);  // attaches the servo on pin 14 to the servo object
  for(int i=0;i<4;i++){
  
    for (int pos = 0; pos <= 150; pos += 2) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (int pos = 150; pos >= 0; pos -=2) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  }
  myservo.detach();
}
void get_ntp_hour_and_minute(){  
  timeClient.update();
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();
  
}
void listen_app_commands(){
    WiFiClient client = server.available(); // Listen for incoming clients

 
  if ((hora_manha == currentHour && minuto_manha  == currentMinute && estado_temporizador_manha == true) || (hora_noite == currentHour && minuto_noite == currentMinute && estado_temporizador_noite == true)){
    if(onetime == false){
    digitalWrite(output12, 0);
    food_drop();
    onetime = true;
    } 
  }
  else{
    onetime = false;
    digitalWrite(output12, 1);
  }
  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /1sthour=") >= 0) {
              Serial.println("header= ");
              Serial.println(header.substring(5, 25));
              String str_hour = header.substring(5, 25);
              hora_manha = str_hour.substring(8, 10).toInt();
              minuto_manha = str_hour.substring(18,20).toInt();

              Serial.println(hora_manha);
              Serial.println(minuto_manha);
              Serial.println("finish");
              //output5State = "off";
              digitalWrite(output12, 1);
              estado_temporizador_manha = true;
            }

            if (header.indexOf("GET /2ndhour=") >= 0) {
              Serial.println("header= ");
              Serial.println(header.substring(5, 25));
              String str_hour = header.substring(5, 25);
              hora_noite = str_hour.substring(8, 10).toInt();
              minuto_noite = str_hour.substring(18, 20).toInt();

              Serial.println(hora_noite);
              Serial.println(minuto_noite);
              Serial.println("finish");
              //output5State = "off";
              digitalWrite(output12, 1);
              estado_temporizador_noite = true;
            }


           

            if (header.indexOf("GET /f") >= 0) {
              Serial.println("dar comida");
              //client.print("0");
              digitalWrite(output12, LOW);;
              food_drop();
            }

            if (header.indexOf("GET /estado_manha") >= 0) {
              client.print(estado_temporizador_manha);
            }
             if (header.indexOf("GET /estado_noite") >= 0) {
              client.print(estado_temporizador_noite);
            }

            if (header.indexOf("GET /1hour") >= 0) {
               if(estado_temporizador_manha == false)
              {
                client.print("Temporizador Desativado para a parte da manha");  
              }
              else{
                Serial.println("estado");
                if(hora_noite == 24  && minuto_noite == 60)
                  client.print("Hora Marcada (Parte da manha): Hora indefinida ");
                else{
                  client.print("Hora Marcada (Parte da manha): ");
                  client.print(hora_manha);
                  client.print(":");
                  client.print(minuto_manha);
                  Serial.println(header);
                }
              }
            }

             if (header.indexOf("GET /2hour") >= 0) {
              if(estado_temporizador_noite == false)
              {
                client.print("Temporizador Desativado para a parte da noite");  
              }
              else
              {
                if(hora_noite == 24  && minuto_noite == 60)
                  client.print("Hora Marcada (Parte da noite): Hora indefinida ");
                else
                {
                  client.print("Hora Marcada (Parte da tarde): ");
                  client.print(hora_noite);
                  client.print(":");
                  client.print(minuto_noite);
                  Serial.println(header);
                }
              }

            }
            if (header.indexOf("GET /Lmanha") >= 0) {
              estado_temporizador_manha = true;
              digitalWrite(output12, LOW);

            }
            if (header.indexOf("GET /Lnoite") >= 0) {
              estado_temporizador_noite = true;
              digitalWrite(output12, LOW);

            }
             if (header.indexOf("GET /Dmanha") >= 0) {
              estado_temporizador_manha = false;
              digitalWrite(output12, HIGH);
              
            }
            
            if (header.indexOf("GET /Dnoite") >= 0) {
              estado_temporizador_noite = false;
              digitalWrite(output12, HIGH);
              
            }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }

      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

}
void setup() {
  Serial.begin(115200);
  server_connection();
  // Initialize the output variables as outputs
  pinMode(output12, OUTPUT);
  digitalWrite(output12, LOW);
  //begin ntp client
  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void loop() {
  get_ntp_hour_and_minute();
  listen_app_commands();
}
