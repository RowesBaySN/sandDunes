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

const char* ssid = "TCC-Sensors";
const char* password = "5Birds+Feathers";
const char* nodeID = "sandDunes";

void connectWiFi();

long lastConnectionTime = 0;
int sound_max = 0;
long sound_count = 0;
long sound_sum = 0;
int sound_sample;
int updateInterval = 1*60000;

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

	if (millis() - lastConnectionTime > updateInterval){
		String postData = ("id="+String(nodeID));

		float h = dht.readHumidity();
		float t = dht.readTemperature();
		if (isnan(h) || isnan(t)) {
			Serial.println("Failed to read from DHT sensor!");
			return;
		} else {
			String S1 = String(h);
			String S2 = String(t);
			postData += ("&humidity=" + S1 + "&temperature=" + S2);
		}

		Serial.println(sound_count);
		String S3 = String(round(sound_sum / sound_count));
		String S4 = String(sound_max);
		postData += ("&soundAverage=" + S3 + "&soundMax=" + S4);
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
						postData += ("barometricPressure=" + S6 + "tempBaro=" + S7);
					}
				}
			}
		}

		Serial.println(postData);

    WiFiClient client;
		if (client.connect("159.89.117.177", 8080)) {
			Serial.println("connected");
			client.println("GET /?"+postData);
			client.println("Host: 159.89.117.177");
			client.println("Connection: close");
			client.println();
		}

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
