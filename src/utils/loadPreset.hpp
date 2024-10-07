#pragma once

/**
 * A function to read in information from a .json file and
 * return it as a "Preset". The loaded preset is then passed
 * to the solver and renderer upon construction.
 * 
 * Uses jsoncpp (https://github.com/open-source-parsers/jsoncpp) 
 * to parse the .json file.
 *
 * Example preset files are located in the project presets folder
 */

#include "Preset.hpp"
#include <vector>
#include <string>

Preset loadPreset(const std::string& filepath);