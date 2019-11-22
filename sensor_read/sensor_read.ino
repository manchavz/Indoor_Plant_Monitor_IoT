/******************************************************************************
   Copyright 2018 Google
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************************************************/
#include "esp32_mqtt.h"
#include "DHTesp.h"
#define DHTpin 32
#define moist 33
#define lite 35

DHTesp dht;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupCloudIoT();
  dht.setup(32, DHTesp::DHT11);

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-25200);
}

unsigned long lastMillis = 0;

void loop() {
    mqttClient->loop();
    delay(1);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected()) {
    connect();
  }

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  
  delay(dht.getMinimumSamplingPeriod());
  float humidity ;
  float temperature ;

  // TODO: replace with your code
  // publish a message roughly every second.
  if (millis() - lastMillis > 12000) {
    lastMillis = millis();
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    int moisture = analogRead(moist);
    int light = analogRead(lite);


    //int val = analogRead(32);

    String payload = 
    "{\"time\":\""+formattedDate+"\""+
    ",\"temp\":"+String(temperature)+
    ",\"moisture\":"+String(moisture)+
    ",\"light\":"+String(light)+"}";
    
    Serial.println(payload + "\n");
    //publishTelemetry(mqttClient, "/sensors", getDefaultSensor());
    publishTelemetry(payload);
  }
}
