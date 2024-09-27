#include "SpatialHashSolver.hpp"

#include <cmath>
#include <iostream>

SpatialHashSolver::SpatialHashSolver(std::vector<BallType> ballTypes)
	: Solver(ballTypes) 
{
	std::size_t numRows = 1 + static_cast<std::size_t>(std::sqrt(static_cast<double>(m_balls.size())));

	m_numRows   = numRows;
	m_cellWidth = (m_world.xMax - m_world.xMin) / static_cast<float>(m_numRows);

	m_grid.resize(numRows * numRows); // Number of cells is order m_balls.size()
}

void SpatialHashSolver::populateCells()
{
	for (std::size_t i = 0; i < m_balls.size(); ++i)
	{
		const Ball& b = m_balls[i];

		const float& r = m_ballTypes[b.typeindex].radius;

		float xLeft  = b.position.x - r;
		float xRight = b.position.x + r;
		float yLow   = b.position.y - r;
		float yHigh  = b.position.y + r;

		if (xLeft < m_world.xMin)
		{
			xLeft  += m_world.xWidth;
			xRight += m_world.xWidth;
		}
		if (yLow < m_world.yMin)
		{
			yLow  += m_world.yWidth;
			yHigh += m_world.yWidth;
		}

		for (std::size_t r = posToCell(yLow); r <= posToCell(yHigh); ++r)
		{
			for (std::size_t c = posToCell(xLeft); c <= posToCell(xRight); ++c)
			{
				m_grid[hashCell(r, c)].addBall(i);
			}
		}
	}
}

void SpatialHashSolver::solve()
{
	clearCells();

	populateCells();

	checkCollisions();
}

void SpatialHashSolver::checkCollisions()
/**
 * Iterate through each cell checking for collisions
 */
{
	for (std::size_t r = 0; r < m_numRows; ++r)
	{
		for (std::size_t c = 0; c < m_numRows; ++c)
		{
			findCollisionsInCell(m_grid[hashCell(r, c)]);
		}
	}
}

void SpatialHashSolver::findCollisionsInCell(Cell& c1)
{
	static const std::vector<Vec2<float>> translates = 
	{ { -m_world.xWidth, -m_world.yWidth }, {  0.0f, -m_world.yWidth }, {  m_world.xWidth, -m_world.yWidth },
	  { -m_world.xWidth,  0.0f           }, {  0.0f,  0.0f           }, {  m_world.xWidth,  0.0f           },
	  { -m_world.xWidth,  m_world.yWidth }, {  0.0f,  m_world.yWidth }, {  m_world.xWidth,  m_world.yWidth } };

	for (std::size_t i1 = 0; i1 < c1.numBalls; ++i1)
	{
		for (std::size_t i2 = i1+1; i2 < c1.numBalls; ++i2)
		{
			for (Vec2<float> translate : translates)
			{
				m_balls[c1.ballIDs[i1]].position += translate;
				if (overlap(c1.ballIDs[i1], c1.ballIDs[i2]))
					resolveCollision(c1.ballIDs[i1], c1.ballIDs[i2]);
				m_balls[c1.ballIDs[i1]].position -= translate;
			}
		}
	}
}

void SpatialHashSolver::clearCells()
{
	for (Cell& c : m_grid)
		c.clear();
}

std::size_t SpatialHashSolver::hashCell(std::size_t row, std::size_t col)
{
	return (row % m_numRows) * m_numRows + (col % m_numRows);
}

std::size_t SpatialHashSolver::posToCell(float x)
{
	std::size_t i = static_cast<std::size_t>(((x - m_world.xMin) / m_world.xWidth) * static_cast<float>(m_numRows));

	return i;
}