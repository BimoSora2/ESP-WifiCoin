#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Thermal_Printer.h>

// WiFi settings
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// Mikrotik API settings
const char* mikrotik_ip = "192.168.88.1";
const int mikrotik_port = 8728;
const char* mikrotik_user = "admin";
const char* mikrotik_pass = "password";

// Pin definitions
const int COIN_PIN = D2;
const int BUTTON_PIN = D3;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Thermal printer setup
Thermal_Printer printer(D5, D6);  // RX, TX pins

// Variables
int coinValue = 0;
String voucher = "";

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.print("WiFi Coin System");
  
  // Initialize printer
  printer.begin();
  
  // Set up pins
  pinMode(COIN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  lcd.clear();
  lcd.print("Ready");
}

void loop() {
  // Check for coin insertion
  if (digitalRead(COIN_PIN) == LOW) {
    coinValue += 1; // Assume 1 coin = 1 unit of value
    lcd.clear();
    lcd.print("Coin inserted");
    lcd.setCursor(0, 1);
    lcd.print("Value: " + String(coinValue));
    delay(500);
  }
  
  // Check for button press to generate voucher
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (coinValue > 0) {
      generateVoucher();
      printVoucher();
      coinValue = 0;
    } else {
      lcd.clear();
      lcd.print("Insert coin first");
      delay(2000);
    }
  }
}

void generateVoucher() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    
    String url = "http://" + String(mikrotik_ip) + "/rest/ip/hotspot/user/add";
    
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(mikrotik_user, mikrotik_pass);
    
    String profileName = "1hour"; // Adjust based on coin value
    String username = "user" + String(random(10000, 99999));
    String password = String(random(1000, 9999));
    
    String payload = "{\"name\":\"" + username + "\",\"password\":\"" + password + "\",\"profile\":\"" + profileName + "\"}";
    
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      voucher = username + ":" + password;
      lcd.clear();
      lcd.print("Voucher created");
    } else {
      lcd.clear();
      lcd.print("Voucher error");
    }
    
    http.end();
  }
}

void printVoucher() {
  printer.justify('C');
  printer.boldOn();
  printer.println("WiFi Voucher");
  printer.boldOff();
  printer.println("----------------");
  printer.println("Username: " + voucher.substring(0, voucher.indexOf(':')));
  printer.println("Password: " + voucher.substring(voucher.indexOf(':') + 1));
  printer.println("----------------");
  printer.println("Valid for 1 hour");
  printer.feed(3);
  
  lcd.clear();
  lcd.print("Voucher printed");
  delay(2000);
  lcd.clear();
  lcd.print("Ready");
}
