#pragma once
#include <array>
#include "vec2.hpp"

struct balltype
{
	float                 radius;        // Radius of each ball of this balltype
	float                 mass;          // Mass of each ball of this balltype
	unsigned int          count;         // Number of balls of this balltype
	std::array<float, 4>  rgba;          // RGBA color values for the balltype
	vec2<float>           totalMomentum; // Total initial momentum of balls with this balltype
	bool                  wrapTexture;   // Whether to wrap ball textures across screen (not recommended for small balls)
	bool                  render;        // Whether to render balls of this balltype
};

struct
{
	bool operator()(const balltype& bt1, const balltype& bt2) const { return bt1.radius > bt2.radius; }
} sortByRadiusDecreasing;