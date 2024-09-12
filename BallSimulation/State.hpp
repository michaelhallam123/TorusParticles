#pragma once

#include <vector>

#include "balltype.hpp"
#include "ball.hpp"

class State
{
public:
	State(std::vector<balltype> ballTypes);

	std::vector<ball> balls;             // Store position and velocity and type of each ball
	const std::vector<balltype> ballTypes;     // Store each balltype (mass, radius, color, etc.)

	inline float getRadius(unsigned int i) const
	{
		return ballTypes[balls[i].typeindex].radius;
	}
	inline float getMass(unsigned int i) const
	{
		return ballTypes[balls[i].typeindex].mass;
	}
};