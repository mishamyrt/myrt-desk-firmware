#include "Fill.h"

bool FillEffect::handle(EffectStateController *host, Strip *strip) {
  if (_transitioned) {
    return false;
  }
  _progress.tick();
  CRGB color = blend_colors(
    host->color(),
    host->target_color(),
    _progress.value()
  );
  strip->set(color);
  if (_progress.is_done()) {
    host->onColorUpdated();
    _transitioned = true;
  }
  return true;
}

void FillEffect::finalize(EffectStateController *controller) {
  controller->onEffectFinished();
}

void FillEffect::onColorUpdate(EffectStateController *controller) {
  _progress.setDuration(
    controller->transition_time()
  );
  _transitioned = false;
}

void FillEffect::prepare(EffectStateController *controller) {
  onColorUpdate(controller);
}

FillEffect Fill;