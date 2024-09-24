#pragma once

#include <array>

struct Cell
{
	std::array<std::size_t, 20> m_ballIDs; // Assume no more than ten balls per cell
	std::size_t numBalls;                  // Number of balls contained in the cell

	void addBall(std::size_t ballID)
	{
		if (numBalls < m_ballIDs.size())
			m_ballIDs[numBalls++] = ballID;
	}

	void clear()
	{
		numBalls = 0;
	}

};