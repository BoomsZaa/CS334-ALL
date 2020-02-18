#include <ESP8266WiFi.h>
#include <iSYNC.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient client;
iSYNC iSYNC(client);

String ssid = "DI-PC";
String pass = "hunt.1234";

String iSYNC_USERNAME = "boomskung";
String iSYNC_KEY = "5e4bdd040e0a6a0278932701";
String iSYNC_AUTH = "5e4bdce50e0a6a02789326f9"; //auth project

///////////////////////////////////////////////
float Celsius = 0;
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex = 0;
///////////////////////////////////////////////

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  Serial.print("[iSYNC]-> ");
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if (msg.startsWith("LINE:"))msg = msg.substring(5);

  /* command control */
  if (msg.equals("เปิดไฟ")) {
    iSYNC.mqPub(iSYNC_KEY, String(random(0,100)));  //Publish
  } else if (msg.equals("ปิดไฟ")) {
    iSYNC.mqPub(iSYNC_KEY, String(random(0,100)));  //Publish
  }
}

void connectMQTT() {
  while (!iSYNC.mqConnect()) {
    Serial.println("Reconnect MQTT...");
    delay(3000);
  }
  iSYNC.mqPub(iSYNC_KEY, "พร้อมรับคำสั่งแล้ว");  //Publish on Connect
  // iSYNC.mqSubProject(); //subscribe all key in your project
  iSYNC.mqSub(iSYNC_KEY); //subscribe key
}

void setup() {
  Serial.begin(115200);
  Serial.println(iSYNC.getVersion());

  iSYNC.begin(ssid, pass);
  iSYNC.mqInit(iSYNC_USERNAME, iSYNC_AUTH);
  iSYNC.MQTT->setCallback(callback);
  connectMQTT();
  ////////////////////////////////////////////////////////
  sensors.begin();
  lcd.begin();
  lcd.backlight();
}

void loop() {
  if (!iSYNC.mqConnected())connectMQTT();
  iSYNC.mqLoop();
  /////////////////////////////////////////////////////////////////////
  sensors.requestTemperatures();
  Celsius = sensors.getTempCByIndex(0);

  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;
  if (millis() - samplingTime > samplingInterval)
  {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth)pHArrayIndex = 0;
    voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;
    pHValue = 3.5 * voltage + Offset;
    samplingTime = millis();
  }
  if (millis() - printTime > printInterval)  //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
    Serial.print(voltage, 2);
    Serial.print("    pH value: ");
    Serial.print(pHValue, 2);
    Serial.print("    Tem value: ");
    Serial.print(Celsius);
    Serial.print(" C  ");
    Serial.println();
    printTime = millis();
  }

  lcd.setCursor(0, 0);
  lcd.print(" pH:     ");
  lcd.setCursor(5, 0);
  lcd.print(pHValue);
  lcd.setCursor(0, 1);
  lcd.print("Tem:     ");
  lcd.setCursor(5, 1);
  lcd.print(Celsius);
  lcd.setCursor(11, 1);
  lcd.print("C");
  delay(2000);

  if (msg.equals("0")) {
    iSYNC.mqPub(iSYNC_KEY,pHValue);  //Publish
  } else if (msg.equals("1")) {
    iSYNC.mqPub(iSYNC_KEY,Celsius);  //Publish
  }
}

///////////////////////////////////////////////////////////////////////////////<-- คำนวณของ pH
double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;      //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}
