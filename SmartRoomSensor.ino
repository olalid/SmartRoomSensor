#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <SPI.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#define MQTT_VERSION MQTT_VERSION_3_1
#include <PubSubClient.h>

//#define DEBUG_CONFIG 1

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int potVal = 0;
int adcVal = 0;
const int spiCSPin = 15;
long targetTemp = 0;

SPISettings settingsSPI(1000000, MSBFIRST, SPI_MODE0); 

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "1883";
char mqtt_user[64] = "";
char mqtt_pass[64] = "";
char mqtt_tset_topic[64] = "sensor/heatpump/target_temp_set";
char mqtt_tget_topic[64] = "sensor/heatpump/target_temp_get";
char mqtt_cget_topic[64] = "sensor/heatpump/current_temp_get";
char mqtt_mode_topic[64] = "sensor/heatpump/mode";

//flag for saving data
bool shouldSaveConfig = false;
bool didReceiveMQTT = false;

// Callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Support function to convert string to integer
int natoi(char *s, int n)
{
    int x = 0;
    while(isdigit(s[0]) && n--)
    {
        x = x * 10 + (s[0] - '0');      
        s++;
    }
    return x;
}

// Subscription callback for MQTT library
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  uint16 setVal;

  // Print what we received
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Convert the value to integer
  targetTemp = natoi((char *)payload, length);
  // Check that we are within our limits
  if(targetTemp < 5)
    targetTemp = 5;
  if(targetTemp > 30)
    targetTemp = 30;

  // Convert to potentiometer value
  setVal = 0x1300 | (((89*(30-targetTemp)+145)/10) & 0xff);
  
  snprintf(msg, 75, "Set: %ld pot: %ld", targetTemp, setVal);
  Serial.println(msg);
  SPI.beginTransaction(settingsSPI);
  SPI.write16(setVal);
  SPI.endTransaction();

  // Publish back to acknowledge that we have received and set the value
  snprintf(msg, 75, "%ld", targetTemp);
  client.publish(mqtt_tget_topic, msg, true);

  // Set the flag to tell our main code that we are done
  didReceiveMQTT = true;
  
}

void setup() {

  int i;
  long bootTime;
  boolean r;
  
  bootTime = millis();
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  //clean FS, for testing
#ifdef DEBUG_CONFIG
  SPIFFS.format();
#endif

  //Try to read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        //json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_user, json["mqtt_user"]);
          strcpy(mqtt_pass, json["mqtt_pass"]);
          strcpy(mqtt_tset_topic, json["mqtt_tset_topic"]);
          strcpy(mqtt_tget_topic, json["mqtt_tget_topic"]);
          strcpy(mqtt_cget_topic, json["mqtt_cget_topic"]);
          strcpy(mqtt_mode_topic, json["mqtt_mode_topic"]);
          
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt username", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt password", mqtt_pass, 40);
  WiFiManagerParameter custom_mqtt_tset_topic("tset_topic", "mqtt tset topic", mqtt_tset_topic, 62);
  WiFiManagerParameter custom_mqtt_tget_topic("tget_topic", "mqtt tget topic", mqtt_tget_topic, 62);
  WiFiManagerParameter custom_mqtt_cget_topic("cget_topic", "mqtt cget topic", mqtt_cget_topic, 62);
  WiFiManagerParameter custom_mqtt_mode_topic("mode_topic", "mqtt mode topic", mqtt_mode_topic, 62);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_tset_topic);
  wifiManager.addParameter(&custom_mqtt_tget_topic);
  wifiManager.addParameter(&custom_mqtt_cget_topic);
  wifiManager.addParameter(&custom_mqtt_mode_topic);

  //reset settings - for testing
#ifdef DEBUG_CONFIG
  wifiManager.resetSettings();
#endif

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "SmartRoomSensor"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("SmartRoomSensor")) {
    Serial.println("failed to connect and hit timeout. Sleeping for 10 seconds.");
    ESP.deepSleep(10*1e6);
  }

  //if you get here you have connected to the WiFi
  Serial.println("WiFi connected");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(mqtt_tset_topic, custom_mqtt_tset_topic.getValue());
  strcpy(mqtt_tget_topic, custom_mqtt_tget_topic.getValue());
  strcpy(mqtt_cget_topic, custom_mqtt_cget_topic.getValue());
  strcpy(mqtt_mode_topic, custom_mqtt_mode_topic.getValue());

  //save the custom parameters to FS if needed
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["mqtt_tset_topic"] = mqtt_tset_topic;
    json["mqtt_tget_topic"] = mqtt_tget_topic;
    json["mqtt_cget_topic"] = mqtt_cget_topic;
    json["mqtt_mode_topic"] = mqtt_mode_topic;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  // We are connected, print the local IP
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  // MQTT client init
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(mqttCallback);

  // Init the SPI interface
  SPI.pins(6, 7, 8, 0);
  SPI.begin();

  // Connect to the MQTT server
  // If it fails we will go to sleep again and start over 
 
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if(strlen(mqtt_user)) {
    // If username is given, connect with username password
    r = client.connect("smartroomsensor", mqtt_user, mqtt_pass);
  } else {  
    // No username given
    r = client.connect("smartroomsensor");
  }
  if (r) {
    Serial.println("connected");
    // Publish a mode topic to make Home Assistant happy...
    client.publish(mqtt_mode_topic, "heat", true);
    // ... and subscribe
    Serial.print("Subscribe to: ");
    Serial.println(mqtt_tset_topic);
    client.subscribe(mqtt_tset_topic);
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 10 seconds. Going to sleep.");
    ESP.deepSleep(10*1e6);

  }
  // Library process
  client.loop();

  // Read current temperature frpm ADC
  adcVal = analogRead(A0);
  snprintf(msg, 75, "Analog input: %ld", adcVal);
  Serial.println(msg);

  // Publish current temperature to the MQTT server
  adcVal = (607.84091-(0.719697*((double)adcVal)))+1;
  snprintf (msg, 75, "%ld.", adcVal/10);
  if(adcVal % 10 >= 5)
    strcat(msg, "5");
  else
    strcat(msg, "0");
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(mqtt_cget_topic, msg, true);

  long start, now;
  start = millis();
  for(;;) {
    client.loop();
    now = millis();
    if(didReceiveMQTT)
      break;
    if(now - start > 5000)
      break;
  }
  client.loop();
  client.disconnect();
  client.loop();
  while(client.connected())
    client.loop();

  // Give the MQTT server some time to really disconnect...
  delay(500);
  
  bootTime = millis() - bootTime;
  snprintf(msg, 75, "Active time: %ld msecs", bootTime);
  Serial.println(msg);
  Serial.println("Now going to sleep. See you on the other side...");
  if(bootTime > 59e3)
    ESP.deepSleep(60*1e6);
  
  ESP.deepSleep((60-(bootTime/1000))*1e6);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

