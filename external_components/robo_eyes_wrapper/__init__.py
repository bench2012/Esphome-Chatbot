import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome import automation

robo_eyes_ns = cg.esphome_ns.namespace('robo_eyes')
RoboEyesComponent = robo_eyes_ns.class_('RoboEyesComponent', cg.Component)

# Define the Action class link
SetMoodAction = robo_eyes_ns.class_("SetMoodAction", automation.Action)
SetShapeAction = robo_eyes_ns.class_("SetShapeAction", automation.Action)
SetPositionAction = robo_eyes_ns.class_("SetPositionAction", automation.Action)
SetCuriosityAction = robo_eyes_ns.class_("SetCuriosityAction", automation.Action)
SetSweatAction = robo_eyes_ns.class_("SetSweatAction", automation.Action)
OpenAction = robo_eyes_ns.class_("OpenAction", automation.Action)
CloseAction = robo_eyes_ns.class_("CloseAction", automation.Action)
LaughAction = robo_eyes_ns.class_("LaughAction", automation.Action)
ConfusedAction = robo_eyes_ns.class_("ConfusedAction", automation.Action)
SetIdleModeAction = robo_eyes_ns.class_("SetIdleModeAction", automation.Action)
SetHFlickerAction = robo_eyes_ns.class_("SetHFlickerAction", automation.Action)
SetVFlickerAction = robo_eyes_ns.class_("SetVFlickerAction", automation.Action)
SetAutoblinkerAction = robo_eyes_ns.class_("SetAutoblinkerAction", automation.Action)
SetDisplayColorsAction = robo_eyes_ns.class_("SetDisplayColorsAction", automation.Action)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(RoboEyesComponent),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    # Use full URLs to ensure 2026 compatibility
    cg.add_library(
        name="FluxGarage_RoboEyes",
        repository="https://github.com/FluxGarage/RoboEyes.git",
        version=None
    )    
    # Correct Library Dependencies for 2026
    cg.add_library("Adafruit GFX Library", None)
    cg.add_library("Adafruit SH110X", None)
    cg.add_library("Adafruit BusIO", None) # Often required by SH110X
    cg.add_library("Wire", None)            # Ensure the built-in Wire library is linked

# Updated Schema for 2026.1.x compatibility
SET_MOOD_ACTION_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
    cv.Required("mood"): cv.templatable(cv.string),
})

SET_SHAPE_ACTION_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
    cv.Optional("width"): cv.templatable(cv.int_),
    cv.Optional("height"): cv.templatable(cv.int_),
    cv.Optional("radius"): cv.templatable(cv.int_),
    cv.Optional("space"): cv.templatable(cv.int_),
    cv.Optional("cyclops"): cv.templatable(cv.boolean),
})

# Rgister with the explicit schema for Set Shape
@automation.register_action("robo_eyes.set_shape", SetShapeAction, SET_SHAPE_ACTION_SCHEMA)
async def set_shape_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    # Create the action object with ONLY the parent and template_arg
    var = cg.new_Pvariable(action_id, template_arg, parent)
    
    # Use setters for each optional field to avoid the "config" object error
    if "width" in config:
        template_ = await cg.templatable(config["width"], args, cg.int_)
        cg.add(var.set_width(template_))
    if "height" in config:
        template_ = await cg.templatable(config["height"], args, cg.int_)
        cg.add(var.set_height(template_))
    if "radius" in config:
        template_ = await cg.templatable(config["radius"], args, cg.int_)
        cg.add(var.set_radius(template_))
    if "space" in config:
        template_ = await cg.templatable(config["space"], args, cg.int_)
        cg.add(var.set_space(template_))
    if "cyclops" in config:
        template_ = await cg.templatable(config["cyclops"], args, cg.bool_)
        cg.add(var.set_cyclops(template_))
        
    return var

# Re-register with the explicit schema for Set Mood
@automation.register_action("robo_eyes.set_mood", SetMoodAction, SET_MOOD_ACTION_SCHEMA)
async def set_mood_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    # Use await cg.templatable to handle !lambda 'return x;'
    mood_template = await cg.templatable(config["mood"], args, cg.std_string)
    return cg.new_Pvariable(action_id, template_arg, parent, mood_template)


# --- Register Position Action ---
@automation.register_action("robo_eyes.set_position", SetPositionAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("position"): cv.templatable(cv.string),
    }))
