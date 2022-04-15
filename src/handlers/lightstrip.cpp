#include "Arduino.h"
#include "Lightstrip.h"
#include "mServer.h"
#include "FirmwareReader.h"

const char *field_brightness PROGMEM = "brightness";
const char *field_enabled PROGMEM = "enabled";
const char *field_color PROGMEM = "color";
const char *field_active PROGMEM = "active";
const char *field_temperature PROGMEM = "temperature";

void registerLightstripHandlers(mServer *server, Lightstrip *light, FirmwareReader *reader) {

  server->addRoute("/lightstrip")
    .get([light](AsyncWebServerRequest *request) {
      AsyncJsonResponse * response = new AsyncJsonResponse();
      JsonVariant& json = response->getRoot();
      json[field_brightness] = light->brightness();
      json[field_temperature] = light->temperature();
      json[field_enabled] = light->enabled();
      JsonArray color = json.createNestedArray(field_color);
      RGB strip_color = light->color();
      color.add(strip_color.r);
      color.add(strip_color.g);
      color.add(strip_color.b);
      response->setLength();
      request->send(response);
    })

    // .put("/effects", [light, server](AsyncWebServerRequest *request, JsonVariant &json) {
    //   if (!json.containsKey(field_active)) {
    //     return server->sendStatus(request, REQUEST_BAD);
    //   }
    //   bool is_success = light->setEffect(json[field_active]);
    //   server->sendStatus(request, is_success ? REQUEST_SUCCESS : REQUEST_ERROR);
    // })

    // .put("/progress", [light, server](AsyncWebServerRequest *request, JsonVariant &json) {
    //   if (!json.containsKey(field_color)) {
    //     return server->sendStatus(request, REQUEST_BAD);
    //   }
    //   bool is_success = light->setProgressColor(
    //     json[field_color][0],
    //     json[field_color][1],
    //     json[field_color][2]
    //   );
    //   server->sendStatus(request, is_success ? REQUEST_SUCCESS : REQUEST_ERROR);
    // })

    .put("/color", [server, light](AsyncWebServerRequest *request, JsonVariant &json) {
      if (!json.containsKey(field_brightness)
        ||!(json.containsKey(field_color) || json.containsKey(field_temperature))
        ) {
        return server->sendStatus(request, REQUEST_BAD);
      }
      bool planned;
      if (json.containsKey(field_color)) {
        planned = light->setColor(
          json[field_brightness],
          json[field_color][0],
          json[field_color][1],
          json[field_color][2]
        );
      } else {
        planned = light->setTemperature(
          json[field_brightness],
          json[field_temperature]
        );
      }
      server->sendStatus(request, planned ? REQUEST_SUCCESS : REQUEST_ERROR);
    })

    .put("/power", [server, light](AsyncWebServerRequest *request, JsonVariant &json) {
      if (!json.containsKey(field_enabled)) {
        return server->sendStatus(request, REQUEST_BAD);
      }
      if (json[field_enabled] == light->enabled()) {
        return server->sendStatus(request, REQUEST_SUCCESS);
      }
      bool is_success;
      if (json[field_enabled]) {
        is_success = light->powerOn();
      } else {
        is_success = light->powerOff();
      }
      server->sendStatus(request, is_success ? REQUEST_SUCCESS : REQUEST_ERROR);
    })

    .postRaw("/update-firmware", [server, light, reader](AsyncWebServerRequest *request) {
      light->updateFirmware(reader);
      server->sendStatus(request, REQUEST_SUCCESS);
    }, [reader](AsyncWebServerRequest *request, uint8_t *data,
      size_t len,
      size_t index,
      size_t total
    ) {
      if(!index){
        reader->create(total);
      }
      reader->appendChunk(data, len, index);
    });
}
