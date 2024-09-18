#pragma once
#include <array>
#include "vec2.h"

struct balltype
{
	float radius;
	float mass;
	unsigned int count;        // Number of balls with these properties
	std::array<float, 4> rgba; // RGBA color values for the balltype
	vec2<float> totalMomentum; // Total momentum of the balls with these properties
	bool wrapTexture = false;
	bool render = true;
};

struct
{
	bool operator()(const balltype& bt1, const balltype& bt2) const { return bt1.radius > bt2.radius; }
} sortByRadiusDecreasing;