async def set_position_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    # Use await cg.templatable to handle !lambda 'return x;'
    pos_template = await cg.templatable(config["position"], args, cg.std_string)
    return cg.new_Pvariable(action_id, template_arg, parent, pos_template)

# --- Register Curiosity/Sweat Actions (Boolean) ---
@automation.register_action("robo_eyes.set_curiosity", SetCuriosityAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("state"): cv.templatable(cv.boolean),
    }))
    
async def set_curiosity_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, parent, config["state"])

@automation.register_action("robo_eyes.set_sweat", SetSweatAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("state"): cv.templatable(cv.boolean),
    }))
async def set_sweat_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, parent, config["state"])

# --- Register Open/Close Actions ---
@automation.register_action("robo_eyes.open", OpenAction, cv.Schema({cv.Required(CONF_ID): cv.use_id(RoboEyesComponent)}))
async def open_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, parent)

@automation.register_action("robo_eyes.close", CloseAction, cv.Schema({cv.Required(CONF_ID): cv.use_id(RoboEyesComponent)}))
async def close_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, parent)

@automation.register_action("robo_eyes.laugh", LaughAction, cv.Schema({cv.Required(CONF_ID): cv.use_id(RoboEyesComponent)}))
async def laugh_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, parent)

# --- Register Confused Action ---
@automation.register_action("robo_eyes.confused", ConfusedAction, cv.Schema({cv.Required(CONF_ID): cv.use_id(RoboEyesComponent)}))
async def confused_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, parent)

# --- Register Idle Mode Action ---
@automation.register_action("robo_eyes.set_idle_mode", SetIdleModeAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("state"): cv.templatable(cv.boolean),
    }))
async def set_idle_mode_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    template_ = await cg.templatable(config["state"], args, cg.bool_)
    return cg.new_Pvariable(action_id, template_arg, parent, template_)

# --- Register HFlicker Action ---
@automation.register_action("robo_eyes.set_hflicker", SetHFlickerAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("state"): cv.templatable(cv.boolean),
        cv.Required("amplitude"): cv.templatable(cv.int_range(min=0, max=255)),
    }))
async def set_hflicker_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    template_state = await cg.templatable(config["state"], args, cg.bool_)
    cg.add(var.set_state(template_state))
    template_amp = await cg.templatable(config["amplitude"], args, cg.uint8)
    cg.add(var.set_amplitude(template_amp))
    return var

# --- Register VFlicker Action ---
@automation.register_action("robo_eyes.set_vflicker", SetVFlickerAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("state"): cv.templatable(cv.boolean),
        cv.Required("amplitude"): cv.templatable(cv.int_range(min=0, max=255)),
    }))
async def set_vflicker_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    template_state = await cg.templatable(config["state"], args, cg.bool_)
    cg.add(var.set_state(template_state))
    template_amp = await cg.templatable(config["amplitude"], args, cg.uint8)
    cg.add(var.set_amplitude(template_amp))
    return var

# --- Register Autoblinker Action ---
@automation.register_action("robo_eyes.set_autoblinker", SetAutoblinkerAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("state"): cv.templatable(cv.boolean),
        cv.Optional("interval"): cv.templatable(cv.int_),
        cv.Optional("variation"): cv.templatable(cv.int_),
    }))
async def set_autoblinker_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    
    # Set state (bool)
    template_state = await cg.templatable(config["state"], args, cg.bool_)
    cg.add(var.set_state(template_state))
    
    # Set interval (int, default 4 if not provided)
    interval = config.get("interval", 4)
    template_interval = await cg.templatable(interval, args, cg.int_)
    cg.add(var.set_interval(template_interval))
    
    # Set variation (int, default 2 if not provided)
    variation = config.get("variation", 2)
    template_variation = await cg.templatable(variation, args, cg.int_)
    cg.add(var.set_variation(template_variation))
    
    return var
    
@automation.register_action("robo_eyes.set_display_colors", SetDisplayColorsAction, 
    cv.Schema({
        cv.Required(CONF_ID): cv.use_id(RoboEyesComponent),
        cv.Required("background"): cv.templatable(cv.int_range(min=0, max=255)),
        cv.Required("main"): cv.templatable(cv.int_range(min=0, max=255)),
    }))
async def set_display_colors_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    
    # Process background color
    template_bg = await cg.templatable(config["background"], args, cg.uint8)
    cg.add(var.set_background(template_bg))
    
    # Process main/foreground color
    template_main = await cg.templatable(config["main"], args, cg.uint8)
    cg.add(var.set_main(template_main))
    
    return var
 

