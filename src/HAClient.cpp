#include "HAClient.h"

HAClient::HAClient(WiFiClientSecure *client, String HA_endpoint, int HA_API_port, String HA_bearer_token)
{
    this->client = client;
    this->HA_endpoint = HA_endpoint;
    this->HA_API_port = HA_API_port;
    this->HA_bearer_token = HA_bearer_token;
}

bool HAClient::updateState(String entity_id, String payload)
{
    bool success=true;
    if (this->client)
    {
        // Open HTTPS connection
        HTTPClient https;
        String uri = HA_UPDATE_STATE_ROUTE;
        uri+= "/";
        uri+= entity_id;
        https.begin(*this->client, this->HA_endpoint, this->HA_API_port, uri, true);

        // Add headers
        https.addHeader("Content-Type", "application/json", false, false);
        https.addHeader("Authorization", "Bearer " + this->HA_bearer_token, false, true);

        Serial.print("[HTTPS] POST...");

        // start connection and send HTTPS header
        int httpCode = https.POST(payload);
        Serial.printf(" code: %d\n", httpCode);

        if (httpCode > 0)
        {
            // print 
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
                //TODO: fix it
                String payload = https.getString();
                Serial.println(payload);
            } else {
                success=false;
            }
        }
        else
        {
            Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
            success=false;
        }
        https.end();
    }
    else
    {
        Serial.printf("[HTTPS] Unable to connect\n");
        success=false;
    }
    return success;
}