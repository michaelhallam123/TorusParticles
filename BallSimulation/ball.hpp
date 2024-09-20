#pragma once

/**
 * Simple struct for storing ball information
 */

#include "Vec2.hpp"

struct Ball
{
	Vec2<float> position;
	Vec2<float> velocity;
	unsigned int typeindex; // Index of the ball's type in Solver::m_ballTypes
};