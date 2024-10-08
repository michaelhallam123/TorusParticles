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

	World(float aspectRatio)
		: xMin(-aspectRatio), xMax(aspectRatio), xWidth(2.0f*aspectRatio), xMid(0.0f),
		  yMin(       -1.0f), yMax(       1.0f), yWidth(            2.0f), yMid(0.0f) {}
};