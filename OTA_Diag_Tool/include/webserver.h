#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <iostream>

using namespace std;

String html_type = "text/html";
String json_type = "application/json";
String plain_type = "text/plain";
String loading_page ="<!DOCTYPE HTML>\n \
                    <html>\n \
                    <h1>Welcome to the Webpage!</h1>\n \
                    <h3>LED Controls<h3>\n \
                    <br><a href=\"/ledon\"\"><button>LED ON</button></a>\n \
                    <a href=\"/ledoff\"\"><button>LED OFF</button></a><br/>";

String loading_page_new = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 Control Panel</title>
    <style>
        /* Paste the CSS here */
    </style>
</head>
<body>
    <!-- Paste the HTML body here -->
    <script>
        // Paste the JavaScript here
    </script>
</body>
</html>)";


String response_code[] = {"200", "204"};

String html_class = "HTTP/1.1";

class ResponseObject{
    WiFiClient client;
    String RequestString;
    String ResponseString;
    void prepare_response_header(ResponseObject& pResp, const char* content_type = "html", uint8_t status = 200)
    {
        pResp.ResponseString += html_class + " ";
        if(status == 200 || status == 204)
        {
            pResp.ResponseString += to_string(status) + " OK\n";
            if(content_type == "plain")
            {
                pResp.ResponseString += "Content-Type: " + plain_type + "\n\n"; 
            }
            else if(content_type == "html")
            {
                pResp.ResponseString += "Content-Type: " + html_type + "\n\n";                 
            }
            else if(content_type == "json")
            {
                pResp.ResponseString += "Content-Type: " + json_type + "\n\n";
            }
        }
        else
        {
            pResp.ResponseString += to_string(400) + " Bad Request\n";
            pResp.ResponseString += "Content-Type: " + plain_type + "\n";
            pResp.ResponseString += "Connection: close\n\n";
            pResp.ResponseString += "Invalid request\n";
        }
    }
    public:
        ResponseObject()
        {
            ;
        }
        WiFiClient get_client_of_response_object(void)
        {
            return client;
        }
        void set_client_of_response_object(WiFiClient pClient)
        {
            client = pClient;
        }
        String get_response_string_prepared(void)
        {
            return ResponseString;
        }
        void append_to_response_string(String response_content)
        {
            ResponseString += response_content;
        }
        void set_new_response_string(String response_content)
        {
            ResponseString = response_content;
        }
        String request_from_client(ResponseObject&);
        void response_loading_page(ResponseObject&, uint8_t status_code);       
        void respond_error_page();
};

void start_server(WiFiServer& pServer);
WiFiClient is_any_client_connecting(WiFiServer& pServer);
String get_request_from_client(WiFiClient& pClient);




#endif