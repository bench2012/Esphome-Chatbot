#pragma once

#include "esphome.h"
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>

namespace esphome {
namespace robo_eyes {

class RoboEyesComponent;

// --- COMPONENT DEFINITION ---
class RoboEyesComponent : public Component {
 public:
  Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);
  RoboEyes<Adafruit_SH1106G> roboEyes;
  bool ready_ = false; // Guard flag to prevent boot crashes

  RoboEyesComponent() : roboEyes(display) {}

  void setup() override {
    Wire.setPins(41, 42); 
    if(!display.begin(0x3C, false)) {
        ESP_LOGE("robo_eyes", "Display init failed!");
    }
    roboEyes.begin(128, 64, 30);
    roboEyes.setAutoblinker(true);
    roboEyes.setIdleMode(false);
    roboEyes.setMood(DEFAULT);
    
    this->ready_ = true; // Mark as ready for actions
    ESP_LOGI("robo_eyes", "RoboEyes Initialization Complete");
  }

  void loop() override {
    roboEyes.update();
  }

  void set_mood(std::string mood) {
    if (!ready_) return;
    if (mood == "HAPPY") roboEyes.setMood(HAPPY);
    else if (mood == "ANGRY") roboEyes.setMood(ANGRY);
    else if (mood == "TIRED") roboEyes.setMood(TIRED);
    else roboEyes.setMood(DEFAULT);
  }

  void set_shape(int w, int h, int r, int s, bool cyc) {
    if (!ready_) return;
    if (w > 0) roboEyes.setWidth(w, w);
    if (h > 0) roboEyes.setHeight(h, h);
    if (r >= 0) roboEyes.setBorderradius(r, r);
    if (s != -999) roboEyes.setSpacebetween(s);
    roboEyes.setCyclops(cyc);
  }
};

// --- ACTION CLASSES WITH BOOT GUARDS ---

template<typename... Ts> class SetMoodAction : public Action<Ts...> {
 public:
  SetMoodAction(RoboEyesComponent *parent, TemplatableValue<std::string, Ts...> mood) : parent_(parent), mood_(mood) {}
  void play(const Ts &...x) override { if(this->parent_->ready_) this->parent_->set_mood(this->mood_.value(x...)); }
 protected:
  RoboEyesComponent *parent_;
  TemplatableValue<std::string, Ts...> mood_;
};

template<typename... Ts> class SetPositionAction : public Action<Ts...> {
 public:
  SetPositionAction(RoboEyesComponent *parent, TemplatableValue<std::string, Ts...> pos) : parent_(parent), pos_(pos) {}
  void play(const Ts &...x) override {
    if(!this->parent_->ready_) return;
    std::string p = this->pos_.value(x...);
    if (p == "N") this->parent_->roboEyes.setPosition(N);
    else if (p == "S") this->parent_->roboEyes.setPosition(S);
    else if (p == "E") this->parent_->roboEyes.setPosition(E);
    else if (p == "W") this->parent_->roboEyes.setPosition(W);
    else if (p == "NE") this->parent_->roboEyes.setPosition(NE);
    else if (p == "NW") this->parent_->roboEyes.setPosition(NW);
    else if (p == "SE") this->parent_->roboEyes.setPosition(SE);
    else if (p == "SW") this->parent_->roboEyes.setPosition(SW);
    else this->parent_->roboEyes.setPosition(DEFAULT);
  }
 protected:
  RoboEyesComponent *parent_;
  TemplatableValue<std::string, Ts...> pos_;
};

template<typename... Ts> class SetCuriosityAction : public Action<Ts...> {
 public:
  SetCuriosityAction(RoboEyesComponent *parent, TemplatableValue<bool, Ts...> state) : parent_(parent), state_(state) {}
  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    bool val = this->state_.value(x...);
    this->parent_->roboEyes.setCuriosity(val);
    if (!val) {
        this->parent_->roboEyes.setIdleMode(false);
        this->parent_->roboEyes.update();
        // Return to auto-idle after a frame
    }
  }
 protected:
  RoboEyesComponent *parent_;
  TemplatableValue<bool, Ts...> state_;
};

template<typename... Ts> class SetSweatAction : public Action<Ts...> {
 public:
  SetSweatAction(RoboEyesComponent *parent, TemplatableValue<bool, Ts...> state) : parent_(parent), state_(state) {}
  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    bool val = this->state_.value(x...);
    this->parent_->roboEyes.setSweat(val);
    if (!val) {
        this->parent_->roboEyes.setIdleMode(false);
        this->parent_->roboEyes.update();
    }
  }
 protected:
  RoboEyesComponent *parent_;
  TemplatableValue<bool, Ts...> state_;
};

