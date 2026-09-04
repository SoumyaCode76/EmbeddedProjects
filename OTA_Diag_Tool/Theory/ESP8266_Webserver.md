#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Airtel_soum_3262";
const char* password = "air44778";

ESP8266WebServer server(80);

// Queue structure for offloading requests
#define QUEUE_SIZE 10

struct Request {
  String uri;
  unsigned long timestamp;
};

Request requestQueue[QUEUE_SIZE];
int queueHead = 0;
int queueTail = 0;
int queueCount = 0;

// Queue management functions
bool enqueueRequest(String uri) {
  if (queueCount >= QUEUE_SIZE) {
    return false; // Queue is full
  }
  
  requestQueue[queueTail].uri = uri;
  requestQueue[queueTail].timestamp = millis();
  queueTail = (queueTail + 1) % QUEUE_SIZE;
  queueCount++;
  return true;
}

bool dequeueRequest(Request* req) {
  if (queueCount == 0) {
    return false; // Queue is empty
  }
  
  *req = requestQueue[queueHead];
  queueHead = (queueHead + 1) % QUEUE_SIZE;
  queueCount--;
  return true;
}



void handleRoot() {
  if (enqueueRequest("/")) {
    server.send(202, "text/plain", "Request queued");
  } else {
    server.send(503, "text/plain", "Queue full");
  }
}

void handleNotFound() {
  if (enqueueRequest(server.uri())) {
    server.send(202, "text/plain", "Request queued");
  } else {
    server.send(503, "text/plain", "Queue full");
  }
}

void handleLed() {
  String path = server.uri();
  if (enqueueRequest(path)) {
    server.send(202, "text/plain", "Request queued");
  } else {
    server.send(503, "text/plain", "Queue full");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(D0, OUTPUT);
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
  server.on("/ledoff", handleLed);

  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void processRequest(Request req) {
  Serial.print("Processing request: ");
  Serial.println(req.uri);
  
  if (req.uri == "/") {
    // Root request processing
    Serial.println("Root request processed");
  } else if (req.uri == "/ledon") {
    digitalWrite(D0, LOW);
    Serial.println("LED turned ON");
  } else if (req.uri == "/ledoff") {
    digitalWrite(D0, HIGH);
    Serial.println("LED turned OFF");
  } else {
    Serial.println("Unknown request");
  }
}

void loop() {
  server.handleClient();
  
  // Process queued requests
  Request req;
  while (dequeueRequest(&req)) {
    processRequest(req);
    delay(10); // Small delay between processing
  }
}
