#ifndef HACLIENT_H
#define HACLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const String HA_UPDATE_STATE_ROUTE = "/api/states";

class HAClient
{
public:
    WiFiClientSecure *client;
    String HA_bearer_token;
    String HA_endpoint;
    int HA_API_port;
    HAClient(WiFiClientSecure *client, String HA_endpoint, int HA_API_port, String HA_bearer_token);
    bool updateState(String entity_id, String payload);
};
#endif