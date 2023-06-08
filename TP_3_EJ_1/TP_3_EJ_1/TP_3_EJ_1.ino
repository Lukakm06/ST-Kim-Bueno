//Barrere, Kim, Bueno
//TP 3 Telegram

//Librerias
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Wire.h>

//WiFi
//const char* ssid = "ORT-IoT";
// const char* password = "OrtIOTnew22$2";
const char* ssid = "Fibertel WiFi247 2.4GHz";
const char* password = "0042411200";
WiFiClientSecure client;

//Bot Telegram
#define BOTtoken "6141959127:AAGI9q7fKhbCldpHnKjGeh3AVjKf95gI9rQ"
#define CHAT_ID "5869871087"
UniversalTelegramBot bot(BOTtoken, client);

//Máquina de Estados
int sM;
#define SCR1 0
#define SCR2 1
#define BTNP1 2
#define BTNP2 3
#define BTNP3 4
#define WAIT 5

//Botones
#define BTN_PIN 34
#define BTN_PIN2 35
bool readBtn1;
bool readBtn2;

//I2C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//DHT
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperature;
float umbral = 28;
String botTemp;

//Tiempo
unsigned long lastDHT = 0;
unsigned long lastPressed = 0;
unsigned long lastTimeBotRan;

int delayDHT = 500;
int delayPressed = 5000;
int botRequestDelay = 1000;

void setup() {
    
    //Serial
    Serial.begin(9600);

    //Botones
    pinMode(BTN_PIN,INPUT_PULLUP);
    pinMode(BTN_PIN2,INPUT_PULLUP);

    //Display
    display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);

    //DHT
    dht.begin();

    
   // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 

  ///bloquea el programa si no se puede conectar a internet 
  while (WiFi.status() != WL_CONNECTED) {   
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
    Serial.println(WiFi.localIP());
    bot.sendMessage(CHAT_ID, "Bot Hola mundo", "");
}




void loop() {
    temperature = dht.readTemperature();
    readBtn1 = digitalRead(BTN_PIN);
    readBtn2 = digitalRead(BTN_PIN2);

    stateMachine();

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Bot en línea");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
}



void stateMachine(){
    switch(sM){
        case SCR1:
        
            if(millis() >= lastDHT + delayDHT){
                lastDHT = millis();
                temperature = dht.readTemperature();
            }
            
            display.setCursor(0,0);
            display.print("T = ");
            display.print(temperature);
            display.print(" °C");
            display.setCursor(1,0);
            display.print("T.U = ");
            display.print(umbral);
            display.print("°C");
            display.display();
            
            Serial.println("Pantalla 1");
            
            if(readBtn1 == LOW && readBtn2 == HIGH){
                sM = BTNP1;
            }
            
        break;
        
        case BTNP1:
        
            if(readBtn1 == HIGH && readBtn2 == LOW){
              sM = BTNP2;
              lastPressed = millis();
            }
            
            Serial.println("Press 1");
            
            if(millis() >= lastPressed + delayPressed){
              lastPressed = millis();
              sM = SCR1;
            }
            
        break; 
        
        case BTNP2:
        
            if(readBtn1 == LOW && readBtn2 == HIGH){
                lastPressed = millis();
                sM = BTNP3;
            }
            
            Serial.println("Press 2");
            
            if(millis() >= lastPressed + delayPressed){
              lastPressed = millis();
              sM = SCR1;
            }
            
            break;
        
        case BTNP3:
        
            if(readBtn1 == HIGH && readBtn2 == HIGH){
                sM = SCR2;
            }
            
            Serial.println("Press 3");
            
        break;
        
        case SCR2:
        
            if(readBtn1 == LOW && readBtn2 == LOW){
                sM = WAIT;
            }
            
            Serial.println("Pantalla 2");
            
        break;
        
        case WAIT:
        
            if(readBtn1 == HIGH && readBtn2 == HIGH){
              sM = SCR1;
              Serial.println("gueiting");
            }
            
        break;
    }
}



void handleNewMessages(int numNewMessages) {
  Serial.println("Mensaje nuevo");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // inicio de verificacion
    String chat_id = String(bot.messages[i].chat_id);
    
    ///fin de verificacion

    // imprime el msj recibido 
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/temp") {  
      
      temperature = dht.readTemperature();
      botTemp = temperature;
      Serial.println("Hacen " + botTemp);
      bot.sendMessage(chat_id, "Temperature is: " + botTemp, " °C");

     
    }
      
      if(temperature >= umbral){

    bot.sendMessage(chat_id, "Hace calor");

  }
  }
}
