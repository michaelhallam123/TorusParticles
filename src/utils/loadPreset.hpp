#pragma once

/**
 * A function to read in information from a .json file and
 * return it as a "Preset" object. The loaded preset is then 
 * passed to the solver and renderer upon construction.
 * 
 * If a preset fails to load, an error message is printed. The
 * function then returns a preset with "loadSuccessful" set to
 * "false".
 * 
 * Uses jsoncpp (https://github.com/open-source-parsers/jsoncpp) 
 * to parse the .json file.
 * 
 * WARNING: Incorrect json syntax may cause jsoncpp to fail 
 * without an error message.
 *
 * Example preset files are located in the project presets 
 * folder.
 */

#include "Preset.hpp"
#include <vector>
#include <string>

Preset loadPreset(const std::string& filepath);