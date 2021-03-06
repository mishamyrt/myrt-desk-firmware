// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#include "BacklightController.h"
#include <commands.h>

BacklightController::BacklightController(
  DapMaster *connection,
  AVRLord *board
) {
  _data = connection;
  _data->setController(this);
  _board = board;
  Store.append(&_descriptor);
}

void BacklightController::handle() {
  if (_firmware != NULL) {
    _board->flash(_firmware);
    _firmware->clear();
    _firmware = NULL;
    ESP.restart();
    return;
  }
  _data->handle();
}

void BacklightController::updateFirmware(FirmwareReader *firmware) {
  _firmware = firmware;
}

/// Handles Dap error
void BacklightController::onError() {
  Loggr.print("Got dap error");
  if (_connected) {
    _connected = false;
    connect();
  } else if (_connect_attempts > 0) {
    _connect_attempts--;
    _tryConnect();
  }
}

/// Handles Dap connection
void BacklightController::onConnect() {
  _connect_attempts = 0;
  _connected = true;
  Loggr.print("Dap connected!");
  _applyState();
}

void BacklightController::connect() {
  _connect_attempts = LIGHTSTRIP_CONNECT_ATTEMPTS;
  _tryConnect();
}

bool BacklightController::setColor(uint8_t r, uint8_t g, uint8_t b) {
  _state.color.r = r;
  _state.color.g = g;
  _state.color.b = b;
  _state.mode = MODE_RGB;
  _state.enabled = true;
  _descriptor.update();
  return _data->send(COMMAND_SET_COLOR, r, g, b);
}

bool BacklightController::setBrightness(uint8_t brightness) {
  _state.brightness = brightness;
  _descriptor.update();
  return _data->send(COMMAND_SET_BRIGHTNESS, brightness);
}

bool BacklightController::setTemperature(uint8_t temperature) {
  _state.temperature = temperature;
  _state.mode = MODE_TEMPERATURE;
  _state.enabled = true;
  _descriptor.update();
  return _data->send(COMMAND_SET_WHITE_TEMPERATURE, temperature);
}

bool BacklightController::powerOn() {
  _state.enabled = true;
  _descriptor.update();
  return _data->send(COMMAND_SET_BRIGHTNESS, _state.brightness)
    && _data->send(
      COMMAND_SET_COLOR,
      _state.color.r,
      _state.color.g,
      _state.color.b
    );
}

bool BacklightController::powerOff() {
  _state.enabled = false;
  _descriptor.update();
  return _data->send(COMMAND_SET_BRIGHTNESS, 0)
    && _data->send(COMMAND_SET_COLOR, 0, 0, 0);
}

bool BacklightController::setTransition(uint16_t transition) {
  return _data->send(
    COMMAND_SET_TRANSITION,
    highByte(transition),
    lowByte(transition)
  );
}

bool BacklightController::setEffect(uint8_t effect_code) {
  _state.effect = effect_code;
  _descriptor.update();
  return _data->send(COMMAND_SET_EFFECT, _state.effect);
}

void BacklightController::_tryConnect() {
  _board->reset();
  _data->connect();
}

void BacklightController::_applyState() {
  if (_state.enabled) {
    _data->send(COMMAND_SET_BRIGHTNESS, _state.brightness);
  } else {
    _data->send(COMMAND_SET_BRIGHTNESS, 0);
  }
  _data->send(COMMAND_SET_EFFECT, _state.effect);
  if (_state.mode == MODE_RGB) {
    _data->send(
      COMMAND_SET_COLOR,
      _state.color.r,
      _state.color.g,
      _state.color.b
    );
  } else if (_state.mode == MODE_TEMPERATURE) {
    _data->send(COMMAND_SET_WHITE_TEMPERATURE, _state.temperature);
  }
}
