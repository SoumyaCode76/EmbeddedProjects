#include <Arduino.h>
#include <ESP8266WiFi.h>  // Include the ESP8266 Wi‑Fi library so the chip can connect to a wireless network.
#include <ESP8266WebServer.h>
#include "webserver.h"

#define AP_OR_STA                 (1)     // 1 for STA, 2 for AP, 3 for both

#if (AP_OR_STA == 1)
const char* ssid = "Airtel_soum_3262";      // Wi‑Fi network name (replace with your router's SSID).
const char* password = "air44778";  // Wi‑Fi password (replace with your router's password).
#elif (AP_OR_STA == 2)
const char* ssid = "BatNode";      // Wi‑Fi network name (replace with your router's SSID).
const char* password = "12345678";  // Wi‑Fi password (replace with your router's password).
#endif


const uint16_t port[4] = {80};

WiFiServer server(port[0]);
WiFiClient client;

void setup() {
  Serial.begin(115200);  // Start the serial monitor at 115200 baud for debug output.
  delay(1000);           // Wait a little before trying to connect so the serial port is ready.

#if (AP_OR_STA == 1)
  WiFi.mode(WIFI_STA);   // Set the ESP8266 to station mode so it connects to an existing Wi‑Fi router.
  WiFi.begin(ssid, password);  // Start connecting to the Wi‑Fi network using the SSID and password.
  Serial.print("Connecting to WiFi");  // Print a message to the serial monitor before connection begins.
  while (WiFi.status() != WL_CONNECTED) {  // Keep waiting until the ESP8266 successfully connects.
    delay(500);                            // Wait 500 ms between retry attempts.
    Serial.print(".");                    // Print a dot for each retry so the user can see the connection progress.
  }
#elif (AP_OR_STA == 2)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
#endif

  Serial.println();  // Move to the next line after the dots.
  Serial.print("Connected to WiFi");  // Print a success message when the connection is complete.
  Serial.println();  // Move to the next line.
  Serial.print("IP Address: ");  // Label the next value as the assigned IP address.
#if (AP_OR_STA == 1)
  Serial.println(WiFi.localIP());  // Print the ESP8266 IP address on the network.
#elif (AP_OR_STA == 2)
  Serial.println(WiFi.softAPIP());
#endif
  start_server(server);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);  
}

void loop() {
  // This example keeps the board connected and does nothing else in the loop.
  // You can add sensors, web server code, MQTT, etc. here.
  client = is_any_client_connecting(server);
  if(!client)
  {
    return;
  }
  else
  {
    ResponseObject resp;
    resp.set_client_of_response_object(client);
    Serial.println("Client connected");
    Serial.print("Client IP Address: ");
    Serial.println(resp.get_client_of_response_object().remoteIP());
    // while(client.connected())
    // {
    //   if(client.available())
    //   {
        
    String request = resp.request_from_client(resp);
    request.trim();
    Serial.println(request);

    // client.println("<a href=\"/led2on\"\"><button>LED 2 ON</button></a>");
    // client.println("<a href=\"/led2off\"\"><button>LED 2 OFF</button></a><br/>");
    if(request.indexOf("/ ") != -1)
    {
        digitalWrite(D0, HIGH);  
        resp.response_loading_page(resp, 200);
        resp.get_client_of_response_object().println(resp.get_response_string_prepared());
    }
    else if(request.indexOf("/ledon") != -1)
    {
        digitalWrite(D0, LOW);
        resp.append_to_response_string("<h3><b><u>LED ON</u></b></h3>\n</html>");
        resp.get_client_of_response_object().println(resp.get_response_string_prepared());
    }
    else if(request.indexOf("/ledoff") != -1)
    {
        digitalWrite(D0, HIGH);
        client.println("<h3><b><u>LED OFF</u></b></h3>\n</html>");                       
        resp.get_client_of_response_object().println(resp.get_response_string_prepared());             
    }
    //   }
    // }
    // client.stop();
  }
  delay(1000);  // Wait 1 second before the next loop iteration.
}