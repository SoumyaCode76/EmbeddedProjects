#include "webserver.h"

void start_server(WiFiServer& pServer)
{
    pServer.begin();
}

WiFiClient is_any_client_connecting(WiFiServer& pServer)
{
    return pServer.accept();
}

String get_request_from_client(WiFiClient& pClient)
{
    return pClient.readStringUntil('\n');
}

String ResponseObject::request_from_client(ResponseObject& pResp)
{
    return pResp.client.readStringUntil('\n');
}
void ResponseObject::response_loading_page(ResponseObject& pResp, uint8_t status_code)
{
    pResp.prepare_response_header(pResp);
    pResp.ResponseString += loading_page;
    pResp.client.println(pResp.ResponseString);
}



void ResponseObject::respond_error_page(void)
{
    // TODO
}

// helper functions