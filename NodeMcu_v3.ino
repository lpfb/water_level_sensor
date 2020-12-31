#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Ticker.h>

// =======================================
//          HTML Header and body
// =======================================
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
     <h2>Nível de água da Caixa do AP 12</h2>
    
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

// =======================================
//          Global variables
// =======================================
// Wifi Credentials
char ssid[] = "Apartamento 12";       //  your network SSID (name)
char pass[] = "72918348";          //  your network password

// Wifi server port
WiFiServer server(80);

String tmpString = "";
String time_stmp = "";

// NTP parameters
bool new_conection = false;
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -3 * 3600, 60000);
 
void setup() 
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial started at 115200");
    Serial.println();
 
    // Connect to a WiFi network
    Serial.print(F("Connecting to "));  Serial.println(ssid);
    WiFi.begin(ssid, pass);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(500);
    }
 
    Serial.println("");
    Serial.println(F("[CONNECTED]"));
    Serial.print("[IP ");              
    Serial.print(WiFi.localIP()); 
    Serial.println("]");
 
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
} // void setup()

void GET_TIME_ISR()
{
  timeClient.update();
  time_stmp = String(daysOfTheWeek[timeClient.getDay()]) + ", ";
  time_stmp = time_stmp + timeClient.getHours() + ":" + timeClient.getMinutes();
}

void loop() 
{
    // Check if a client has connected
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
} // void loop()
