#pragma once

/**
 * Simple struct to store data of the world space,
 * namely boundaries and width.
 */

#include <cmath>

struct World
{
	float xMin;
	float xMax;
	float xWidth;
	float xMid;

	float yMin;
	float yMax;
	float yWidth;
	float yMid;

	World(float aspectRatio)
		: xMin(     -std::sqrt(aspectRatio)), xMax(     std::sqrt(aspectRatio)), xWidth(2.0f*std::sqrt(aspectRatio)), xMid(0.0f),
		  yMin(-1.0f/std::sqrt(aspectRatio)), yMax(1.0f/std::sqrt(aspectRatio)), yWidth(2.0f/std::sqrt(aspectRatio)), yMid(0.0f) {}
};