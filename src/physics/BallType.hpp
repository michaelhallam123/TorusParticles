#pragma once

/**
 * Simple struct for storing information about each
 * type of ball.
 */

#include <array>
#include "Vec2.hpp"

struct BallType
{
	float                 radius; 
	float                 mass;
	std::size_t           count;         // Number of balls of this balltype
	std::array<float, 4>  rgba;          // RGBA color values for the balltype
	Vec2<float>           totalMomentum; // Total initial momentum of balls with this balltype
	bool                  wrapTexture;   // Whether to wrap ball textures across screen (not recommended for small balls)
	bool                  render;        // Whether to render balls of this balltype
};