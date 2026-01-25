#pragma once

#include "esphome.h"
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>

namespace esphome {
namespace robo_eyes {

// 1. FORWARD DECLARATIONS
class RoboEyesComponent;

// 2. MOOD ACTION CLASS
template<typename... Ts> class SetMoodAction : public Action<Ts...> {
 public:
  SetMoodAction(RoboEyesComponent *parent, std::string mood) : parent_(parent), mood_(mood) {}
  void play(Ts... x) override;
 protected:
  RoboEyesComponent *parent_;
  std::string mood_;
};

// 3. SHAPE ACTION CLASS
template<typename... Ts> class SetShapeAction : public Action<Ts...> {
 public:
  SetShapeAction(RoboEyesComponent *parent) : parent_(parent) {}
  
  // Setter methods for Python's cg.add(var.set_...) calls
  TEMPLATABLE_VALUE(int, width)
  TEMPLATABLE_VALUE(int, height)
  TEMPLATABLE_VALUE(int, radius)
  TEMPLATABLE_VALUE(int, space)
  TEMPLATABLE_VALUE(bool, cyclops)

  void play(Ts... x) override {
    // We pass -1 or -999 to indicate the value was NOT provided in YAML
    this->parent_->set_shape(
      this->width_.value_or(x..., -1),
      this->height_.value_or(x..., -1),
      this->radius_.value_or(x..., -1),
      this->space_.value_or(x..., -999),
      this->cyclops_.value_or(x..., false)
    );
  }

 protected:
  RoboEyesComponent *parent_;
};
// 4. MAIN COMPONENT
class RoboEyesComponent : public Component {
 public:
  Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);
  RoboEyes<Adafruit_SH1106G> roboEyes;

  RoboEyesComponent() : roboEyes(display) {}

  void setup() override {
    Wire.setPins(41, 42); 
    if(!display.begin(0x3C, false)) {
        ESP_LOGE("robo_eyes", "SH1106 Display init failed!");
    }
    roboEyes.begin(128, 64, 30);
    roboEyes.setAutoblinker(true);
    roboEyes.setMood(DEFAULT);
  }

  void loop() override {
    roboEyes.update();
  }

  void set_mood(std::string mood) {
    if (mood == "HAPPY") roboEyes.setMood(HAPPY);
    else if (mood == "ANGRY") roboEyes.setMood(ANGRY);
    else if (mood == "TIRED") roboEyes.setMood(TIRED);
    else roboEyes.setMood(DEFAULT);
    ESP_LOGD("robo_eyes", "Mood set to: %s", mood.c_str());
  }

  void set_shape(int w, int h, int r, int s, bool cyc) {
    if (w > 0) roboEyes.setWidth(w, w);
    if (h > 0) roboEyes.setHeight(h, h);
    if (r >= 0) roboEyes.setBorderradius(r, r);
    if (s != -999) roboEyes.setSpacebetween(s);
    roboEyes.setCyclops(cyc);
    ESP_LOGD("robo_eyes", "Shape updated: W:%d H:%d R:%d S:%d C:%d", w, h, r, s, cyc);
  }
};

// 5. ACTION IMPLEMENTATIONS
template<typename... Ts> 
void SetMoodAction<Ts...>::play(Ts... x) {
  this->parent_->set_mood(this->mood_);
}



} // namespace robo_eyes
} // namespace esphome

