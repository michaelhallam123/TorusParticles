#pragma once

/**
 * A simple struct for storing data used in the 
 * sweep and prune algorithm
 */

struct Endpoints
{
	float       left;  // Leftmost endpoint of the ball
	float       right; // Rightmost endpoint of the ball
	std::size_t ind;   // Index of the ball in m_balls
};

