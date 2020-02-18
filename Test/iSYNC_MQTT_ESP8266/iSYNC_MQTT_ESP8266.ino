#include <ESP8266WiFi.h>
#include <iSYNC.h>

WiFiClient client;
iSYNC iSYNC(client);

String ssid = "DI-PC";
String pass = "hunt.1234";

String iSYNC_USERNAME = "boomskung";
String iSYNC_KEY = "5e4bdd040e0a6a0278932701";
String iSYNC_AUTH = "5e4bdce50e0a6a02789326f9"; //auth project

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("[iSYNC]-> ");
	for (int i = 0; i < length; i++)Serial.print((char)payload[i]);
	Serial.println();
}

void connectMQTT(){
	while(!iSYNC.mqConnect()){
		Serial.println("Reconnect MQTT...");
		delay(3000);
	}
	iSYNC.mqSubProject(); //subscribe all key in your project
//  iSYNC.mqSub(iSYNC_KEY); //subscribe key
}

void setup() {
	Serial.begin(115200);
	Serial.println(iSYNC.getVersion());

	iSYNC.begin(ssid,pass);
	iSYNC.mqInit(iSYNC_USERNAME,iSYNC_AUTH);
	iSYNC.MQTT->setCallback(callback);
	connectMQTT();
}

long last = 0;
void loop() {
	if (!iSYNC.mqConnected())connectMQTT();
	iSYNC.mqLoop();

	if (millis() - last > 5000) {
			last = millis();
			String payload="{\"Temperature\":"+String(random(0,100))+",\"Humidity\":"+String(random(0,100))+"}";
			Serial.println("[iSYNC]<- "+payload);
			iSYNC.mqPub(iSYNC_KEY,payload);   //Publish
	}
}
