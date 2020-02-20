#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SSID        "DI-PC"
#define PASSWORD    "hunt.1234"
#define LINE_TOKEN  "WfIT2aCsAF48E4aTTbYQG5Msu6Ne9rXcgp8TMdUDgfk"

///////////////////////////////////////////////////////////////////
float Celsius = 0;
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00           //deviation compensate
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex = 0;
///////////////////////////////////////////////////////////////////
void setup() {
  sensors.begin();
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);

  ////LCD/////
  lcd.begin();
  lcd.backlight();
}

void loop() {
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
    Serial.print(Celsius, 2);
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
  //  delay(2000);

  if (pHValue >= 10) {
    LINE.notify("\nค่า pH เกินที่กำหนดไว้ : " + String(pHValue) + "\nอุณหภูมิ : " + String(Celsius));
    //    LINE.notify("อุณหภูมิ : "+String(Celsius));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////<---คำนวน pH
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
