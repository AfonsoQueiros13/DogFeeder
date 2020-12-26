// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

int hora;
int minuto;
int ADC=0;
int sensor=0;

/* Set these to your desired credentials. */
const char *ssid = "NOS_Internet_5533"; //Enter your WIFI ssid
const char *password = "91293068"; //Enter your WIFI password
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output16 = 16;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(output16, OUTPUT);
  digitalWrite(output16, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
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
  server.begin();
  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void loop(){
  timeClient.update();
  int currentHour = timeClient.getHours();
  //Serial.print("Hour: ");
 // Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  //Serial.print("Minutes: ");
  //Serial.println(currentMinute); 

  WiFiClient client = server.available();   // Listen for incoming clients
 

  ////Serial.println("hora def = ");
  //Serial.print(hora);
  //Serial.println("minutos def = ");
  //Serial.print(minuto);

  
   if(hora == currentHour && minuto == currentMinute)
    digitalWrite(LED_BUILTIN, 0);
   else
    digitalWrite(LED_BUILTIN, 1);
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /hour=") >= 0) {
              Serial.println("header= ");
              Serial.println(header.substring(5,22));
              String teste = header.substring(5,22);
              hora = teste.substring(5,7).toInt();
              minuto = teste.substring(15,17).toInt();
              
              Serial.println(hora);
              Serial.println(minuto);
              Serial.println("finish");
              //output5State = "off";
              digitalWrite(LED_BUILTIN, 1);
              

              }

              if (header.indexOf("GET /D") >= 0) {
               Serial.println("desligado");
               client.print("Dispensador desligado");
               digitalWrite(output16, HIGH);;
               delay(1000);                // waits for a second
              }

               if (header.indexOf("GET /estado") >= 0) {
               Serial.println("estado");
               client.print("ESTADO DISPENSADOR: OFF");
               digitalWrite(output16, HIGH);;
               delay(1000);                // waits for a second
              }

               if (header.indexOf("GET /hour") >= 0) {
               Serial.println("estado");
                client.print("Hora Marcada");
                client.print(hora);
                client.print(":");
                client.print(minuto);
                Serial.println(header);
            
              }

               if (header.indexOf("GET /L") >= 0) {
                Serial.println("ligado");
                digitalWrite(output16, LOW);

              }

               if (header.indexOf("GET /adc") >= 0) {
                sensor= analogRead(ADC);
                 client.print(sensor);
              }
              
            
            
           
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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
