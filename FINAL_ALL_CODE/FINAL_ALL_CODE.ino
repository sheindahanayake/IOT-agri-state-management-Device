#define BLYNK_TEMPLATE_ID "TMPL6qPpGxfmk"   
#define BLYNK_TEMPLATE_NAME "state management system" 
#define BLYNK_AUTH_TOKEN "IpcvknpRJ-3mGhaCyiowF_ZezKBA3fdc"  
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

// Replace with your network credentials
const char* ssid = "Huawei Y7 Pro"; 
const char* password = "zxcvbnm0"; 

// Blynk authorization token
char auth[] = "IpcvknpRJ-3mGhaCyiowF_ZezKBA3fdc"; 

// Firebase configuration
FirebaseConfig config;
FirebaseAuth authFirebase;
FirebaseData firebaseData;  

#define DHTPIN 4 // GPIO for DHT sensor
#define DHTTYPE DHT11 // DHT sensor type
#define SOIL_MOISTURE_PIN 34 // Soil moisture sensor connected to D34

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize Blynk
  Blynk.begin(auth, ssid, password);

  // Set Firebase host and API key
  config.host = "https://state-management-system-e10f6-default-rtdb.asia-southeast1.firebasedatabase.app/";
  config.api_key = "AIzaSyBeUAeDi78eRNsD1gCjZMeiBRGwA9YSgec";

  // Initialize Firebase
  Firebase.begin(&config, &authFirebase);
  Firebase.reconnectWiFi(true);

  dht.begin();
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
}

void loop() {
  // Read humidity and temperature from DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check DHT sensor readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return; 
  }

  // Read soil moisture sensor value
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisture = map(soilMoistureValue, 0, 4095, 0, 100); // Adjust mapping as needed for ESP32

  // Print readings to Serial
  Serial.printf("Humidity: %.2f%%  Temperature: %.2f°C  Soil Moisture: %.2f%%\n", humidity, temperature, soilMoisture);

  // Update the LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("% T:");
  lcd.print(temperature);
  lcd.print("C");

  // Check fertilization needs
  String fertilizerRecommendations = "No Fertilizer";
  
  if (soilMoisture < 80) {
    fertilizerRecommendations = "Add: N,K";
  } else if (temperature < 30) { // Corrected this line
    fertilizerRecommendations = "Add: P,K";
  }

  // Display fertilization recommendations
  lcd.setCursor(0, 1);
  lcd.print(fertilizerRecommendations);  // Display recommendations on the second line
  lcd.setCursor(10, 1);  // Set cursor to display soil moisture after recommendations
  lcd.print("M:");
  lcd.print(soilMoisture);
  lcd.print("%");

  // Send data to Firebase
  if (Firebase.setFloat(firebaseData, "/humidity", humidity)) {
    Serial.println("Humidity sent to Firebase");
  } else {
    Serial.println("Failed to send humidity to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/temperature", temperature)) {
    Serial.println("Temperature sent to Firebase");
  } else {
    Serial.println("Failed to send temperature to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/soilMoisture", soilMoisture)) {
    Serial.println("Soil moisture sent to Firebase");
  } else {
    Serial.println("Failed to send soil moisture to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  // Send fertilization status to Firebase
  if (Firebase.setString(firebaseData, "/fertilizationRecommendations", fertilizerRecommendations)) {
    Serial.println("Fertilization recommendations sent to Firebase");
  } else {
    Serial.println("Failed to send fertilization recommendations to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  // Send data to Blynk
  Blynk.virtualWrite(V0, humidity); // Send humidity to virtual pin V0
  Blynk.virtualWrite(V1, temperature); // Send temperature to virtual pin V1
  Blynk.virtualWrite(V2, soilMoisture); // Send soil moisture to virtual pin V2 
  Blynk.virtualWrite(V3, fertilizerRecommendations); // Send fertilization recommendations to V3

  // Process Blynk
  Blynk.run();

  // Delay before the next reading
  delay(2000);
}