template<typename... Ts> class SetShapeAction : public Action<Ts...> {
 public:
  SetShapeAction(RoboEyesComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(int, width)
  TEMPLATABLE_VALUE(int, height)
  TEMPLATABLE_VALUE(int, radius)
  TEMPLATABLE_VALUE(int, space)
  TEMPLATABLE_VALUE(bool, cyclops)
  void play(const Ts &...x) override {
    if(!this->parent_->ready_) return;
    this->parent_->set_shape(this->width_.value_or(x..., -1), this->height_.value_or(x..., -1),
                             this->radius_.value_or(x..., -1), this->space_.value_or(x..., -999),
                             this->cyclops_.value_or(x..., false));
  }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class OpenAction : public Action<Ts...> {
 public:
  OpenAction(RoboEyesComponent *parent) : parent_(parent) {}
  void play(const Ts &...x) override { if(this->parent_->ready_) this->parent_->roboEyes.open(); }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class CloseAction : public Action<Ts...> {
 public:
  CloseAction(RoboEyesComponent *parent) : parent_(parent) {}
  void play(const Ts &...x) override { if(this->parent_->ready_) this->parent_->roboEyes.close(); }
 protected:
  RoboEyesComponent *parent_;
};

// NEW ACTION CLASSES

template<typename... Ts> class LaughAction : public Action<Ts...> {
 public:
  LaughAction(RoboEyesComponent *parent) : parent_(parent) {}
  void play(const Ts &...x) override { 
    if(this->parent_->ready_) this->parent_->roboEyes.anim_laugh(); 
  }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class ConfusedAction : public Action<Ts...> {
 public:
  ConfusedAction(RoboEyesComponent *parent) : parent_(parent) {}
  void play(const Ts &...x) override { 
    if(this->parent_->ready_) this->parent_->roboEyes.anim_confused(); 
  }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class SetIdleModeAction : public Action<Ts...> {
 public:
  SetIdleModeAction(RoboEyesComponent *parent, TemplatableValue<bool, Ts...> state) : parent_(parent), state_(state) {}
  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    bool val = this->state_.value(x...);
    this->parent_->roboEyes.setIdleMode(val);
    ESP_LOGD("robo_eyes", "Idle Mode set to: %s", val ? "ON" : "OFF");
  }
 protected:
  RoboEyesComponent *parent_;
  TemplatableValue<bool, Ts...> state_;
};

template<typename... Ts> class SetHFlickerAction : public Action<Ts...> {
 public:
  SetHFlickerAction(RoboEyesComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(bool, state)
  TEMPLATABLE_VALUE(uint8_t, amplitude)

  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    bool s = this->state_.value(x...);
    uint8_t a = s ? this->amplitude_.value(x...) : 0; // Force 0 if false
    
    this->parent_->roboEyes.setHFlicker(s, a);
    
    // 2026 Fix: If turning off, force a redraw to center the eyes
    if (!s) {
        this->parent_->roboEyes.update();
    }
  }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class SetVFlickerAction : public Action<Ts...> {
 public:
  SetVFlickerAction(RoboEyesComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(bool, state)
  TEMPLATABLE_VALUE(uint8_t, amplitude)

  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    bool s = this->state_.value(x...);
    uint8_t a = s ? this->amplitude_.value(x...) : 0; // Force 0 if false

    this->parent_->roboEyes.setVFlicker(s, a);
    
    // 2026 Fix: Force redraw to reset vertical alignment
    if (!s) {
        this->parent_->roboEyes.update();
    }
  }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class SetAutoblinkerAction : public Action<Ts...> {
 public:
  SetAutoblinkerAction(RoboEyesComponent *parent) : parent_(parent) {}
  
  TEMPLATABLE_VALUE(bool, state)
  TEMPLATABLE_VALUE(int, interval)
  TEMPLATABLE_VALUE(int, variation)

  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    
    // Resolve values from YAML or Lambdas
    bool s = this->state_.value(x...);
    int i = this->interval_.value(x...);
    int v = this->variation_.value(x...);

    this->parent_->roboEyes.setAutoblinker(s, i, v);
    ESP_LOGD("robo_eyes", "Autoblinker set to: %s (Interval: %ds, Var: %ds)", s ? "ON" : "OFF", i, v);
  }
 protected:
  RoboEyesComponent *parent_;
};

template<typename... Ts> class SetDisplayColorsAction : public Action<Ts...> {
 public:
  SetDisplayColorsAction(RoboEyesComponent *parent) : parent_(parent) {}
  
  TEMPLATABLE_VALUE(uint8_t, background)
  TEMPLATABLE_VALUE(uint8_t, main)

  void play(const Ts &...x) override { 
    if(!this->parent_->ready_) return;
    
    uint8_t bg = this->background_.value(x...);
    uint8_t mn = this->main_.value(x...);

    // FIX: If background is 1 (white) and main is 0 (black),
    // we use the hardware inversion to clear the "eyebrow" artifact.
    if (bg == 1 && mn == 0) {
        this->parent_->display.invertDisplay(true);
    } else {
        this->parent_->display.invertDisplay(false);
    }

    // Also update the library colors for future draw calls
    this->parent_->roboEyes.setDisplayColors(bg, mn);
    
    // Force a full redraw
    this->parent_->roboEyes.update();
  }
 protected:
  RoboEyesComponent *parent_;
};


// Add new action class here

} // namespace robo_eyes
} // namespace esphome

