#pragma once

/**
 * Simple struct for storing ball information.
 * 
 * Intrinsic ball information such as mass, 
 * radius, color, etc., are stored in the ball's
 * BallType data in m_ballTypes. The corresponding
 * index for this data is stored in Ball::typeindex.
 */

#include "Vec2.hpp"

struct Ball
{
	Vec2<float> position;
	Vec2<float> velocity;
	std::size_t typeindex; // Index of the ball's type in Solver::m_ballTypes
};