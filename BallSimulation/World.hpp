#pragma once

/**
 * Simple struct to store data of the world space,
 * namely boundaries and width.
 */

struct World
{
	float xMin;
	float xMax;
	float xWidth;

	float yMin;
	float yMax;
	float yWidth;

	World(float xMin, float xMax, float yMin, float yMax)
		: xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), xWidth(xMax - xMin), yWidth(yMax - yMin) {}
};