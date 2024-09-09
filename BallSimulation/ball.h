#pragma once
#include <array>
#include "vec2.h"

struct ball
{
	float radius;
	float mass;
	std::array<float, 4> rgba; // RGBA color values for the type of ball
	unsigned int count; // Number of balls with these properties to appear in the simulation
	vec2<float> totalMomentum; // Total momentum of the balls with these properties in the simulation
};