/*
Author : Akul
Connections
1. ESP (TX) to AC Dimmer Module (RX)
2. ESP (D0) to Relay (IN1) //FINAL PIN
3. ESP (D1) to Relay (IN2) //FINAL PIN

4. ESP (D2) to Touch1 (Bulb1)
5. ESP (D3) to Touch2 (Bulb2)
6. ESP (D4) to Touch3 (Fan Low)
7. ESP (D5) to Touch4 (Fan High)
8. ESP (D6)
9. ESP (D7)
10. ESP (D8)
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid="B1004";
const char* pass="thisissparta";
const char* mqttServer="192.168.1.225";
const int mqttPort=1883;

const int relay1=D3;
const int relay2=D5;

int button1 = D6;
int button2 = D7;
int val1 = 0;
int val2 = 0;


int currentState=0;
int requiredState=0;
int diff=0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() 
{
  Serial.begin(9600);
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
//    Serial.print(".");
  }
//  Serial.println("Wifi Connected");
  
  client.setServer(mqttServer,mqttPort);
  client.setCallback(callback);
  
  while(!client.connected())
  {
//    Serial.println("Connecting to the MQTT server");
    if(client.connect("client_name"))
    {
//      Serial.println("Connected to MQTT broker");
    }
    else
    {
//      Serial.print("failed with state ");
//      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish("esp/receive","Hello from ESP");
  client.subscribe("esp/command");
}


void callback(char* topic, byte* payload, unsigned int length) {
 
//  Serial.print("Message arrived in topic: ");
//  Serial.println(topic);
  String mess="";
//  Serial.print("Message:");
  for (int i = 0; i < length; i++) 
  {
    mess+=(char)payload[i];
  }
//  Serial.println(mess);
  if(mess[0]=='1')
  {
    if(digitalRead(relay1)==1)
      digitalWrite(relay1,LOW);
    else
      digitalWrite(relay1,HIGH);
  }
  else if(mess[0]=='2')
  {
    if(digitalRead(relay2)==1)
      digitalWrite(relay2,LOW);
    else
      digitalWrite(relay2,HIGH);
  }
  else if(mess[0]=='f')
  {
    if(mess[1]=='1')
    {
      requiredState=52;
    }
    else if(mess[1]=='2')
    {
      requiredState=104;
    }
    else if(mess[1]=='3')
    {
      requiredState=156;
    }
    else if(mess[1]=='4')
    {
      requiredState=208;
    }
    else if(mess[1]=='5')
    {
      requiredState=255;
    }
    else if(mess[1]=='0')
    {
      requiredState=0;
    }
    diff=currentState-requiredState;
    while(currentState!=requiredState)
    {
      if(diff>0)
      {
        Serial.write(currentState);
        currentState-=1;
      }
      else if(diff<0)
      {
        Serial.write(currentState);
        currentState+=1;
      }
      delay(50);
    }
//    Serial.println("CurrentState Now is");
//    Serial.print(currentState);
  }
 
}
int temp=0;
void loop() 
{
 client.loop();
}
