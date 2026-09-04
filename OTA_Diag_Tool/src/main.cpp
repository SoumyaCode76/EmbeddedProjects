#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Airtel_soum_3262";
const char* password = "air44778";

ESP8266WebServer server(80);



void handleRoot() {
  server.send(200, "text/html", "<h1>Hello from ESP8266!</h1>");
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

void handleLed() {
  String path = server.uri();
  if (path == "/ledon") {
    digitalWrite(LED_BUILTIN, LOW);
    server.send(200, "text/html", "<h1>LED ON</h1>");
  } else if (path == "/ledoff") {
    digitalWrite(LED_BUILTIN, HIGH);
    server.send(200, "text/html", "<h1>LED OFF</h1>");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup server routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/ledon", handleLed);

  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
