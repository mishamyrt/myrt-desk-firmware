// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#include "mServer.h"
#include <Height.h>

const char *field_height PROGMEM = "height";

void registerLegsHandlers(mServer *server, HeightController *height) {
  server->addRoute("/legs")
    .get([height](AsyncWebServerRequest *request) {
      AsyncJsonResponse * response = new AsyncJsonResponse();
      JsonVariant& json = response->getRoot();
      json[field_height] = height->get();
      response->setLength();
      request->send(response);
    })
    .put([height, server](AsyncWebServerRequest *request, JsonVariant &json) {
      if (!json.containsKey(field_height)) {
        return server->sendStatus(request, REQUEST_BAD);
      }
      height->set(json[field_height]);
      server->sendStatus(request, REQUEST_SUCCESS);
    });
    // .post("/calibrate", [server, height]() {
    //   server->sendStatus(REQUEST_SUCCESS);
    // });
}
