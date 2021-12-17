#include <WiFi.h>
#include <Adafruit_AHTX0.h>
#include <PubSubClient.h>
char mac[17];
const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "142.93.174.193";
const char* topic1 = "zurich/glassboxKriAri/temperature/in";
const char* topic3 = "zurich/glassboxKriAri/humidity/in";
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_AHTX0 aht;
void setup() {
 Serial.begin(115200);
 setup_aht();
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
}
void setup_aht() {
 Serial.print("Searching AHT10 / AHT20...");
 while(!aht.begin()) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("done!");
}
void setup_wifi() {
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid);
 while(WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("done!");
 byte m[6];
 WiFi.macAddress(m);
 sprintf(mac, "%02X:02X:02X:02X:02X:02X", m[0],m[1],m[2],m[3],m[4],m[5]);
}
void callback(char* topic, byte* payload, unsigned int length) {
 if(strcmp(topic, topic1) == 0) {
  
 char message[length];
 for(int i = 0; i < length; i++) {
 message[i] = (char)payload[i];
 }
 Serial.print("Received Message: ");
 Serial.println(message);
 }
}
void reconnect() {
 Serial.print("Attempting MQTT connection...");
 while(!client.connected()) {
 if(client.connect("Esp32.Glassbox.sender.KriAri")) {
 Serial.println("done!");
 client.subscribe(topic1);
 client.subscribe(topic3);
 } else {
 delay(500);
 Serial.print(".");
 }
 }
}

void loop() {
 if(!client.connected()) {
 reconnect();
 }
 sensors_event_t humidity, temp;
 aht.getEvent(&humidity, &temp);
 char tempBuffer[10];
 char humBuffer[10];
 sprintf(tempBuffer, "%.3f", temp.temperature);
 sprintf(humBuffer, "%.3f", humidity.relative_humidity);
 client.publish(topic1, tempBuffer);
 client.publish(topic3, humBuffer);
 delay(1000);
 client.loop();
}
