#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define HTML_CONTENT_IMPLEMENTATION
#define HEADLIGHT_PIN                 (D0) // Define the pin for headlight control
#include "web_api_services.h"

const char* ssid = "Airtel_soum_3262";
const char* password = "air44778";

constexpr uint32_t RESET_STATUS_MAGIC = 0xEC050101;

ESP8266WebServer server(80);

// Queue structure for offloading requests
#define QUEUE_SIZE 10

struct Request {
  ApiRequest apiRequest;
  uint32_t timestamp;
};

void processRequest(Request& req);

Request requestQueue[QUEUE_SIZE];
int queueHead = 0;
int queueTail = 0;
int queueCount = 0;

uint16_t engine_rpm = 0;
uint16_t max_speed = 0;
bool headlight_state = false;
const int HeadlightPin = HEADLIGHT_PIN; // Define the pin for headlight control
// main.cpp
uint8_t resetStatus __attribute__((section(".noinit"))); // 0: No reset, 1: Reset requested
uint32_t resetStatusMagic __attribute__((section(".noinit")));


// Queue management functions
bool enqueueRequest(ESP8266WebServer& requestServer)
{
  if (queueCount >= QUEUE_SIZE) {
    return false; // Queue is full
  }
  ApiRequest& apiRequest = requestQueue[queueTail].apiRequest;
  apiRequest.uri = requestServer.uri();
  apiRequest.method = requestServer.method();
  const int argumentCount = requestServer.args();
  apiRequest.argCount = static_cast<uint8_t>(
    argumentCount < MAX_REQUEST_ARGS ? argumentCount : MAX_REQUEST_ARGS);

  for (uint8_t index = 0; index < apiRequest.argCount; ++index) {
    apiRequest.argNames[index] = requestServer.argName(index);
    apiRequest.argValues[index] = requestServer.arg(index);
  }
  
  requestQueue[queueTail].timestamp = millis();
  queueTail = (queueTail + 1) % QUEUE_SIZE;
  queueCount++;
  return true;
}

bool dequeueRequest(Request* req)
{
  if (queueCount == 0) {
    return false; // Queue is empty
  }
  
  *req = requestQueue[queueHead];
  queueHead = (queueHead + 1) % QUEUE_SIZE;
  queueCount--;
  return true;
}

void handleRoot()
{
  Serial.println("Serving loading page");
  server.send(200, "text/html", loading_page_html);
}

void handleResetStatus()
{
  const bool resetCompleted = resetStatusMagic == RESET_STATUS_MAGIC && resetStatus == 1;
  resetStatus = 0;
  resetStatusMagic = 0;
  server.send(200, "application/json", resetCompleted
    ? "{\"ecu_reset\":true,\"message\":\"ECU reset completed\"}"
    : "{\"ecu_reset\":false}");
}

void processIncomingRequest()
{
  if (enqueueRequest(server))
  {
    Request request;
    dequeueRequest(&request);
    processRequest(request);
  }
  else
  {
    server.send(503, "text/plain", "Queue full");
  }
}

void handleRequest()
{
  processIncomingRequest();
}

void setup() {
  Serial.begin(115200);
  pinMode(D0, OUTPUT);
  pinMode(A0, INPUT);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup server routes
  server.on("/", handleRoot);
  server.on("/reset-status", handleResetStatus);
  server.onNotFound(handleRequest);
  server.on("/diag", handleRequest);

  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void processRequest(Request& req) {
  ApiResponse response = {0};

  Serial.print("Processing request: ");
  Serial.print(req.apiRequest.uri);
  Serial.print(" with method: ");
  Serial.print(req.apiRequest.method == HTTP_GET ? "GET" : "POST");
  Serial.print(" and arguments: ");
  for (uint8_t i = 0; i < req.apiRequest.argCount; ++i) {
    Serial.print(req.apiRequest.argNames[i]);
    Serial.print("=");
    Serial.print(req.apiRequest.argValues[i]);
    if (i < req.apiRequest.argCount - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();


  
  if (req.apiRequest.uri == "/")
  {
    Serial.println("Root request processed");
  }
  else if(req.apiRequest.uri == "/diag")
  {
    response.json = "";
    Error_t ret = web_api_main(req.apiRequest, response);
    if(ret ==  SUCCESS)
    {
      server.send(response.httpStatus, response.contentType, response.json);
      if(resetStatus == 1)
      {
        Serial.println("Resetting ESP8266...");
        delay(100);
        ESP.restart();
      }
    }
    else if(response.json == "")
    {
      web_api_error_response(req.apiRequest, response, ret);
      server.send(response.httpStatus, response.contentType, response.json);
    }
    else
    {
      server.send(static_cast<int>(response.httpStatus), "application/json", response.json);
    }
  } 
  else 
  {
    web_api_error_response(req.apiRequest, response, INVALID_REQUEST);
    server.send(response.httpStatus, response.contentType, response.json);
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
