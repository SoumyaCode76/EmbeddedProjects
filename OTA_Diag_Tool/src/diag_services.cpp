#include "web_api_services.h"
#include "diag_services.h"

read_svc_callback_t read_svc_callback_functions[static_cast<size_t>(Diag_API_Requests_t::MAX_REQUESTS)] = {
    read_diag_F190_handler,
    read_diag_1001_handler,
    read_diag_2001_handler,
    read_diag_CF00_handler
};

write_svc_callback_t write_svc_callback_functions[static_cast<size_t>(Diag_API_Requests_t::MAX_REQUESTS)] = {
    NULL,
    NULL,
    NULL,
    NULL,
    write_diag_2001_handler,
    write_diag_CF00_handler,
};

general_svc_callback_t svc_callback_functions[static_cast<size_t>(General_Service_API_Requests_t::MAX_REQUESTS)] = {
    svc_callback_3E_handler,
    svc_callback_11_handler
};

Error_t read_diag_F190_handler(ApiResponse& response)
{
    const String vin = VEHICLE_IDENTIFICATION_NUMBER;

    response.httpStatus = 200;
    response.contentType = "application/json";
    response.decodedResponse = vin;
    response.udsResponse = "62 F1 90 57 56 57 5A 5A 5A 31 4A 5A 58 57 30 30 30 30 30 31";
    response.canFrames = "[\"10 14 62 F1 90 57 56\",\"21 57 5A 5A 5A 31 4A 5A\",\"22 58 57 30 30 30 30 30\",\"23 31 00 00 00 00 00 00\"]";
    response.json = "{\"service_id\":\"22\",\"sub_id\":\"F190\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"vin\":\"" +
                    response.decodedResponse + "\"},\"can_frames\":" +
                    response.canFrames + "}";

    return SUCCESS;
}
Error_t read_diag_1001_handler(ApiResponse& response)
{ 
    static bool randomInitialized = false;
    if (!randomInitialized)
    {
        randomSeed(analogRead(A0) ^ micros());
        randomInitialized = true;
    }

    const uint16_t rpm = static_cast<uint16_t>(random(800, 4001));
    const uint16_t rawRpm = rpm * 4;
    const uint8_t highByte = static_cast<uint8_t>(rawRpm >> 8);
    const uint8_t lowByte = static_cast<uint8_t>(rawRpm & 0xFF);
    engine_rpm = rpm;

    String highHex = String(highByte, HEX);
    String lowHex = String(lowByte, HEX);
    highHex.toUpperCase();
    lowHex.toUpperCase();
    if (highHex.length() < 2) highHex = "0" + highHex;
    if (lowHex.length() < 2) lowHex = "0" + lowHex;

    response.httpStatus = 200;
    response.contentType = "application/json";
    response.udsResponse = "62 10 01 " + highHex + " " + lowHex;
    response.decodedResponse = "Engine RPM: " + String(rpm) + " RPM";
    response.canFrames = "[\"05 " + response.udsResponse + " 00 00\"]";
    response.json = "{\"service_id\":\"22\",\"sub_id\":\"1001\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"engine_rpm\":" +
                    String(rpm) + "},\"can_frames\":" + response.canFrames + "}";
    return SUCCESS;
}
Error_t read_diag_2001_handler(ApiResponse& response) 
{ 
    response.httpStatus = 200;
    response.contentType = "application/json";
    response.udsResponse = "62 20 01 " + String(max_speed, HEX);
    response.decodedResponse = "Maximum Speed: " + String(max_speed) + " km/h";
    response.canFrames = "[\"05 " + response.udsResponse + " 00 00\"]";
    response.json = "{\"service_id\":\"22\",\"sub_id\":\"2001\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"max_speed\":" +
                    String(max_speed) + "},\"can_frames\":" + response.canFrames + "}";

    return SUCCESS;
}
Error_t read_diag_CF00_handler(ApiResponse& response)
{
    response.httpStatus = 200;
    response.contentType = "application/json";
    response.udsResponse = "62 CF 00 " + String(headlight_state ? "01" : "00");
    response.decodedResponse = headlight_state ? "Headlights ON" : "Headlights OFF";
    response.canFrames = "[\"05 " + response.udsResponse + " 00 00\"]";
    response.json = "{\"service_id\":\"22\",\"sub_id\":\"CF00\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"headlight_state\":" +
                    (headlight_state ? "\"ON\"" : "\"OFF\"") + "},\"can_frames\":" +
                    response.canFrames + "}";

    return SUCCESS;
}

