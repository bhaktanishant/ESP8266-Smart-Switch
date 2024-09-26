#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid = "Nothing Phone (2a)_6332";
const char* password = "nahi pata";

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
void reconnectWiFi(); // Function to reconnect to Wi-Fi

void setup() {
    Serial.begin(115200);
    EEPROM.begin(1);

    pinMode(relayPin, OUTPUT);

    // Restore relay state from EEPROM
    relayState = EEPROM.read(0);
    if (relayState != 0 && relayState != 1) {
        relayState = 1; // Default to ON if EEPROM has invalid data
    }
    digitalWrite(relayPin, relayState ? HIGH : LOW); // HIGH turns ON the relay

    // Restore powerOnCount from EEPROM
    powerOnCount = EEPROM.read(powerOnCountAddress);

    // Increment powerOnCount each time the ESP is powered on
    powerOnCount++;
    EEPROM.write(powerOnCountAddress, powerOnCount); // Save updated count to EEPROM
    EEPROM.commit(); // Commit changes

    // Check if the power-on limit has been reached
    checkPowerOnCount(); 

    // Connect to Wi-Fi
    reconnectWiFi(); // Call the reconnect function

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        reconnectWiFi(); // Call reconnect function if disconnected
    }
    
    handleClient(); // Handle client requests
}

void reconnectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi. Retrying...");
    }
}

void handleClient() {
    WiFiClient client = server.available(); // Changed to available()
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
            html += "<style>";
            html += "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }";
            html += ".switch { position: relative; display: inline-block; width: 60px; height: 34px; }";
            html += ".switch input { opacity: 0; width: 0; height: 0; }";
            html += ".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }";
            html += ".slider:before { position: absolute; content: ''; height: 26px; width: 26px; border-radius: 50%; left: 4px; bottom: 4px; background-color: white; transition: .4s; }";
            html += "input:checked + .slider { background-color: #4CAF50; }";
            html += "input:checked + .slider:before { transform: translateX(26px); }";
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
            html += "<h2>Smart Switch</h2>";
            html += "<label class='switch'>";
            html += "  <input type='checkbox' id='relayToggle' onclick='toggleRelay()' " + String(relayState ? "checked" : "") + ">";
            html += "  <span class='slider'></span>";
            html += "</label>";
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
        digitalWrite(relayPin, HIGH); // Set to HIGH to turn ON the relay
        relayState = true;
        saveState();
    }
}

void handleOff() {
    Serial.println("Handling /off");
    if (relayState) { // Only change state if it's currently on
        digitalWrite(relayPin, LOW); // Set to LOW to turn OFF the relay
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
        digitalWrite(relayPin, HIGH); // Turn the relay ON
        
        // Only write if there's a change
        if (EEPROM.read(powerOnCountAddress) != powerOnCount) {
            EEPROM.write(powerOnCountAddress, powerOnCount); // Save reset count to EEPROM
            EEPROM.commit(); // Commit changes
            Serial.println("State saved to EEPROM and relay turned ON");
        }
    }
}