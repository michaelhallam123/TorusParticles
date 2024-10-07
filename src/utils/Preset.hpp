#pragma once

#include <vector>

#include "BallType.hpp"

struct Preset
{
    float dt;
    float worldAspectRatio;
    bool antialiasing;
    std::vector<BallType> ballTypes;
};