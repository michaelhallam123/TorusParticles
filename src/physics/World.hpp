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

	World(float xWidth, float yWidth)
		: xMin(-xWidth/2.0f), xMax(xWidth/2.0f), xWidth(xWidth), xMid(0.0f),
		  yMin(-yWidth/2.0f), yMax(yWidth/2.0f), yWidth(yWidth), yMid(0.0f) {}

	World(float aspectRatio)
		: xMin(-aspectRatio), xMax(aspectRatio), xWidth(2.0f*aspectRatio), xMid(0.0f),
		  yMin(       -1.0f), yMax(       1.0f), yWidth(            2.0f), yMid(0.0f) {}
};