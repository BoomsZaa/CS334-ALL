#include <PubSubClient.h>
#include <WiFi.h>

// Update these with values suitable for your network.
const char* ssid = "DI-PC";
const char* password = "hunt.1234";

// Config MQTT Server
#define mqtt_server "m16.cloudmqtt.com"
#define mqtt_port 17507
#define mqtt_user "hvwzeuyp"
#define mqtt_password "gYmJZHlLxxhp"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(2, OUTPUT);

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
//  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
  } else {
    client.subscribe({topic});
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  int i=0;
  while (i<length) msg += (char)payload[i++];
  Serial.println(msg);
  digitalWrite(D0, (msg == "LEDON" ? HIGH : LOW));
}
