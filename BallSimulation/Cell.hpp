#pragma once

#include <array>

struct Cell
{
	std::array<std::size_t, 20> ballIDs;  // Assume no more than ten balls per cell
	std::size_t                 numBalls; // Number of balls contained in the cell

	void addBall(std::size_t ballID)
	{
		if (numBalls < ballIDs.size())
			ballIDs[numBalls++] = ballID;
	}

	void clear()
	{
		numBalls = 0;
	}

};