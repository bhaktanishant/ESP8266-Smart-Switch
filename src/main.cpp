#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid = "ESP8266_Hotspot"; // Hotspot name
const char* password = ""; // No password

const int relayPin = D4; // Pin to which relay is connected

bool relayState = true; // Initialize relayState to true
WiFiServer server(80);

int powerOnCount = 0; // Counter for power-on events
const int powerOnCountAddress = 1; // EEPROM address to store powerOnCount

// Function prototypes
void handleClient();
void handleOn();
void handleOff();
void saveState();
void checkPowerOnCount();

void setup() {
    Serial.begin(115200);
    EEPROM.begin(1);

    pinMode(relayPin, OUTPUT);

    // Restore relay state from EEPROM
    relayState = EEPROM.read(0);
    if (relayState != 0 && relayState != 1) {
        relayState = 1; // Default to ON if EEPROM has invalid data
    }
    digitalWrite(relayPin, relayState ? LOW : HIGH); // HIGH turns ON the relay

    // Restore powerOnCount from EEPROM
    powerOnCount = EEPROM.read(powerOnCountAddress);
    powerOnCount++; // Increment powerOnCount
    EEPROM.write(powerOnCountAddress, powerOnCount); // Save updated count to EEPROM
    EEPROM.commit(); // Commit changes

    // Check if the power-on limit has been reached
    checkPowerOnCount(); 

    // Set up Wi-Fi hotspot
    WiFi.softAP(ssid, password);
    Serial.println("Hotspot started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    handleClient(); // Handle client requests
}

void handleClient() {
    WiFiClient client = server.accept(); // Check for client
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
        } else if (request.indexOf("/off") != -1) {
            handleOff();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("");
            client.println("Relay turned off");
        } else {
            // Return simplified HTML page
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("");

            String html = "<!DOCTYPE html>";
            html += "<html>";
            html += "<head>";
            html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
            html += "<style>body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }</style>";
            html += "</head>";
            html += "<body>";
            html += "<h2>Smart Switch</h2>";
            html += "<label class='switch'>";
            html += "  <input type='checkbox' id='relayToggle' onclick='toggleRelay()' " + String(relayState ? "checked" : "") + ">";
            html += "  <span class='slider'></span>";
            html += "</label>";
            html += "<script>";
            html += "function toggleRelay() {";
            html += "  var xhttp = new XMLHttpRequest();";
            html += "  var state = document.getElementById('relayToggle').checked ? '/on' : '/off';";
            html += "  xhttp.open('GET', state, true);";
            html += "  xhttp.send();";
            html += "}";
            html += "</script>";
            html += "</body>";
            html += "</html>";

            client.print(html);
        }

        client.stop();
        Serial.println("Client disconnected");
    }
}

void handleOn() {
    Serial.println("Handling /on");
    if (!relayState) { // Only change state if it's currently off
        digitalWrite(relayPin, LOW); // Set to HIGH to turn ON the relay
        relayState = true;
        saveState();
    }
}

void handleOff() {
    Serial.println("Handling /off");
    if (relayState) { // Only change state if it's currently on
        digitalWrite(relayPin, HIGH); // Set to LOW to turn OFF the relay
        relayState = false;
        saveState();
    }
}

void saveState() {
    // Check if the relay state has changed before writing to EEPROM
    if (EEPROM.read(0) != relayState) {
        EEPROM.write(0, relayState);
        EEPROM.commit();
        Serial.println("Relay state saved to EEPROM");
    }
    
    // Reset powerOnCount only if it has changed
    int currentPowerOnCount = EEPROM.read(powerOnCountAddress);
    if (currentPowerOnCount != 0) {
        powerOnCount = 0; // Reset count when state changes
        EEPROM.write(powerOnCountAddress, powerOnCount); // Save reset count to EEPROM
        EEPROM.commit(); // Commit changes
        Serial.println("PowerOnCount reset in EEPROM");
    }
}

void checkPowerOnCount() {
    if (powerOnCount >= 5) {
        Serial.println("Power on limit reached. Resetting powerOnCount and turning relay ON...");
        powerOnCount = 0; // Reset the powerOnCount
        relayState = true; // Set relayState to ON
        digitalWrite(relayPin, LOW); // Turn the relay ON
        
        // Only write if there's a change
        if (EEPROM.read(powerOnCountAddress) != powerOnCount) {
            EEPROM.write(powerOnCountAddress, powerOnCount); // Save reset count to EEPROM
            EEPROM.commit(); // Commit changes
            Serial.println("State saved to EEPROM and relay turned ON");
        }
    }
}
