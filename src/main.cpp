#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid = "Nothing Phone (2a)_6332";
const char* password = "nahi pata";

const int relayPin = D4;                // Pin to which relay is connected
const int buttonPin = D2;               // Pin to wh-ich button is connected

bool relayState = false;
WiFiServer server(80);

// Function prototypes
void handleClient();
void handleRoot();
void handleOn();
void handleOff();
void saveState();

void setup() {
  Serial.begin(115200);
  EEPROM.begin(1);

  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Restore relay state from EEPROM
  relayState = EEPROM.read(0);
  digitalWrite(relayPin, relayState ? LOW : HIGH);

  // If EEPROM does not have a valid state, set relayState to false
  if (relayState != true && relayState != false) {
    relayState = false;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  handleClient(); // Handle client requests

  // Check if physical button is pressed to toggle relay state
  if (digitalRead(buttonPin) == LOW) {
    delay(50); // debounce
    if (digitalRead(buttonPin) == LOW) {
      relayState = !relayState;
      digitalWrite(relayPin, relayState ? LOW : HIGH);
      saveState();
      Serial.println(relayState ? "Relay turned on" : "Relay turned off");
      delay(1000); // delay to avoid multiple detections
    }
  }
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("/on") != -1) {
      handleOn();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("");
      client.println("Relay turned on");
    } 
    else if (request.indexOf("/off") != -1) {
      handleOff();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("");
      client.println("Relay turned off");
    } 
    else {
      handleRoot();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      
      String html = "<!DOCTYPE html>";
      html += "<html>";
      html += "<head>";
      html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
      html += "<style>";
      html += "body {";
      html += "  font-family: Arial, sans-serif;";
      html += "  display: flex;";
      html += "  justify-content: center;";
      html += "  align-items: center;";
      html += "  height: 100vh;";
      html += "  margin: 0;";
      html += "  background: linear-gradient(to right, #00c6ff, #0072ff);";
      html += "  color: #fff;";
      html += "}";
      html += ".container {";
      html += "  text-align: center;";
      html += "}";
      html += ".title {";
      html += "  font-size: 2.5em;";
      html += "  margin-bottom: 20px;";
      html += "}";
      html += ".switch {";
      html += "  position: relative;";
      html += "  display: inline-block;";
      html += "  width: 120px;";
      html += "  height: 60px;";
      html += "}";
      html += ".switch input {";
      html += "  opacity: 0;";
      html += "  width: 0;";
      html += "  height: 0;";
      html += "}";
      html += ".slider {";
      html += "  position: absolute;";
      html += "  cursor: pointer;";
      html += "  top: 0;";
      html += "  left: 0;";
      html += "  right: 0;";
      html += "  bottom: 0;";
      html += "  background-color: #ccc;";
      html += "  transition: .4s;";
      html += "  border-radius: 30px;";
      html += "}";
      html += ".slider:before {";
      html += "  position: absolute;";
      html += "  content: '';";
      html += "  height: 50px;";
      html += "  width: 50px;";
      html += "  border-radius: 50%;";
      html += "  left: 5px;";
      html += "  bottom: 5px;";
      html += "  background-color: white;";
      html += "  transition: .4s;";
      html += "}";
      html += "input:checked + .slider {";
      html += "  background-color: #4CAF50;";
      html += "}";
      html += "input:checked + .slider:before {";
      html += "  transform: translateX(60px);";
      html += "}";
      html += ".slider .label-on, .slider .label-off {";
      html += "  position: absolute;";
      html += "  top: 50%;";
      html += "  transform: translateY(-50%);";
      html += "  font-size: 1em;";
      html += "  color: #fff;";
      html += "  user-select: none;";
      html += "}";
      html += ".slider .label-on {";
      html += "  left: 10px;";
      html += "}";
      html += ".slider .label-off {";
      html += "  right: 10px;";
      html += "}";
      html += "</style>";
      html += "<script>";
      html += "function toggleRelay() {";
      html += "  var xhttp = new XMLHttpRequest();";
      html += "  var state = document.getElementById('relayToggle').checked ? '/on' : '/off';";
      html += "  xhttp.open('GET', state, true);";
      html += "  xhttp.send();";
      html += "}";
      html += "</script>";
      html += "</head>";
      html += "<body>";
      html += "<div class='container'>";
      html += "  <div class='title'>Smart Switch</div>";
      html += "  <label class='switch'>";
      html += "    <input type='checkbox' id='relayToggle' onclick='toggleRelay()' ";
      html += (relayState ? "checked" : "");
      html += ">";
      html += "    <span class='slider'>";
      html += "      <span class='label-on'>ON</span>";
      html += "      <span class='label-off'>OFF</span>";
      html += "    </span>";
      html += "  </label>";
      html += "</div>";
      html += "</body>";
      html += "</html>";

      client.print(html);
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}



void handleRoot() {
  Serial.println("Handling root");
  // Handle root request (show control options)
}

void handleOn() {
  Serial.println("Handling /on");
  digitalWrite(relayPin, LOW);
  relayState = true;
  saveState();
}

void handleOff() {
  Serial.println("Handling /off");
  digitalWrite(relayPin, HIGH);
  relayState = false;
  saveState();
}

void saveState() {
  EEPROM.write(0, relayState);
  EEPROM.commit();
  Serial.println("State saved to EEPROM");
}
