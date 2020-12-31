#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <NTPClient.h>

// WiFi Credentials
#ifndef STASSID
#define STASSID "Apartamento 12"
#define STAPSK  "72918348"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Wifi server port
WiFiServer server(80);

// NTP parameters and configurations
bool new_conection = false;
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -3 * 3600, 60000);

// =======================
// HTML Related configs
// =======================
String tmpString = "";
String time_stmp = "";

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
 
String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
 <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 <meta charset='utf-8'>
 <meta http-equiv='refresh' content='5'>
 <style>
   body {font-size:100%;} 
   #main {display: table; margin: auto;  padding: 0 10px 0 10px; } 
   h2 {text-align:center; } 
   p { text-align:left; }
 </style>
   <title>Medição de nível</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>OTA - Nível de água da Caixa do AP 12</h2>
    
    <div id='wifi_pwr'> 
       <p>WiFI Power: %wifi_pwr%</p>
    </div>

    <div id='time_stmp'> 
       <p>Date: %time_stmp%</p>
    </div>
   </div>
 </body>
</html>
)====="; 


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // start a server
  server.begin();
  Serial.println("Server started");

  // Starting time NTP Client
  timeClient.begin();

  // Configuring timer
  // TIM_DIV16 = 1,  // ESp clock 80MHz/16 = 5MHz = 200 ns
  timer1_attachInterrupt(GET_TIME_ISR); // Add ISR Function
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(50000000); // Configure timer counter to 1 s
}

void GET_TIME_ISR()
{
  timeClient.update();
  time_stmp = String(daysOfTheWeek[timeClient.getDay()]) + ", ";
  time_stmp = time_stmp + timeClient.getHours() + ":" + timeClient.getMinutes();
}

void loop() {
  ArduinoOTA.handle();
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  String wifi_pwr = String(WiFi.RSSI()); // Getting WiFi power
  
  tmpString = html_1;
  tmpString.replace("%wifi_pwr%", String(wifi_pwr));
  tmpString.replace("%time_stmp%", time_stmp);
  
  client.flush();
  client.print( header );
  client.print( tmpString );  
  
  Serial.print("Wifi PWR(dBm): ");
  Serial.println(wifi_pwr); 
  
  delay(5);
}
