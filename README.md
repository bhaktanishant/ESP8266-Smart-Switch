# Smart Switch using ESP8266

This project allows you to control a relay as a smart switch using an ESP8266 microcontroller configured as a Wi-Fi hotspot. You can connect any device to the hotspot and use a web interface to toggle the relay on and off.

## Features

- **Wi-Fi Hotspot**: The ESP8266 creates an open hotspot named `ESP8266_Hotspot` for easy connectivity.
- **Smart Relay Control**: Turn the relay on and off via simple HTTP requests.
- **State Persistence**: The relay state and power-on count are stored in EEPROM, ensuring they persist across reboots.
- **User-Friendly Interface**: A simple web interface allows for easy control from any connected device.

## Requirements

- **Hardware**:
  - ESP8266 module (e.g., NodeMCU, Wemos D1 Mini)
  - Relay module

- **Software**:
  - Arduino IDE with ESP8266 board package installed

## Installation

1. **Clone the repository** or download the code:
   ```bash
   git clone https://github.com/bhaktanishant/ESP8266-Smart-Switch.git

2.  **Open the code in Arduino IDE or VS Code**.
    
3.  **Select the correct board** under Tools > Board.
    
4.  **Upload the code** to your ESP8266.
    

Usage
-----

1.  Power on your ESP8266. It will start the hotspot.
    
2.  Connect your device to the Wi-Fi network named ESP8266\_Hotspot (no password required).
    
3.  Open a web browser and navigate to 192.168.4.1.
    
4.  Use the toggle switch on the web interface to control the smart switch.
    

Code Overview
-------------

*   The code sets up the ESP8266 as a Wi-Fi hotspot.
    
*   It uses EEPROM to store the relay state and a power-on count.
    
*   The web interface is generated dynamically, allowing users to send commands to the ESP8266.
    

Notes
-----

*   Ensure the relay is properly connected to the specified GPIO pin (D4 in this case).
    
*   The ESP8266 should be powered adequately to handle the relay.
    

License
-------

This project is open source and available under the MIT License.
