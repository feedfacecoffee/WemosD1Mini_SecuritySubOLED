
//OLED
#define I2C_ADDRESS 0x3C
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
//ESP8266 and MQTT
#include<ESP8266WiFi.h>
#include<PubSubClient.h>

#define _WLAN_SSID "MySSID"
#define _WLAN_PASS "MyPassword"

#define _MQTT_BROKER "10.0.1.250"
#define _MQTT_PORT 1883
#define _MQTT_USER "MyMQTTUser"
#define _MQTT_PASS "MyMQTTPassword"

SSD1306AsciiWire oled;

const char* ssid = _WLAN_SSID;
const char* password = _WLAN_PASS;

WiFiClient wlanClient;
PubSubClient mqttClient(wlanClient);
long lastMsg = 0;
char msg[50];
int buttonState[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
int previousButtonState[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
int securityPublisherState = -1;//0 == disconnected, 1 == connected
int previousSecurityPublisherState = -1;
int firstRun = 1;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  oled.clear();
  oled.println("WiFi start");
  oled.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  oled.clear();
  oled.println("WiFi conn");
  oled.println(ssid);
}

//char MQTT_BROKER[] PROGMEM = _MQTT_BROKER;
//char MQTT_CLIENTID[] PROGMEM = __TIME__ _MQTT_USER;
//char MQTT_USER[] PROGMEM = _MQTT_USER;
//char MQTT_PASS[] PROGMEM = _MQTT_PASS;

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    oled.clear();
    oled.println("MQTT start");
    if (mqttClient.connect("WeMosOLEDScreen")) {
      Serial.println("Connected");
      oled.clear();
      oled.println("MQTT conn");
      mqttClient.subscribe("/security/#", 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      oled.clear();
      oled.println("MQTT fail");
      oled.print("RC: ");
      oled.println(mqttClient.state());
      delay(5000);//TODO: Modify to remove delay.
    }
    
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //if (topic == "switch1") {
  if (strcmp(topic, "/security/zone1") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[0] = 1;
    } else {
      buttonState[0] = 0;
    }
  //} else if (topic == "switch2") {
  } else if (strcmp(topic, "/security/zone2") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[1] = 1;
    } else {
      buttonState[1] = 0;
    }
  } else if (strcmp(topic, "/security/zone3") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[2] = 1;
    } else {
      buttonState[2] = 0;
    }
  } else if (strcmp(topic, "/security/zone4") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[3] = 1;
    } else {
      buttonState[3] = 0;
    }
  } else if (strcmp(topic, "/security/zone5") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[4] = 1;
    } else {
      buttonState[4] = 0;
    }
  } else if (strcmp(topic, "/security/zone6") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[5] = 1;
    } else {
      buttonState[5] = 0;
    }
  } else if (strcmp(topic, "/security/zone7") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[6] = 1;
    } else {
      buttonState[6] = 0;
    }
  } else if (strcmp(topic, "/security/zone8") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[7] = 1;
    } else {
      buttonState[7] = 0;
    }
  } else if (strcmp(topic, "/security/zone9") == 0) {
    if ((char)payload[0] == '1') {
      buttonState[8] = 1;
    } else {
      buttonState[8] = 0;
    }
  }
  
  if (strcmp(topic, "/security/will") == 0) {
    Serial.println("Got LWT");
    if ((char)payload[0] == 'C' && (char)payload[1] == 'o' && (char)payload[2] == 'n') {
      Serial.println("Publisher connected.");
      securityPublisherState = 1;
    } else {
      Serial.println("Pusblisher disconnected.");
      securityPublisherState = 0;
    }
  }

}

void setup() {
  Serial.begin(9600);

  //OLED setup
  Wire.begin();                
  oled.begin(&MicroOLED64x48, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();

  //WiFi & MQTT setup
  setup_wifi();
  mqttClient.setServer(_MQTT_BROKER, _MQTT_PORT);
  mqttClient.setCallback(callback);
}

void loop() {

  int rc = -1;

  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  delay(2);

  for (int x=0; x<=8; x++) {
    if(buttonState[x] != previousButtonState[x] || securityPublisherState == !previousSecurityPublisherState || firstRun) {
      oled.clear();
      oled.setScroll(true);
      if (securityPublisherState == 0) {
        oled.println("Pub Discon.");
      } else {
        if (buttonState[0] == 1) {
          oled.println("Front Door");
        }
        if (buttonState[1] == 1) {
          oled.println("Garage Dr.");
        }
        if (buttonState[2] == 1) {
          oled.println("Deck Door");
        }
        if (buttonState[3] == 1) {
          oled.println("Bsmt Door");
        }
        if (buttonState[4] == 1) {
          oled.println("Master");
        }
        if (buttonState[5] == 1) {
          oled.println("Fam + Kit");
        }
        if (buttonState[6] == 1) {
          oled.println("Dining");
        }
        if (buttonState[7] == 1) {
          oled.println("Basement");
        }
        if (buttonState[8] == 1) {
          oled.println("Bsmt Stair");
        }
        previousButtonState[x] = buttonState[x];
      }
    }

    if (firstRun == 1) {
      firstRun = 0;
    }

    previousSecurityPublisherState = securityPublisherState;
  }
}
