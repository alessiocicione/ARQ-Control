#include "BluetoothSerial.h"
#include <APIClient.h>


#define RXD2 12
#define TXD2 14 


apic::WIFIConnector wifi;
apic::APIClient apiClient;
BluetoothSerial SerialBT;

String bufferBLT;

void setup(){
  
  Serial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1,RXD2,TXD2);
  
  SerialBT.begin("AirQualityControl");
  SerialBT.setPin("1234");

  wifi.setConnection("SSID", "PASSWORD");
  wifi.connect();

  unsigned int host[] = {192, 168, 73, 242};
  //String host = "www.sitoweb.com";
  //apiClient.serServer(host, 80);
  
  apiClient.setServer("www.airqualitycontrol.altervista.org");
  //apiClient.setServer(host);
  apiClient.setAPIKey("unicas_airqualitycontrol2022");
 
}

void loop(){

  if (SerialBT.available()){
    wifi.disconnect();
    
    bufferBLT = SerialBT.readString();
    String ssid = bufferBLT.substring(0, bufferBLT.indexOf('&'));
    String key = bufferBLT.substring(bufferBLT.indexOf('&') + 1, bufferBLT.length());
    
    SerialBT.println("Provo la connessione con SSID: " + ssid + " Password: " + key);
    wifi.setConnection(ssid.c_str(), key.c_str());
    wifi.connect();

    if (wifi.connected()){
      SerialBT.println("Connessione riuscita");
    }else
      SerialBT.println("Connessione fallita");

    delay(20);
  }

  String dataStr = Serial1.readString();
  dataStr.replace("/n","");
  apiClient.sendData("/api.php", dataStr);
  Serial.print(dataStr);
  //SerialBT.print(Serial1.readString());
  delay(10);

 
  
}
