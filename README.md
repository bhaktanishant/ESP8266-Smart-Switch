# ESP8266 Smart Switch

This project uses an ESP8266 microcontroller to create a smart switch that can control a relay over Wi-Fi. The switch can be toggled via a web interface, allowing users to turn the connected device on or off from anywhere within Wi-Fi range.

## Features

- **Wi-Fi Connectivity**: Connects to your local Wi-Fi network and provides a web interface.
- **Relay Control**: Control the state of a relay, which can be used to switch various devices on or off.
- **Persistent State**: Uses EEPROM to save the relay state and power-on count, ensuring the state is retained after power loss.
- **User-Friendly Web Interface**: A responsive and clean HTML interface with instant feedback on relay status.
- **Automatic Reconnection**: Automatically attempts to reconnect to Wi-Fi if the connection is lost.

## Requirements

- ESP8266 board (e.g., NodeMCU, Wemos D1 Mini)
- Arduino IDE with ESP8266 board support installed
- Relay module (compatible with your application)
- Basic knowledge of Arduino programming and electronics

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/ESP8266-Smart-Switch.git
   cd ESP8266-Smart-Switch

2.  **Open the project in Arduino IDE**.
    
3.  **Install required libraries**:
    
    *   Ensure you have the ESP8266WiFi and EEPROM libraries included in your Arduino IDE.
        
4.  **Configure your Wi-Fi credentials**:
    
    *   Modify the ssid and password variables in the code to match your Wi-Fi network.

5.  **Change Output Pin**:

    *   The default relay control pin is set to **D4** to control onboard LED for test purpose. To change it, modify the relayPin variable in the code to your desired GPIO pin number.
        
6.  **Upload the code**:
    
    *   Select the correct board and port from the Tools menu and upload the code to your ESP8266.
        
7.  **Access the Web Interface**:
    
    *   Open a web browser and navigate to the IP address assigned to your ESP8266 (displayed in the Serial Monitor).
        

Usage
-----

*   Use the toggle switch in the web interface to control the relay.
    
*   The current state of the relay is displayed on the page, updating immediately upon toggling.
    

EEPROM Usage
------------

*   The relay state and power-on count are stored in EEPROM.
    
*   The power-on count resets after the relay state changes or reaches a specific threshold.
    

License
-------

This project is licensed under the MIT License - see the LICENSE file for details.
    

Contributing
------------
Feel free to submit issues, fork the repository, or submit pull requests. Contributions are welcome!