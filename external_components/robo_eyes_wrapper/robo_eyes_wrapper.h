#pragma once

#include "esphome.h"
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>

namespace esphome {
namespace robo_eyes {

// Forward declaration
class RoboEyesComponent;

// C++ Helper for the Action
template<typename... Ts> class SetMoodAction : public Action<Ts...> {
 public:
  // Constructor stores the component and the requested mood
  SetMoodAction(RoboEyesComponent *parent, std::string mood) : parent_(parent), mood_(mood) {}
  void play(Ts... x) override; // We will define this at the bottom
  
 protected:
  RoboEyesComponent *parent_;
  std::string mood_;
};

class RoboEyesComponent : public Component {
 public:
  Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);
  RoboEyes<Adafruit_SH1106G> roboEyes;

  RoboEyesComponent() : roboEyes(display) {}


  void setup() override {
    // 1. Manually assign your GPIOs to the global Wire object
    // This tells the library exactly which pins to use
    Wire.setPins(41, 42); // SDA: 41, SCL: 42

    // 2. Initialize the display logic.
    // Use 'false' for the second parameter to tell the library
    // NOT to call Wire.begin() internally, preventing the "already acquired" crash.
    if(!display.begin(0x3C, false)) { 
        ESP_LOGE("custom", "SH1106 initialization failed");
    }

    roboEyes.begin(128, 64, 30);
    roboEyes.setAutoblinker(true);
    // Using the macro DEFAULT directly as defined in the library
    roboEyes.setMood(DEFAULT);
  }

  void loop() override {
    roboEyes.update();
  }
// Pruned to only use valid library macros: DEFAULT, TIRED, ANGRY, HAPPY
  void set_mood(std::string mood) {
    if (mood == "HAPPY") {
        roboEyes.setMood(HAPPY);
    } else if (mood == "ANGRY") {
        roboEyes.setMood(ANGRY);
    } else if (mood == "TIRED") {
        roboEyes.setMood(TIRED);
    } else {
        roboEyes.setMood(DEFAULT);
    }
    
    ESP_LOGD("robo_eyes", "Mood changed to: %s", mood.c_str());
  }
};

template<typename... Ts> 
void SetMoodAction<Ts...>::play(Ts... x) {
  this->parent_->set_mood(this->mood_);
}


}  // namespace robo_eyes
}   // namespace robo_eyes