Error_t write_diag_2001_handler(ApiRequest& request, ApiResponse& response)
{
    if (request.argCount < 3 || request.argNames[2] != "value")
    {
        response.httpStatus = 400;
        response.json = "{\"error\":\"value parameter is required for writing maximum speed\"}";
        return INVALID_REQUEST;
    }

    int newMaxSpeed = request.argValues[2].toInt();
    if (newMaxSpeed < 0 || newMaxSpeed > 300)
    {
        response.httpStatus = 400;
        response.json = "{\"error\":\"value must be between 0 and 300\"}";
        return INVALID_REQUEST;
    }

    max_speed = static_cast<uint16_t>(newMaxSpeed);
    response.httpStatus = 200;
    response.contentType = "application/json";
    response.decodedResponse = "Maximum speed set to: " + String(max_speed) + " km/h";
    response.udsResponse = "6E 20 01 " + String(max_speed, HEX);
    response.canFrames = "[\"05 " + response.udsResponse + " 00 00\"]";
    response.json = "{\"service_id\":\"2E\",\"sub_id\":\"2001\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"max_speed\":" +
                    String(max_speed) + "},\"can_frames\":" + response.canFrames + "}";

    return SUCCESS;
}

Error_t write_diag_CF00_handler(ApiRequest& request, ApiResponse& response)
{
    if (request.argCount < 3 || request.argNames[2] != "value")
    {
        response.httpStatus = 400;
        response.json = "{\"error\":\"value parameter is required for writing headlight state\"}";
        return INVALID_REQUEST;
    }

    String value = request.argValues[2];
    if (value != "0" && value != "1")
    {
        response.httpStatus = 400;
        response.json = "{\"error\":\"value must be '0' (OFF) or '1' (ON)\"}";
        return INVALID_REQUEST;
    }

    headlight_state = (value == "1");
    digitalWrite(HeadlightPin, headlight_state ? LOW : HIGH); // Assuming LOW turns the headlights ON and HIGH turns them OFF
    delay(5); // Allow time for the hardware to respond
    response.httpStatus = 200;
    response.contentType = "application/json";
    response.decodedResponse = headlight_state ? "Headlights turned ON" : "Headlights turned OFF";
    response.udsResponse = "6E CF 00 " + String(headlight_state ? "01" : "00");
    response.canFrames = "[\"05 " + response.udsResponse + " 00 00\"]";
    response.json = "{\"service_id\":\"2E\",\"sub_id\":\"CF00\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"headlight_state\":" +
                    (headlight_state ? "\"ON\"" : "\"OFF\"") + "},\"can_frames\":" +
                    response.canFrames + "}";

    return SUCCESS;
}

Error_t svc_callback_3E_handler(ApiRequest& request, ApiResponse& response)
{
    response.httpStatus = 200;
    response.contentType = "application/json";
    response.decodedResponse = "Tester present acknowledged";
    response.udsResponse = "7E 00";
    response.canFrames = "[\"02 " + response.udsResponse + " 00 00 00\"]";
    response.json = "{\"service_id\":\"3E\",\"sub_id\":\"00\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"tester_present\":\"acknowledged\"},\"can_frames\":" +
                    response.canFrames + "}";

    return SUCCESS;
}

Error_t svc_callback_11_handler(ApiRequest& request, ApiResponse& response)
{
    response.httpStatus = 200;
    response.contentType = "application/json";
    response.decodedResponse = "ECU reset command acknowledged";
    response.udsResponse = "51 01";
    response.canFrames = "[\"02 " + response.udsResponse + " 00 00 00\"]";
    response.json = "{\"service_id\":\"11\",\"sub_id\":\"01\",\"uds_response\":\"" +
                    response.udsResponse + "\",\"decoded\":{\"ecu_reset\":\"acknowledged\"},\"can_frames\":" +
                    response.canFrames + "}";
    resetStatus = 1;
    resetStatusMagic = 0xEC050101;
    return SUCCESS;
}

WiFiClient ResponseObject::get_client_of_response_object(void)
{
    return client;
}
void ResponseObject::set_client_of_response_object(WiFiClient pClient)
{
    client = pClient;
}
void ResponseObject::prepare_response_header(ResponseObject &pResp, 
    const char *content_type, uint8_t status)
{
    pResp.ResponseString += html_class + " ";
    if (status == 200 || status == 204)
    {
        pResp.ResponseString += String(status) + " OK\n";
        if (strcmp(content_type, "plain") == 0)
        {
            pResp.ResponseString += "Content-Type: " + plain_type + "\n\n";
        }
        else if (strcmp(content_type, "html") == 0)
        {
            pResp.ResponseString += "Content-Type: " + html_type + "\n\n";
        }
        else if (strcmp(content_type, "json") == 0)
        {
            pResp.ResponseString += "Content-Type: " + json_type + "\n\n";
        }
    }
    else
    {
        pResp.ResponseString += String(400) + " Bad Request\n";
        pResp.ResponseString += "Content-Type: " + plain_type + "\n";
        pResp.ResponseString += "Connection: close\n\n";
        pResp.ResponseString += "Invalid request\n";
    }
}
String ResponseObject::get_response_string_prepared(ResponseObject& pResp)
{
    return pResp.ResponseString;
}
void ResponseObject::append_to_response_string(ResponseObject& pResp, String response_content)
{
    pResp.ResponseString += response_content;
}
void ResponseObject::set_new_response_string(ResponseObject& pResp, String response_content)
{
    pResp.ResponseString = response_content;
}        

