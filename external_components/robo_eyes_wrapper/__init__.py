import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome import automation

robo_eyes_ns = cg.esphome_ns.namespace('robo_eyes')
RoboEyesComponent = robo_eyes_ns.class_('RoboEyesComponent', cg.Component)

# Define the Action class link
SetMoodAction = robo_eyes_ns.class_("SetMoodAction", automation.Action)
SetShapeAction = robo_eyes_ns.class_("SetShapeAction", automation.Action)

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

# Re-register with the explicit schema
@automation.register_action("robo_eyes.set_mood", SetMoodAction, SET_MOOD_ACTION_SCHEMA)
async def set_mood_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    # Ensure we pass the mood value correctly from the config dictionary
    return cg.new_Pvariable(action_id, template_arg, parent, config["mood"])


