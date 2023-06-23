// Didier B. , Luka K. y Ariel B.
//TP 4: EEPROM

#include "U8g2lib.h"
#include "DHT.h"
#include "Adafruit_Sensor.h"
#include <Preferences.h>

Preferences preferences;

//U8g2
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//DHT
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Máquina de Estados
int sM = 0;
#define PANTALLA1 0
#define PANTALLA2 1
#define CAMBIO1 2
#define CAMBIO2 3
#define SUMAR_UMBRAL 4
#define RESTAR_UMBRAL 5

//int tumbral = 10;
unsigned int tumbral;

//Botones
#define BTN_PIN1 35
#define BTN_PIN2 34


void setup() {

  //U8g2
  u8g2.begin();

  //DHT
  dht.begin();

  //Botones
  pinMode(BTN_PIN1, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);

  //Preferences
  preferences.begin("my-app", false);
  tumbral = preferences.getUInt("UMBRAL", 0);
}

void loop() {
  float t = dht.readTemperature();
  //Serial.println(t);
  char temp[5];
  sprintf(temp, "%2.1f", t);
  // Serial.println(temp);
  char umbral[2];
  sprintf(umbral, "%i", tumbral);
  
  switch(sM){
        case PANTALLA1:
            u8g2.setFont(u8g2_font_cupcakemetoyourleader_tr );
            u8g2.drawStr(5, 15, "Temperatura: ");
            u8g2.drawStr(5, 30, temp);
            u8g2.drawStr(60, 30, "C");
            u8g2.drawStr(5, 45, "T. Umbral:");
            u8g2.drawStr(5, 60, umbral);
            u8g2.sendBuffer();
            
            if (digitalRead(BTN_PIN1) == LOW && digitalRead(BTN_PIN2) == LOW) {
                sM = CAMBIO1;
            }
        break;
        
        
        case PANTALLA2:
            u8g2.setFont(u8g2_font_cupcakemetoyourleader_tr );
            u8g2.drawStr(0, 30, "T. Umbral: ");
            u8g2.drawStr(0, 50, umbral);
            u8g2.sendBuffer();
            
            if (digitalRead(BTN_PIN1) == LOW && digitalRead(BTN_PIN2) == LOW) {
                sM = CAMBIO2;
            }
            
            if(digitalRead(BTN_PIN1) == LOW && digitalRead(BTN_PIN2) == HIGH){
                sM = SUMAR_UMBRAL;
            }
            
            if(digitalRead(BTN_PIN1) == HIGH && digitalRead(BTN_PIN2) == LOW){
                sM = RESTAR_UMBRAL;
            }
            
        break;
        
        
        case CAMBIO1:
            u8g2.clearBuffer();
            u8g2.sendBuffer();
            if (digitalRead(BTN_PIN1) == HIGH && digitalRead(BTN_PIN2) == HIGH) {
                sM = PANTALLA2;
            }
        break;
        
        
        case CAMBIO2:
            u8g2.clearBuffer();
            u8g2.sendBuffer();
            if (digitalRead(BTN_PIN1) == HIGH && digitalRead(BTN_PIN2) == HIGH) {
                preferences.putUInt("UMBRAL", tumbral);
                preferences.end();
                sM = PANTALLA1;
            }
        break;

        case SUMAR_UMBRAL:
            if(digitalRead(BTN_PIN1) == HIGH && digitalRead(BTN_PIN2) == HIGH){
                tumbral++;
                u8g2.clearBuffer();
                u8g2.sendBuffer();
                sM = PANTALLA2;
            }
        break;

        case RESTAR_UMBRAL:
             if(digitalRead(BTN_PIN1) == HIGH && digitalRead(BTN_PIN2) == HIGH){
                tumbral--;
                u8g2.clearBuffer();
                u8g2.sendBuffer();
                sM = PANTALLA2;
            }
        break;

    }
}

void stateMachine(){
}
