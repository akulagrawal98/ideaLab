/* Pin Layout
 *  D0, D3, D5, D6, D7, D8, TX, 10, 3, 9 
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Relay Output Pin
const int relay1=D0;
const int relay2=D3;

// WIFI VARIABLES
const char* ssid = "F-101";
const char* pass = "lastsemester";
const char* mqttServer = "192.168.0.103";
const int mqttPort = 1883;

//const char* ssid = "B1004";
//const char* pass = "thisissparta";
//const char* mqttServer = "192.168.1.225";
//const int mqttPort = 1883;

// MQTT Topics
String pubTopic = "esp/receive";
String subTopic = "esp/command";

int fanSpeed[] = {0, 52, 104, 156, 208, 255};
int currentSpeed = 0;

// Touch Button for relays
int button1 = D5;
int button2 = D6;
int val1 = 0;
int val2 = 0;
int state1 = 0;
int state2 = 0;
int led1 = 3;
int led2 = 9;

// Touch Button for Fan
int butLow = D7;
int butHigh = D8;
int valLow = 0;
int valHigh = 0;

int buzzer = 10;

// Variables For Fan
int currentState=0;
int requiredState=0;
int diff=0;


WiFiClient espClient;
PubSubClient client(espClient);

void setup() 
{
  Serial.begin(9600);
  
  // SETTING ALL PINMODES
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(button1,INPUT);
  pinMode(button2,INPUT);
  pinMode(butLow,INPUT);
  pinMode(butHigh,INPUT);

  

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
      delay(2000);
    }
  }
  client.publish((char *) pubTopic.c_str(),"Hello from ESP");
  client.subscribe((char *) subTopic.c_str());
}


void callback(char* topic, byte* payload, unsigned int length) 
{
  String mess="";
  for (int i = 0; i < length; i++)
    mess+=(char)payload[i];
 
  if(mess[0] == 'f')
  {
    char fan = mess[1];
    if(fan=='1')
    {
      requiredState=52;
      currentState = requiredState;
    }
    else if(fan=='2')
    {
      requiredState=104;
      currentState = requiredState;
    }
    else if(fan=='3')
    {
      requiredState=156;
      currentState = requiredState;
    }
    else if(fan=='4')
    {
      requiredState=208;
      currentState = requiredState;
    }
    else if(fan=='5')
    {
      requiredState=255;
      currentState = requiredState;
    }
    else if(fan=='0')
    {
      requiredState=0;
      currentState = requiredState;
    }
//    diff=currentState-requiredState;
//    while(currentState!=requiredState)
//    {
//      if(diff>0)
//      {
////        Serial.write(currentState);
//        currentState-=1;
//      }
//      else if(diff<0)
//      {
////        Serial.write(currentState);
//        currentState+=1;
//      }
//      delay(50);
//    }
    //Serial.write(currentState);
//    Serial.println(currentState);
  }
  else if(mess[0]=='1')  // FOR RELAY 1
  {
    if(state1 == 1)
    {
      digitalWrite(relay1,LOW);
      digitalWrite(led1,LOW);
      state1 = 0; 
    }
    else
    {
      digitalWrite(relay1,HIGH);
      digitalWrite(led1,HIGH);
      state1 = 1; 
    }
  }
  else if(mess[0]=='2') // FOR RELAY 2
  {
    if(state2 == 1)
    {
      digitalWrite(relay2,LOW);
      digitalWrite(led2,LOW);
      state2 = 0;
    }
    else
    {
        digitalWrite(led2,HIGH);
      digitalWrite(relay2,HIGH);
      //digitalWrite(led2,HIGH);
      state2 = 1;
    }
  }
  else if(mess[0]=='t') // FOR TIMER STATE
  {
    if(mess[1] == 's') // TIMER IN SET STATE
      doubleBeep();
      
    else if(mess[1] == 'o') // TIMER IN OFF STATE
      alarmBeep();
  }
}

void doubleBeep()
{
  digitalWrite(buzzer,HIGH);
  delay(50);
  digitalWrite(buzzer,LOW);
  delay(50);
  digitalWrite(buzzer,HIGH);
  delay(50);
  digitalWrite(buzzer,LOW);
}

void singleBeep()
{
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
}

void alarmBeep()
{
  digitalWrite(buzzer,HIGH);
  delay(150);
  digitalWrite(buzzer,LOW);
  delay(150);
  digitalWrite(buzzer,HIGH);
  delay(150);
  digitalWrite(buzzer,LOW);
}

void loop() 
{
  client.loop();

  // Read Touch Buttons
  val1 = digitalRead(button1);
  val2 = digitalRead(button2);
  valLow=digitalRead(butLow);
  valHigh=digitalRead(butHigh);

  // GET APPROPRIATE TOUCH ACTIONS
  if(val1==HIGH)
  {
      if(state1 == 1)
      {
        digitalWrite(relay1,LOW);
        digitalWrite(led1,LOW);
        //delay(500);
        client.publish((char *) pubTopic.c_str(),"11");
        delay(500);

      }
      else
      {
        digitalWrite(relay1,HIGH);
        digitalWrite(led1,HIGH);
        client.publish((char *) pubTopic.c_str(),"10");
        delay(500);
      }
  }
  else if(val2==HIGH)
  {
      if(state2 == 1)
      {
        digitalWrite(relay2,LOW);
        digitalWrite(led2,LOW);
        client.publish((char *) pubTopic.c_str(),"21");
        delay(500);
      }
      else
      {
        digitalWrite(relay2,HIGH);
        digitalWrite(led2,HIGH);
        client.publish((char *) pubTopic.c_str(),"20");
        delay(500);
      }
  }
  else if(valLow==HIGH || valHigh==HIGH)
  {
    if(valHigh == HIGH)
    {
      Serial.print("+\n");
      if(currentSpeed == 5)
        doubleBeep();
      else
      {
        currentSpeed+=1;
        String fanMsg = 'f' + String(currentSpeed,DEC);
        client.publish((char *) pubTopic.c_str(),(char *) fanMsg.c_str());
        singleBeep();
      }
    }
    else if(valLow == HIGH)
    {
      Serial.print("-\n");
      if(currentSpeed == 0)
        doubleBeep();
      else
      {
        currentSpeed-=1;
        String fanMsg = 'f' + String(currentSpeed,DEC);
        client.publish((char *) pubTopic.c_str(),(char *) fanMsg.c_str());   
        singleBeep();
      }
    }
  }
  delay(500);
}
