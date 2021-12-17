#include <WiFi.h>
#include <Adafruit_AHTX0.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define RED 13
#define GREEN 14
#define BLUE 12



const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "142.93.174.193";

const char* topic2 = "zurich/glassboxKriAri/temperature/out";
const char* topic3 = "zurich/glassboxKriAri/temperature/data";
const char* topic4 = "zurich/glassboxKriAri/humidity/out";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  setup_wifi();
  setup_display();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void setup_display(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
 Serial.println("SSD1306 allocation failed");
 while(1) delay(1);
 }
 display.clearDisplay();
 display.setTextSize(2);
 display.setTextColor(WHITE);
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

  Serial.print("IP address: ");
  
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, topic2) == 0) {
    char message[length + 1];
    message[length] = '\0';
    for(int i = 0; i < length; i++) {
      message[i] = (char)payload[i];
    }
    Serial.print("Received Message: ");
    Serial.println(message);
    
    if(strcmp(message, "HOT!") == 0) {
      analogWrite(RED, 255);
      analogWrite(GREEN, 0);
      analogWrite(BLUE, 0);
      } else if(strcmp(message, "GOOD!") == 0) {
      analogWrite(RED, 0);
      analogWrite(GREEN, 255);
      analogWrite(BLUE, 0);    
      } else if(strcmp(message, "COLD!") == 0) {
      analogWrite(RED, 0);
      analogWrite(GREEN, 0);
      analogWrite(BLUE, 255);    
      }
} 

if(strcmp(topic, topic3) == 0) {
    char message[length + 1];
    message[length] = '\0';
    for(int i = 0; i < length; i++) {
      message[i] = (char)payload[i];
    } 
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(message); display.println(" C");
    display.println("----------");
    display.display();
    delay(250);
    }}

void reconnect() {
  Serial.print("Attempting MQTT connection...");
  while(!client.connected()) {
    if(client.connect("Esp32.Glassbox.receiver.KriAri")) {
      Serial.println("done!");

      client.subscribe(topic2);
      client.subscribe(topic4);
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
  
  delay(1000);
  client.loop();
}
