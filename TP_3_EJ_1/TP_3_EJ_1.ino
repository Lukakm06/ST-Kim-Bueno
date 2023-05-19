//Barrere, Kim, Bueno
//TP 3 Telegram

#include "TimerOne.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

//Máquina de Estados
int sM;
#define SCR1 0
#define SCR2 1
#define BTNP1 2
#define BTNP2 3
#define BTNP3 4
#define WAIT 5

//Botones
#define BTN_PIN 11
#define BTN_PIN2 12
bool readBtn1;
bool readBtn2;

//I2C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
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

//Tiempo
int msSwitch;
int msDisplay;
int msDHT;

void setup() {
    Serial.begin(9600);
  
    Timer1.initialize(1000);
    Timer1.attachInterrupt(ISR_Timer);
    Timer1.attachInterrupt(timerSwitch);

    pinMode(BTN_PIN,INPUT_PULLUP);
    pinMode(BTN_PIN2,INPUT_PULLUP);
  
    display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
  
    dht.begin();
}

void loop() {
    
    readBtn1 = digitalRead(BTN_PIN);
    readBtn2 = digitalRead(BTN_PIN2);

    if(msDHT == 500){
         temperature = dht.readTemperature();
        msDHT = 0;
    }
    
    ISR_Timer();
    stateMachine();
    
    if(msDisplay >= 500){
        display.clearDisplay();
        display.display();
        msDisplay = 0;
    }
}

void stateMachine(){
    switch(sM){
        case SCR1:
            display.setCursor(0,0);
            display.print("Temperatura = ");
            display.print(temperature);
            display.print(" °C");
            display.setCursor(1,0);
            display.print("Temperatura U. = 24 °C");

            Serial.println("Pantalla 1");

            if(readBtn1 == LOW && readBtn2 == HIGH){
                sM = BTNP1;
            }
        break;
        
        case BTNP1:
            timerSwitch();
            if(readBtn1 == HIGH && readBtn2 == LOW){
              sM = BTNP2;
              msSwitch = 0;
            }

            Serial.println("Press 1");
            
            if(msSwitch == 5000){
              sM = SCR1;
            }
        break; 
        
        case BTNP2:
            timerSwitch();
            if(readBtn1 == LOW && readBtn2 == HIGH){
                sM = BTNP3;
            }
            
            Serial.println("Press 2");
            
            if(msSwitch == 5000){
              sM = SCR1;
            }
            break;
        
        case BTNP3:
            if(readBtn1 == HIGH && readBtn2 == HIGH){
                sM = SCR2;
            }
            
            Serial.println("Press 3");
            
            if(msSwitch == 5000){
                sM = SCR1;
            }
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

void ISR_Timer(){
    msDisplay++;
    msDHT++;
}

void timerSwitch(){
  msSwitch++;
}
