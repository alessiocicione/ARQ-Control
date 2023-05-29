#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <APIClient.h>

/*

    Struttura memoria:
    00000000000000000000000000000000000000000000000000000000000000000000000000000000000
    -Posizione 0: Stato dell'impostazione deep sleep, puo valere 0 oppure 1 (off/on)
    -Posizione 1: Check rapido della connessione wifi, controlla se è presente una wifi in memoria, puo valere 0 oppure 1
    -Posizione 2 in poi: WIFI ssid e Password separati dal carattere '&' centrale, '\0' é il carattere di terminazione dopo l'ultimo carattere della key

*/
#define EEPROM_SIZE 83 // (Saved flag) + (max wpa_length) + '&' + (max_key_lenght) + '\0'

#define BLT_BTN 18

#define uS_S 1000000 // Fattore di conversione da us a s
#define SLEEP_S 300 // 5min

apic::WIFIConnector wifi;
apic::APIClient apiClient;
BluetoothSerial SerialBT;



String defaultSSID = "SSID";
String defaultKey = "KEY";

String bufferBLT;
const char* HOST = "www.airqualitycontrol.altervista.org";
const char* API_KEY = "unicas_airqualitycontrol2022";

void saveConnection(String ssid, String key) {

  clearSavedConnection();
  EEPROM.write(1, 1);

  unsigned int keyAddress = 0;
  for (unsigned int i = 0; i < ssid.length(); i++) {
    EEPROM.write(2 + i, ssid[i]);
    keyAddress = 2 + i;
  }

  keyAddress++;
  EEPROM.write(keyAddress, '&');
  keyAddress++;

  unsigned int i = 0;
  for (i = 0; i < key.length(); i++)
    EEPROM.write(keyAddress + i, key[i]);

  EEPROM.write(keyAddress + i, '\0');

  EEPROM.commit();


}

String readConnection() {

  String memory = "";
  char c;
  for (unsigned int i = 2; (c = EEPROM.read(i)) != '\0' && i < EEPROM_SIZE; i++)
    memory += c;

  return memory;

}

bool checkSavedConnection() {

  if (EEPROM.read(1) == 1)
    return true;

  return false;

}

void clearSavedConnection() {

  for (unsigned int i = 2; i < EEPROM_SIZE; i++)
    EEPROM.write(i, 0);

  EEPROM.commit();

}

void setDeepSleep(bool yes) {

  if (yes) {
    EEPROM.write(0, 1);
    EEPROM.commit();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
    esp_sleep_enable_timer_wakeup(SLEEP_S * uS_S);
    esp_deep_sleep_start();
  } else {
    EEPROM.write(0, 0);
    EEPROM.commit();
  }



}

bool doBLTOperations() {

  if (SerialBT.available()) {
    wifi.disconnect();

    bufferBLT = SerialBT.readString();
    if (bufferBLT.startsWith("deepsleep=")) {
      String deepStr = bufferBLT.substring(10, bufferBLT.length());
      if (deepStr == "yes") {
        Serial.println("Entering deep sleep mode");
        SerialBT.println("Entering deep sleep mode");
        delay(100);
        setDeepSleep(true);
      } else {
        Serial.println("Exiting from deep sleep mode");
        SerialBT.println("Exiting from deep sleep mode");
        delay(100);
        setDeepSleep(false);
        if (checkSavedConnection()) {
          String mem = readConnection();
          defaultSSID = mem.substring(0, mem.indexOf('&'));
          defaultKey = mem.substring(mem.indexOf('&') + 1, mem.length());
          Serial.println("Loading WIFI from memory with SSID:" + defaultSSID + " Key:" + defaultKey);
        }
      }
      return true;
    }
    String ssid = bufferBLT.substring(0, bufferBLT.indexOf('&'));
    String key = bufferBLT.substring(bufferBLT.indexOf('&') + 1, bufferBLT.length());

    defaultSSID = ssid;
    defaultKey = key;

    Serial.println("Trying connection with SSID: " + ssid + " Key: " + key);
    SerialBT.println("Trying connectoin with SSID: " + ssid + " Key: " + key);
    wifi.setConnection(ssid.c_str(), key.c_str());
    wifi.connect();

    if (wifi.connected())
      SerialBT.println("Connected");
    else
      SerialBT.println("Connection failed");

    delay(20);

    Serial.println("Im saving WIFI informations in memory...");
    saveConnection(defaultSSID, defaultKey);
    Serial.println("Connection saved");

    return true;
  }

  return false;

}

bool checkDeepSleep() {

  if (EEPROM.read(0) == 1)
    return true;

  return false;

}

void sendData() {
  if (wifi.connected()) {
    String data = "";
    //data = Serial1.readString()....
    apiClient.sendData("/api.php", data);
  }
}


void setup() {

  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  SerialBT.begin("AirQualityControl");

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Wake up, waiting for bluetooth connection. Press the secondary button to exit");
    while (!doBLTOperations()) {
      if (digitalRead(BLT_BTN) == HIGH) {
        Serial.println("Exiting from bluetooth listening");
        break;
      }
    }

    if (checkDeepSleep()) {
      Serial.println("Going to sleep");
      setDeepSleep(true);
    }
  }

  apiClient.setServer(HOST);
  apiClient.setAPIKey(API_KEY);

  if (checkSavedConnection()) {
    String mem = readConnection();
    defaultSSID = mem.substring(0, mem.indexOf('&'));
    defaultKey = mem.substring(mem.indexOf('&') + 1, mem.length());
    Serial.println("Loading WIFI from memory with SSID:" + defaultSSID + " Key:" + defaultKey);
  }

  wifi.setConnection(defaultSSID.c_str(), defaultKey.c_str());
  wifi.connect();
  if (!checkSavedConnection()) {
    saveConnection(defaultSSID, defaultKey);
  }

  sendData();


  if (checkDeepSleep()) {
    Serial.println("Im going to sleep");
    setDeepSleep(true);
  }

}



void loop() {

  doBLTOperations();
  delay(50);

  sendData();

  if (checkDeepSleep()) {
    Serial.println("Im going to sleep");
    setDeepSleep(true);
  }

  delay(50);




}
