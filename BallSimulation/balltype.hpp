#pragma once
#include <array>
#include "vec2.h"

struct balltype
{
	float radius;
	float mass;
	std::array<float, 4> rgba; // RGBA color values for the balltype
	unsigned int count;        // Number of balls with these properties
	vec2<float> totalMomentum; // Total momentum of the balls with these properties
	bool wrapTexture = false;
};