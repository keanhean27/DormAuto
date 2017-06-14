/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
//visit https://bblanchon.github.io/ArduinoJson/api/index.html for library info
#include <ArduinoJson.h>

#define LATITUDE 0
#define LONGITUDE 1
#define SPEED 2

const char* ssid     = "Kean’s MacBook Air";
const char* password = "77992729";

#define WU_API_KEY "fa4898487cdd215a"
#define WU_LOCATION "TW/Tainan"
#define WU_URL "baseride.com"
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(14, OUTPUT);
  // We start by connecting to a WiFi network

  Serial.println();
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
}

int value = 0;

//in miliseconds
int refresh_rate = 0;

void loop() {

  if((millis() <= refresh_rate+500 && millis() >= refresh_rate-500) || value == 0)
  {
    digitalWrite(14, HIGH);
    Serial.print("connecting to ");
    Serial.println(WU_URL);
  
  // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect("baseride.com", httpPort)) {
      Serial.println("connection failed");
      return;
    }
  
  // We now create a URI for the request
    String url = "https://baseride.com/routes/apigeo/routevariantvehicle/44479/?format=json HTTP/1.1\r\n"
    "User-Agent: ESP8266/0.1\r\n"
    "Accept: */*\r\n"
    "Host: baseride.com" "\r\n"
    "Connection: close\r\n"
    "\r\n";

  
  //Serial.print("Requesting URL: ");
  //Serial.println(url);
  
  // This will send the request to the server
    client.print(String("GET ") + url);
    unsigned long timeout = millis();
    while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
      }
    }
    static char respBuffer[8192];

    unsigned int index =0 ;
  
    String line;

  // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      respBuffer[index++] = client.read();
        delay(1);
    //Serial.println(line);
    }

    client.stop();
  
    char * json = strchr(respBuffer,'{');

    DynamicJsonBuffer jBuffer;
    JsonObject& root = jBuffer.parseObject(json);
    JsonArray& vehicles1 = root["vehicles"];
    
    int vehicle_no1 = vehicles1.size();
    float bus_loc1[3][5];
    index = 0;

    while( index < vehicle_no1  )
    {  
       
       JsonObject& bus1 = vehicles1[index];
       JsonObject& bus_pos1 = bus1["position"];
       String latitude1 = bus_pos1["lat"];
       String longitude1 = bus_pos1["lon"];
       String bus_speed1 = bus_pos1["speed"];

       bus_loc1[LATITUDE][index] = latitude1.toFloat();
       bus_loc1[LONGITUDE][index] = longitude1.toFloat();
       bus_loc1[SPEED][index] = bus_speed1.toFloat();

       Serial.println(bus_loc1[LATITUDE][index],6);
       Serial.println(bus_loc1[LONGITUDE][index],6);
       Serial.println(bus_loc1[SPEED][index]);
       index++;
    }
    

      
    JsonObject& vehicles = root["vehicles"][1];
    JsonObject& bus_pos = vehicles["position"];
    String bus_name = root["routename"];
    String latitude = bus_pos["lat"];
    String longitude = bus_pos["lon"];
   
    String bus_speed = bus_pos["speed"];
    String msg = "Latitude = "+latitude+", Longitude = "+longitude+", Speed: "+bus_speed;
  
    Serial.println();
    Serial.println(bus_name);
    Serial.println(msg);
    Serial.println(bus_loc1[LATITUDE][1],6);
    Serial.println(bus_loc1[LONGITUDE][1],6);
    Serial.println(bus_loc1[SPEED][1]);
    Serial.print("number of vehicles: ");
    Serial.println(vehicles1.size());
    Serial.print("number of recorded vehicles: ");
    Serial.println(index);
    Serial.print("closing connection. Data at time: ");
    Serial.println(millis());
    ++value;
    refresh_rate = refresh_rate + 60000;
    Serial.print("Next refresh at: ");
    Serial.println(refresh_rate);
    Serial.println();
    }

  digitalWrite(14, LOW);
    
}

