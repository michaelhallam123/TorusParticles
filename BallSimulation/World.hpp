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
	float xMid;

	float yMin;
	float yMax;
	float yWidth;
	float yMid;

	World(float xMin, float xMax, float yMin, float yMax)
		: xMin(xMin), xMax(xMax), xWidth(xMax - xMin), xMid((xMax + xMin)/2.0f),
		  yMin(yMin), yMax(yMax), yWidth(yMax - yMin), yMid((yMax + yMin)/2.0f) {}
};