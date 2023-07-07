// Ariel B. , Didier B. , Luka K.

#include <Preferences.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"

Preferences preferences;

//I2C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//WiFi
//#define WIFI_SSID "ORT-IoT"
//#define WIFI_PASSWORD "OrtIOTnew22$2"
#define WIFI_SSID "Fibertel WiFi247 2.4GHz"
#define WIFI_PASSWORD "0042411200"

//Firebase
#define API_KEY "AIzaSyAHtcsQ9nZNft14mEnK8IYCrcQ0wmbjz40"
#define EMAIL "lukakm06@gmail.com"
#define PASS "TuberculoBueno"
#define URL "https://fir-st-tp-5-default-rtdb.firebaseio.com/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String databasePath;
String parentPath;
int timestamp;
FirebaseJson json;
long timerDelay = 30000;
const char* ntpServer = "pool.ntp.org";

//Comands
String tempPath = "/temp";
String timePath = "/timestamp";

//TEMPERATURA
#include "DHT.h"
#define DHTPIN 23
#define DHTTYPE DHT11
#define TEMP 27
DHT dht(DHTPIN, DHTTYPE);
float temperature;
float tUmbral = 26;
float oldTemp;
bool flagTemp;

//Botones
#define BOTON1 34
#define BOTON2 35
int readBtn1;
int readBtn2;

//Máquina de Estados
int sM;
#define PANTALLA1 0 
#define PANTALLA2 1
#define WAIT1 2
#define WAIT2 3
#define SUMA 4
#define RESTA 5

void setup() {
    preferences.begin("my-app", false);

    //PINMODES
    pinMode(BOTON1, INPUT_PULLUP);
    pinMode(BOTON2, INPUT_PULLUP);
    pinMode(TEMP, INPUT);

    //Serial
    Serial.begin(115200);
  
    //DHT
    dht.begin();

    //Display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    delay(2000);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);

    //WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
    Serial.println();

    //Firebase
    config.api_key = API_KEY;
    auth.user.email = EMAIL;
    auth.user.password = PASS;
    config.database_url = URL;
    
    Firebase.reconnectWiFi(true);
    
    fbdo.setResponseSize(4096);
    
    config.token_status_callback = tokenStatusCallback;
    config.max_token_generation_retry = 5;
    
    Firebase.begin(&config, &auth);
    Serial.println("Getting User UID");
    
    while ((auth.token.uid) == "") {
        Serial.print('.');
        delay(1000);
    }
    databasePath = "/UsersData/" + uid + "/readings";
}

void loop() {
    temperature = dht.readTemperature();

    //Botones
    readBtn1 = digitalRead(BOTON1);
    readBtn2 = digitalRead(BOTON2);
  
    switch(sM){
        case PANTALLA1:
            display.clearDisplay();
            display.setCursor(0,0);
            display.print("T. A. =");
            display.print(temperature);
            display.print("C");
            display.setCursor(0,0);
            display.print("T. U. =");
            display.print(tUmbral);
            display.print("C");

            if(readBtn1 == LOW && readBtn2 == LOW){
                sM = WAIT1;
            }
        break;
        
        case PANTALLA2:
            display.clearDisplay();
            display.setCursor(30,0);
            display.print("Ciclo");
            display.setCursor(15,15);
            display.print(timerDelay/1000);
            display.print(" Segundos");
            display.display();
            
            if(readBtn1 == LOW && readBtn2 == LOW){
                 sM = WAIT2;
            }

            if(readBtn1 == LOW && readBtn2 == HIGH){
                 sM = SUMA;
            }

            if(readBtn1 == HIGH && readBtn2 == LOW){
                 sM = RESTA;
            }
            
        break;
        case WAIT1:
            if(readBtn1 == HIGH && readBtn2 == HIGH){
                 sM = PANTALLA2;
            }
        break;
        case WAIT2:
            if(readBtn1 == HIGH && readBtn2 == HIGH){
                 sM = PANTALLA1;
            }
        break;
        case SUMA:
            if(readBtn1 == HIGH && readBtn2 == HIGH){
                 timerDelay += 3000;
                 sM = PANTALLA2;
            }
        break;
        case RESTA:
            if(readBtn1 == HIGH && readBtn2 == HIGH){
                 timerDelay -= 3000;
                 sM = PANTALLA2;
            }
        break;
    }

}
