#include <ESP8266WiFi.h>

WiFiClient client;

WiFiServer server(80);

const char* ssid = "latha";      //enter your wi-fi user id

const char* password = "1234567890";   //enter your wi-fi password


void setup()

{

  Serial.begin(115200);    //select the baud rate in serial monitor

  connectWiFi();

  server.begin();

}


void loop()

{


}


void connectWiFi()

{

  Serial.println("Connecting to WIFI");

  WiFi.begin(ssid, password);

  while ((!(WiFi.status() == WL_CONNECTED)))

  {

    delay(300);

    Serial.print("..");

  }

  Serial.println("");

  Serial.println("WiFi connected");

  Serial.println("NodeMCU Local IP is : ");     //Ip address that you need to enter in the code

  Serial.print((WiFi.localIP()));

}
