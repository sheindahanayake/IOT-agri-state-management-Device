# IoT State Management System Device

## Overview
The **IoT State Management System** is designed to monitor soil and environmental conditions in real-time. Using **Arduino ESP32**, it collects data from an **NPK sensor** (Nitrogen, Phosphorus, Potassium levels) and a **DHT11 sensor** (Temperature and Humidity). The system provides real-time monitoring via **Blynk** and stores the data in **Firebase** for notifications and remote access.

The device displays the collected data locally on a display screen and streams the real-time data to a mobile app for monitoring and notifications.

## Features
- **Real-Time Data Collection**: Collects real-time data from:
  - **NPK sensor** for soil Nitrogen, Phosphorus, and Potassium levels.
  - **DHT11 sensor** for temperature and humidity.
- **Mobile Monitoring**: Real-time monitoring via the **Blynk** app on Android and iOS.
- **Local Display**: Displays real-time sensor data on the connected display (such as an OLED).
- **Notifications**: Uses **Firebase** for storing data and sending notifications when soil conditions deviate from optimal levels.
- **Wireless Communication**: Data is transmitted via **Wi-Fi** using **ESP32**.

## Technologies Used
- **Hardware**:
  - **Arduino ESP32**: Microcontroller for handling the sensors and sending data.
  - **NPK Sensor**: Measures soil nutrient levels (Nitrogen, Phosphorus, Potassium).
  - **DHT11 Sensor**: Measures temperature and humidity.
  - **OLED Display**: For displaying real-time data on the device.
  
- **Software**:
  - **Blynk**: Provides mobile monitoring and real-time updates of sensor data.
  - **Firebase**: For storing sensor data and sending notifications to mobile devices.

- **Programming Languages**:
  - **C++**: Used for programming the Arduino ESP32.
  - **Blynk API**: For mobile communication with the Arduino.

## Hardware Setup
1. **Arduino ESP32**: Connect the ESP32 board to your computer via USB.
2. **NPK Sensor**: Connect the NPK sensor to the ESP32's analog input pins.
3. **DHT11 Sensor**: Connect the DHT11 to one of the digital pins on the ESP32.
4. **OLED Display**: Connect the OLED display to the ESP32 using I2C or SPI communication.

### Pin Connections
- **NPK Sensor**: Connect the output pin of the NPK sensor to one of the **analog pins** (e.g., **A0**).
- **DHT11 Sensor**: Connect the **VCC**, **GND**, and **DATA** pins of the DHT11 to the ESP32.
- **OLED Display**: Connect the **SCL**, **SDA**, **VCC**, and **GND** pins of the OLED to the ESP32.

## Software Setup

1. **Arduino IDE Setup**:
   - Install the **ESP32 board** in the Arduino IDE by adding the ESP32 board manager URL in the preferences.
   - Select your ESP32 board model from the **Tools > Board** menu.
   - Install the necessary libraries:
     - **Blynk**: For mobile app integration.
     - **DHT**: For DHT11 sensor functionality.
     - **Adafruit_SSD1306**: For OLED display functionality.

2. **Firebase Setup**:
   - Create a Firebase project at [Firebase Console](https://console.firebase.google.com/).
   - Generate the **Firebase configuration credentials** for your project.
   - Use these credentials in the Arduino code to store data and send notifications.

3. **Blynk Setup**:
   - Create a **Blynk project** via the Blynk app (available for Android and iOS).
   - Get your **Blynk Auth Token** and integrate it into the code for mobile app communication.
   - Add widgets like **Graph**, **Gauge**, or **Value Display** on the Blynk app to monitor the data.

4. **Upload Code**:
   - Open the code in Arduino IDE and upload it to the ESP32 board.
   - Ensure that the ESP32 is connected to Wi-Fi and the Blynk app is running on your mobile.

## Example Code
```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

// Define the pins
#define NPK_SENSOR_PIN A0
#define DHT_PIN 4

// DHT sensor setup
DHT dht(DHT_PIN, DHT11);

// Blynk Auth Token (from your Blynk project)
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";

// Wi-Fi credentials
char ssid[] = "your_wifi_ssid";
char pass[] = "your_wifi_password";

// Initialize OLED display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Initialize DHT11 sensor
  dht.begin();

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Initialize the OLED display
  if (!display.begin(SSD1306_I2C_ADDRESS, 4)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.display();
  delay(2000);
}

void loop() {
  // Read NPK sensor (simplified for this example)
  int npkValue = analogRead(NPK_SENSOR_PIN);

  // Read DHT11 sensor (Temperature and Humidity)
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Display data on the OLED screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("NPK: ");
  display.println(npkValue);
  display.print("Temp: ");
  display.print(temperature);
  display.println("C");
  display.print("Humidity: ");
  display.print(humidity);
  display.println("%");
  display.display();

  // Send data to Blynk
  Blynk.virtualWrite(V1, npkValue); // Virtual Pin V1 for NPK data
  Blynk.virtualWrite(V2, temperature); // Virtual Pin V2 for Temperature
  Blynk.virtualWrite(V3, humidity); // Virtual Pin V3 for Humidity

  // Send data to Firebase (optional, requires Firebase library)

  // Delay for a while before the next reading
  delay(5000);
}
