#pragma once

/**
 * A function to read in BallType information from a .json file and
 * return it as a std::vector<BallType>. This vector can then be
 * passed to the constructor of a Solver object to specify ball types.
 * 
 * Uses jsoncpp (https://github.com/open-source-parsers/jsoncpp) to
 * parse the .json file.
 *
 * Example preset files are located in the project resources
 * folder.
 */

#include "BallType.hpp"
#include <vector>
#include <string>

std::vector<BallType> loadPreset(const std::string& filepath);