Error_t web_api_main(ApiRequest& request, ApiResponse& response)
{
    response.json = "";
    response.canFrames = "";
    response.contentType = "";
    response.decodedResponse = "";
    response.httpStatus = 200;
    response.udsResponse = "";

    if (request.argCount < 2)
    {
        response.httpStatus = 400;
        response.json = "{\"error\":\"service-id and sub-id are required\"}";
        return INVALID_REQUEST;
    }
    if (request.argNames[0] != "service-id" || request.argNames[1] != "sub-id")
    {
        response.httpStatus = 400;
        response.json = "{\"error\":\"invalid diagnostic parameter names\"}";
        return INVALID_REQUEST;
    }
    if (request.argValues[0] == "22")
    {
        if(request.argValues[1] == "F190")
        {
            return read_svc_callback_functions[static_cast<uint8_t>(Diag_API_Requests_t::READ_VEHICLE_ID_NUMBER)](response);
        }
        else if(request.argValues[1] == "1001")
        {
            return read_svc_callback_functions[static_cast<uint8_t>(Diag_API_Requests_t::READ_ENGINE_RPM)](response);
        }
        else if(request.argValues[1] == "2001")
        {
            return read_svc_callback_functions[static_cast<uint8_t>(Diag_API_Requests_t::READ_MAX_SPEED_LIMIT)](response);
        }        
        else if(request.argValues[1] == "CF00")
        {
            return read_svc_callback_functions[static_cast<uint8_t>(Diag_API_Requests_t::READ_HEADLIGHT_STATE)](response);
        }
        else
        {
            return UNKNOWN_DID_RID;
        }
    }
    else if (request.argValues[0] == "2E")
    {
        Serial.println("Write service request received");
        response.httpStatus = 200;
        response.contentType = "application/json";
        if(request.argValues[1] == "2001")
        {
            return write_svc_callback_functions[static_cast<uint8_t>(Diag_API_Requests_t::WRITE_MAX_SPEED_LIMIT)](request, response);
        }
        else if(request.argValues[1] == "CF00")
        {
            return write_svc_callback_functions[static_cast<uint8_t>(Diag_API_Requests_t::SET_HEADLIGHT_STATE)](request, response);
        }
        else
        {
            return UNKNOWN_DID_RID;
        }
    }
    else if (request.argValues[0] == "3E" && request.argValues[1] == "00")
    {
        return svc_callback_3E_handler(request, response);
    }
    else if (request.argValues[0] == "11" && request.argValues[1] == "01")
    {
        return svc_callback_11_handler(request, response);
    }
    else
    {
        response.httpStatus = 404;
        response.json = "{\"error\":\"unsupported diagnostic request\"}";
        return UNKNOWN_SERVICE;
    }
}

void web_api_error_response(ApiRequest& request, ApiResponse& response, Error_t error_state)
{
    uint8_t nrc = 0x10;
    const char* description = "General reject";

    switch (error_state)
    {
        case INVALID_REQUEST:
            nrc = 0x13;
            description = "Invalid format";
            break;
        case UNKNOWN_SERVICE:
            nrc = 0x11;
            description = "Service not supported";
            break;
        case UNKNOWN_SUB_SERVICE:
            nrc = 0x12;
            description = "sub-function not supported";
            break;        
        case UNKNOWN_DID_RID:
            nrc = 0x31;
            description = "Request out of range";
            break;
        case INVALID_SECURITY_LEVEL:
            nrc = 0x33;
            description = "Security access denied";
            break;
        case INVALID_SESSION:
            nrc = 0x7F;
            description = "Service not supported in active session";
            break;
        case REQUEST_PENDING:
            nrc = 0x78;
            description = "Response pending";
            break;
        case SUCCESS:
            response.httpStatus = 200;
            response.contentType = "application/json";
            response.json = "{\"error\":null}";
            return;
        default:
            nrc = 0x10;
            description = "General reject";
    }

    const String serviceId = request.argCount > 0 ? request.argValues[0] : "00";
    const String subId = request.argCount > 1 ? request.argValues[1] : "00";
    String nrcHex = String(nrc, HEX);
    nrcHex.toUpperCase();
    if (nrcHex.length() < 2) nrcHex = "0" + nrcHex;

    response.httpStatus = error_state == REQUEST_PENDING ? 202 : 400;
    response.contentType = "application/json";
    response.decodedResponse = String("NRC ") + nrcHex + ": " + description;
    response.udsResponse = "7F " + serviceId + " " + nrcHex;
    response.canFrames = "[\"03 " + response.udsResponse + " 00 00 00 00\"]";
    response.json = "{\"service_id\":\"" + serviceId +
                    "\",\"sub_id\":\"" + subId +
                    "\",\"uds_response\":\"" + response.udsResponse +
                    "\",\"decoded\":{\"nrc\":\"" + nrcHex +
                    "\",\"description\":\"" + description +
                    "\"},\"can_frames\":" + response.canFrames + "}";
}

