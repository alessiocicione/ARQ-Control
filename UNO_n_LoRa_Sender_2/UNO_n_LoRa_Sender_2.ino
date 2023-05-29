#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTTYPE DHT11

#define PIN_MQ5 A0
#define PIN_PH A2
#define PIN_CO2 A1
#define ALPHA 0.5
#define PIN_TX 0

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

#define SCROLL_UP 4
#define SCROLL_DOWN 3

uint8_t DHTPin = 5;
uint8_t AQC_1[8] = {  
  0x00,
  0x00,
  0x03,
  0x04,
  0x09,
  0x10,
  0x11,
  0x15
  };

uint8_t AQC_2[8] = {  
  0x00,
  0x00,
  0x18,
  0x04,
  0x12,
  0x01,
  0x11,
  0x15
  };

uint8_t AQC_3[8] = {  
  0x15,
  0x15,
  0x15,
  0x0D,
  0x05,
  0x01,
  0x01,
  0x00
  };

uint8_t AQC_4[8] = {  
  0x15,
  0x15,
  0x15,
  0x16,
  0x14,
  0x10,
  0x10,
  0x00
  };

uint8_t Deg[] = {
  0x00,
  0x07,
  0x05,
  0x07,
  0x00,
  0x00,
  0x00,
  0x00
};
int i = 0;

DHT dht(DHTPin, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,20,4);
bool LCDStatus = true;

void LCDProgressiveLeftScroll(int len=20, int del=50);
void LCDProgressiveText(char *txt,int len,int del=50);

RH_RF95 rf95;

int Temperature = 0;
int Humidity = 0;
int MQ5_Reading = 0;
int PH_Reading = 0;
int PH_Prev_Reading = 0;
int PH_Output = 0;
int CO2_Reading = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial) ;
  if (!rf95.init())
    Serial.println("init failed");
  pinMode(DHTPin, INPUT);
  dht.begin();

  pinMode(PIN_MQ5,INPUT);

  pinMode(PIN_PH,INPUT);

  pinMode(PIN_CO2,INPUT);

  pinMode(PIN_TX,OUTPUT);

    pinMode(SCROLL_UP,INPUT_PULLUP);
  pinMode(SCROLL_DOWN,INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, AQC_1);
  lcd.createChar(1, AQC_2);
  lcd.createChar(2, AQC_3);
  lcd.createChar(3, AQC_4);
  lcd.createChar(4, Deg);
  lcd.home();
  

  char line1[12] = " Air Quality";
  char line2[8] = " Control";
  lcd.printByte(0);
  lcd.printByte(1);
  lcd.setCursor(0, 1);
  lcd.printByte(2);
  lcd.printByte(3);
  lcd.setCursor(2,0);
  LCDProgressiveText(line1,12);
  lcd.setCursor(4,1);
  LCDProgressiveText(line2,8);
  delay(2500);
  LCDProgressiveLeftScroll();
  lcd.clear();
}

bool FFLcd(bool stat)
{
  if(stat==true) lcd.noBacklight();
  else lcd.backlight();
  return !stat;  
}

void LCDProgressiveText(char *txt,int len,int del)
{
  for(int i=0;i<len;i++)
  {
    lcd.print(txt[i]);
    delay(del);
  }
}

void LCDProgressiveLeftScroll(int len, int del)
{
  for(int i=0;i<len;i++)
  {
    lcd.scrollDisplayLeft();
    delay(del);
  }
}

void HandleLcd(int Temperature, int Humidity, int CO2_Level, int Brightness, int Air_Quality)
{
    if(digitalRead(SCROLL_UP)==LOW and digitalRead(SCROLL_DOWN)==LOW)
  {
    LCDStatus = FFLcd(LCDStatus);
    //Serial.print("LCDStatus: ");
    //Serial.println(LCDStatus);
  }else if(digitalRead(SCROLL_UP)==LOW) i++;
  else if(digitalRead(SCROLL_DOWN)==LOW) i--;
  //Serial.print("pre: ");
  //Serial.println(i);
  
  if(i>5) i=0;
  else if(i<0) i=5;

  //Serial.print("post: ");
  //Serial.println(i);

  lcd.clear();
  lcd.setCursor(0,0);

  switch(i%5)
  {
    case 0:
      lcd.print("Temperature:");
      lcd.setCursor(0,1);
      lcd.print(String(Temperature)+" ");
      lcd.printByte(4);
      lcd.print("C");
      break;
      
     case 1:
      lcd.print("Humidity:");
      lcd.setCursor(0,1);
      lcd.print(String(Humidity)+" %");
      break;

     case 2:
      lcd.print("Brightness:");
      lcd.setCursor(0,1);
      lcd.print(String(Brightness)+" lm");
      break;

     case 3:
      lcd.print("CO2:");
      lcd.setCursor(0,1);
      lcd.print(String(CO2_Level)+" ppm");
      break;
      
     case 4:
      lcd.print("Air Quality:");
      lcd.setCursor(0,1);
      lcd.print(String(Air_Quality));
      break;   
  }
}

void loop()
{
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();

  MQ5_Reading = analogRead(PIN_MQ5);
  PH_Reading = analogRead(PIN_PH);
  PH_Output = PH_Reading*ALPHA + PH_Prev_Reading*(1-ALPHA);
  PH_Prev_Reading = PH_Reading;
  CO2_Reading = analogRead(PIN_CO2);

  HandleLcd(Temperature,Humidity,CO2_Reading,PH_Output,MQ5_Reading);

  String packet_str;

  packet_str = "temperature="+(String)Temperature+"&"+"humidity="+(String)Humidity+"&"+"co2="+String(CO2_Reading)+"&"+"brightness="+String(PH_Reading)+"&"+"air_quality="+String(MQ5_Reading);
                                                                                                                               //or PH_Output -^
  
  int len = packet_str.length() + 1;
  char packet_ch[len];
  packet_str.toCharArray(packet_ch,len);
  Serial.println(packet_ch);

  rf95.send(packet_ch, sizeof(packet_ch));
  rf95.waitPacketSent();
  //digitalWrite(PIN_TX,HIGH);//no PIN_TX
  delay(10);
  //digitalWrite(PIN_TX,LOW);

  delay(1490);
}
