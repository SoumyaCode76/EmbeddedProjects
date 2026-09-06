#ifndef __WEB_API_SERVICES_H__
#define __WEB_API_SERVICES_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <iostream>

#include "html_content.h"

// Only software functions hooks and objects. No server level functions to be defined

typedef enum __error_t
{
    SUCCESS,
    INVALID_REQUEST,
    UNKNOWN_SERVICE,
    UNKNOWN_SUB_SERVICE,
    UNKNOWN_DID_RID,
    INVALID_SECURITY_LEVEL,
    INVALID_SESSION,
    REQUEST_PENDING
}Error_t;


using namespace std;

#define MAX_REQUEST_ARGS 8

struct ApiRequest
{
    String uri;
    HTTPMethod method;
    uint8_t argCount;
    String argNames[MAX_REQUEST_ARGS];
    String argValues[MAX_REQUEST_ARGS];
};

struct ApiResponse
{
    uint16_t httpStatus = 200;
    String contentType = "application/json";
    String udsResponse;
    String decodedResponse;
    String canFrames;
    String json;
};

extern uint16_t engine_rpm;
extern uint16_t max_speed;
extern bool headlight_state;
extern const int HeadlightPin;

extern uint8_t resetStatus;
extern uint32_t resetStatusMagic;

class ResponseObject;

typedef Error_t (*read_svc_callback_t)(ApiResponse& response);
typedef Error_t (*write_svc_callback_t)(ApiRequest& request, ApiResponse& response);
typedef Error_t (*general_svc_callback_t)(ApiRequest& request, ApiResponse& response);

typedef enum class __diag_api_requests_t
{
    READ_VEHICLE_ID_NUMBER,         // F190
    READ_ENGINE_RPM,                // 1001
    READ_MAX_SPEED_LIMIT,           // 2001
    READ_HEADLIGHT_STATE,           // CF00
    WRITE_MAX_SPEED_LIMIT,          // 2001
    SET_HEADLIGHT_STATE,            // CF00

    MAX_REQUESTS = 6
}Diag_API_Requests_t;

typedef enum class __gen_svc_api_requests_t
{
    TESTER_PRESENT,                  // 3E
    ECU_RESET,                       // 11          
    MAX_REQUESTS = 2
}General_Service_API_Requests_t;

Error_t read_diag_F190_handler(ApiResponse& response);
Error_t read_diag_1001_handler(ApiResponse& response);
Error_t read_diag_2001_handler(ApiResponse& response);
Error_t read_diag_CF00_handler(ApiResponse& response);

Error_t write_diag_2001_handler(ApiRequest& request, ApiResponse& response);
Error_t write_diag_CF00_handler(ApiRequest& request, ApiResponse& response);

Error_t svc_callback_3E_handler(ApiRequest& request, ApiResponse& response);
Error_t svc_callback_11_handler(ApiRequest& request, ApiResponse& response);

extern read_svc_callback_t read_svc_callback_functions[static_cast<size_t>(Diag_API_Requests_t::MAX_REQUESTS)];
extern write_svc_callback_t write_svc_callback_functions[static_cast<size_t>(Diag_API_Requests_t::MAX_REQUESTS)];
extern general_svc_callback_t svc_callback_functions[static_cast<size_t>(General_Service_API_Requests_t::MAX_REQUESTS)];

class ResponseObject{
    public:
        WiFiClient client;
        String RequestString;
        String ResponseString;    
        ResponseObject()
        {
        }
        WiFiClient get_client_of_response_object(void);
        void set_client_of_response_object(WiFiClient pClient);
        void prepare_response_header(ResponseObject &pResp, 
            const char *content_type = "html", uint8_t status = 200);
        String get_response_string_prepared(ResponseObject& pResp);
        void append_to_response_string(ResponseObject& pResp, 
            String response_content);
        void set_new_response_string(ResponseObject& pResp, 
            String response_content);
};

/* 
    * This is the router function to web-api handlers.
    * To make it flexible and scalable, we shall make a LUT based design
    * wherein the respective callback function will be called when the particular api service
    * enum value matches. The enum values will act as indices for the LUT.
*/
Error_t web_api_main(ApiRequest&, ApiResponse&);
void web_api_error_response(ApiRequest&, ApiResponse&, Error_t error_state);

#endif