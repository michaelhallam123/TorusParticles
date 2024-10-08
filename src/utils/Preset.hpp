#pragma once

#include <vector>

#include "BallType.hpp"

/*
 * Simple struct for configuring settings for the 
 * solver and renderer.
 */

struct Preset
{
    float dt;
    float worldAspectRatio;
    bool antialiasing;
    std::vector<BallType> ballTypes;

    bool loadSuccessful = false;
};