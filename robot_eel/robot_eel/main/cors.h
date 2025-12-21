#pragma once
#include <WebServer.h>

inline void enableCORS(WebServer& server)
{
    // Others may override per-route CORS but this covers most cases
    server.enableCORS(true);

    // global headers
    DefaultHeaders::Instance()
        .addHeader("Access-Control-Allow-Origin", "*")
        .addHeader("Access-Control-Allow-Methods", "*")
        .addHeader("Access-Control-Allow-Headers", "*");
}
