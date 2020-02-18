#include <ESP8266WiFi.h>
#include <iSYNC.h>

WiFiClient client;
iSYNC iSYNC(client);

String ssid = "DI-PC";
String pass = "hunt.1234";

String iSYNC_USERNAME = "boomskung";
String iSYNC_KEY = "5e4bdd040e0a6a0278932701";
String iSYNC_AUTH = "5e4bdce50e0a6a02789326f9"; //auth project

const int ledPin =  D0;

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  Serial.print("[iSYNC]-> ");
  for (int i = 0; i < length; i++){
    msg+=(char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(msg.startsWith("LINE:"))msg = msg.substring(5);

  /* command control */
  if(msg.equals("เปิดไฟ")){
      digitalWrite(ledPin, LOW);
      iSYNC.mqPub(iSYNC_KEY,"ได้เลยจ๊ะพี่จ๋า");   //Publish
  }else if(msg.equals("ปิดไฟ")){
      digitalWrite(ledPin, HIGH);
      iSYNC.mqPub(iSYNC_KEY,"ได้เลยจ๊ะพี่จ๋า");   //Publish
  }
}

void connectMQTT(){
  while(!iSYNC.mqConnect()){
    Serial.println("Reconnect MQTT...");
    delay(3000);
  }
  iSYNC.mqPub(iSYNC_KEY,"พร้อมรับคำสั่งแล้วจ๊ะพี่จ๋า");   //Publish on Connect
// iSYNC.mqSubProject(); //subscribe all key in your project
  iSYNC.mqSub(iSYNC_KEY); //subscribe key
}

void setup() {
  Serial.begin(115200);
  Serial.println(iSYNC.getVersion());

//  pinMode(D0,OUTPUT);  // led on/off set output
  pinMode(ledPin, OUTPUT);

  iSYNC.begin(ssid,pass);
  iSYNC.mqInit(iSYNC_USERNAME,iSYNC_AUTH);
  iSYNC.MQTT->setCallback(callback);
  connectMQTT();
}

void loop() {
  if (!iSYNC.mqConnected())connectMQTT();
  iSYNC.mqLoop();
}
