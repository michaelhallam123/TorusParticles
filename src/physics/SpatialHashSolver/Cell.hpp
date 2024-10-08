#pragma once

#include <array>

enum Offset {
	NONE, LEFT, RIGHT, DOWN, DOWN_LEFT, DOWN_RIGHT, UP, UP_LEFT, UP_RIGHT
};

struct BallInfo
{
	std::size_t ballID;
	Offset offset;      // Offset from the balls original position that places it in a cell
};

struct Cell
{
	std::array<BallInfo, 20> ballList;  // Assume no more than ten balls per cell
	std::size_t              numBalls; // Number of balls contained in the cell

	void addBall(BallInfo info)
	{
		if (numBalls < ballList.size())
			ballList[numBalls++] = info;
	}

	void clear()
	{
		numBalls = 0;
	}

};