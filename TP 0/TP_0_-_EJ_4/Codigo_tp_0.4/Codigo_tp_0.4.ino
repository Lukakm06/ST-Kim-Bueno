#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <DHT.h>

//Botones
#define BTN_PIN 34
#define BTN_PIN2 35

//Pantalla
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Máquina de Estados
int sM = 0;
#define STILL 0
#define MOD 1
#define WAIT 2
#define WAIT2 3
#define ADD 4
#define SUB 5


//DHT
#define DHTPIN 23
#define DHTTYPE DHT11
float temperature;
int umbral = 28;
DHT dht(DHTPIN, DHTTYPE);

//LED
#define RELAY_PIN 18

//Delay
int delayDisplay = 500;
int lastDisplay;

void setup() {

  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);


  dht.begin();

}

void loop() {
    temperature = dht.readTemperature();
    
  switch (sM) {
    
    case STILL:
    Serial.println("STILL");
    if (millis() >= lastDisplay + delayDisplay || lastDisplay == 0) {
        lastDisplay = millis();

        Serial.println("Impreso");
        
        display.clearDisplay();
        display.setCursor(0, 16);
        display.println("Temp: ");
        display.println(temperature);
        display.println("Temp U: ");
        display.println(umbral);
        display.display();
    }
     if(digitalRead(BTN_PIN) == LOW && digitalRead(BTN_PIN2) == LOW){
        sM = WAIT;
      }
    break;


    case MOD:
      if (millis() <= lastDisplay + delayDisplay) {
        lastDisplay = millis();

        Serial.println("MOD");
        
        display.clearDisplay();
        display.setCursor(0, 16);
        display.println("Temp U: ");
        display.println(umbral);
        display.display();
      }

      if(digitalRead(BTN_PIN) == LOW && digitalRead(BTN_PIN2) == HIGH){
        sM = ADD;
      }

      if(digitalRead(BTN_PIN) == HIGH && digitalRead(BTN_PIN2) == LOW){
        sM = SUB;
      }
      
      if(digitalRead(BTN_PIN) == LOW && digitalRead(BTN_PIN2) == LOW){
        sM = WAIT2;
      }
    break;

      
    case WAIT:
    Serial.println("WAIT");
      if(digitalRead(BTN_PIN) == HIGH && digitalRead(BTN_PIN2) == HIGH){
        sM = MOD;
      }
    break;

    case WAIT2:
    Serial.println("WAIT2");
      if(digitalRead(BTN_PIN) == HIGH && digitalRead(BTN_PIN2) == HIGH){
        sM = STILL;
      }
    break;


    case ADD:
      if(digitalRead(BTN_PIN) == HIGH && digitalRead(BTN_PIN2) == HIGH){
        umbral++;
        sM = MOD;
      }
    break;


    case SUB:
      if(digitalRead(BTN_PIN) == HIGH && digitalRead(BTN_PIN2) == HIGH){
        umbral--;
        sM = MOD;
      }
    break;
  }
  
}
