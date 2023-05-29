#include <SPI.h>
#include <RH_RF95.h>
#include <LiquidCrystal_I2C.h>

#define LED_PIN_RX 5
#define LED_PIN_NC 4

RH_RF95 rf95;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup()
{
  Serial.begin(9600);
  while (!Serial) ;
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
//    driver.setTxPower(14, true);
  pinMode(LED_PIN_RX,OUTPUT);
  pinMode(LED_PIN_NC,OUTPUT);

  lcd.init();
  lcd.backlight();
}
void loop()
{
  lcd.setCursor(0,0);
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(3000))
  {
    if (rf95.recv(buf, &len))
    {
      Serial.print((char*)buf);
      digitalWrite(LED_PIN_NC,LOW);
      digitalWrite(LED_PIN_RX,HIGH);
      delay(10);
      digitalWrite(LED_PIN_RX,LOW);

      lcd.print((char *)buf);
    }
    else
    {
      digitalWrite(LED_PIN_NC,HIGH);
      delay(10);
      digitalWrite(LED_PIN_NC,LOW);
      Serial.println("recv failed");

      lcd.clear();
      lcd.print("recv failed");
    }
  }
  else
  {
    digitalWrite(LED_PIN_NC,HIGH);
    Serial.println("No reply, is rf95_server running?");

    lcd.clear();
    lcd.print("no reply");
  }
  delay(390);
}
