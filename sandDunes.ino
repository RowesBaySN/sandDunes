#include <ESP8266WiFi.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>

#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
const int microphonePin = A0;
SFE_BMP180 pressure;
#define ALTITUDE 20.0


const char* ssid = "TEHS_297D59";
const char* password = "8847C9C512";


void connectWiFi();


long lastConnectionTime = 0;
int sound_max = 0;
long sound_count = 0;
long sound_sum = 0;
int sound_sample;
int updateInterval = 1*60000;

Initial running script
void setup() {
	Serial.begin(115200);
	delay(10);

	dht.begin();
	pressure.begin();
}


void loop(){
	if (WiFi.status() != WL_CONNECTED){connectWiFi();}

	sound_sample= analogRead(microphonePin);
	if (sound_max < sound_sample) {sound_max = sound_sample;}
	sound_sum += sound_sample;
	sound_count += 1;

	if (millis() - lastConnectionTime > updateInteval){
		String postData = ("");

		float h = dht_0.readHumidity();
		float t = dht_0.readTemperature();
		if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    } else {
      String S1 = String(h);
      String S2 = String(t);
      postData += ("field1=" + S1 + "&field2=" + S2);
		}

		Serial.println(sound_count);
    String S3 = String(round(sound_sum / sound_count));
    String S4 = String(sound_max);
    if (postData != "") { postData += ("&"); }
    postData += ("field3=" + S3 + "&field4=" + S4);
		sound_sum = 0; sound_max = 0; sound_count = 0;

		char status;
		double T,P,p0;
		status = pressure.startTemperature();
		if (status != 0) {
			delay(status);
			status = pressure.getTemperature(T);
			if (status != 0) {
				status = pressure.startPressure(3);
				if (status != 0) {
					delay(status);
					status = pressure.getPressure(P,T);
					if (status != 0) {
						p0 = pressure.sealevel(P,ALTITUDE);
						String S6 = String(p0);
						String S7 = String(T);
						if (postData != "") { postData += ("&"); }
						postData += ("field6=" + S6 + "&field7=" + S7);
					}
				}
			}
		}

		Serial.println(postData);

		//Send Data Code here


		lastConnectionTime = millis();
	}
	delay(5);
}
void connectWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
