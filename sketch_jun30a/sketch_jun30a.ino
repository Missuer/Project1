
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "www"
#define STAPSK  "123456789"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "tcp.lewei50.com";
const uint16_t port = 9960;

char inputString[250];
int ii = 0;
boolean stringComplete = false;

int val = 0;

#define LED1 D4
#define LED2 D5
#define LED3 D6

int led1onoff = 0;
int led2onoff = 0;
int led3onoff = 0;

long timer1 = 0;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  pinMode(LED1, OUTPUT);   //定义led引脚为输出模式
  pinMode(LED2, OUTPUT);   //定义led引脚为输出模式
  pinMode(LED3, OUTPUT);   //定义led引脚为输出模式

  digitalWrite(LED1, led1onoff); //led引脚输出低电平，点亮led
  digitalWrite(LED2, led2onoff); //led引脚输出低电平，点亮led
  digitalWrite(LED3, led3onoff); //led引脚输出低电平，点亮led

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  Serial.println("sending data to server");
  if (client.connected()) {
    client.println("{\"method\": \"update\",\"gatewayNo\": \"01\" ,\"userkey\": \"a2217aa54b134399aeaa3bc443e599fd\"}&^!");
  }
}

void loop() {
  val++;
  if (val > 10)val = 0;


  // This will send a string to the server
  if (millis() - timer1 > 5000)
  {
    timer1 = millis();
    digitalWrite(LED1, led1onoff); //led引脚输出低电平，点亮led
    digitalWrite(LED2, led2onoff); //led引脚输出低电平，点亮led
    digitalWrite(LED3, led3onoff); //led引脚输出低电平，点亮led
    if (client.connected()) {
      Serial.println("sending data to server");
      client.print("{\"method\": \"upload\",\"data\":[{\"Name\":\"t1\",\"Value\":");  ///T1   T2  wei she bei ming
      client.print(val);
      client.println("}]}&^!");
    }

  }

  // Read all the lines of the reply from server and print them to Serial  //Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    //Serial.print(ch);
    inputString[ii] = ch;
    ii++;
    if (ch == '&')
    {
      stringComplete = true;
      ii = 0;
      for (int i = 0; i < 250; i++)
        Serial.print(inputString[i]);
      Serial.println("");
    }
    if (stringComplete)
    {
      if (strstr(inputString , "d1\",\"p2\":\"1") != NULL) {
        led1onoff = 1;
      }
      if (strstr(inputString , "d1\",\"p2\":\"0") != NULL) {
        led1onoff = 0;
      }
      if (strstr(inputString , "d2\",\"p2\":\"1") != NULL) {
        led2onoff = 1;
      }
      if (strstr(inputString , "d2\",\"p2\":\"0") != NULL) {
        led2onoff = 0;
      }
      if (strstr(inputString , "d3\",\"p2\":\"1") != NULL) {
        led3onoff = 1;
      }
      if (strstr(inputString , "d3\",\"p2\":\"0") != NULL) {
        led3onoff = 0;
      }
      if (strstr(inputString , "send") != NULL)
      {
        client.print("{\"method\":\"response\",\"result\":{\"successful\":true,\"message\":\"ok!\",\"data\":[{\"id\":\"d1\",\"value\":\"");
        client.print(led1onoff);
        client.print("\"},{\"id\":\"d2\",\"value\":\"");
        client.print(led2onoff);
        client.print("\"},{\"id\":\"d3\",\"value\":\"");
        client.print(led3onoff);        
        client.println("\"}]}}&^!");
      }
      memset(inputString, 0, 250);
      stringComplete = false;
    }

  }

}
