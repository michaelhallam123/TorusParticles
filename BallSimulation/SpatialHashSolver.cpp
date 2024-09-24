#include "SpatialHashSolver.hpp"

#include <cmath>
#include <iostream>

SpatialHashSolver::SpatialHashSolver(std::vector<BallType> ballTypes)
	: Solver(ballTypes) 
{
	std::size_t numRows = 1 + (std::size_t)std::sqrt((double)m_balls.size());
	m_numRows = numRows;
	m_cellWidth = (m_world.xMax - m_world.xMin) / (float)m_numRows;

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

		for (std::size_t r = posToCell(xLeft); r <= posToCell(xRight); ++r)
		{
			for (std::size_t c = posToCell(yLow); c <= posToCell(yHigh); ++c)
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
 * Iterate through each cell checking its primitive neighbours
 */
{
	static const Vec2<float> upTranslate(0.0f, m_world.yWidth);
	static const Vec2<float> rightTranslate(m_world.xWidth, 0.0f);

	for (std::size_t r = 1; r < m_numRows - 1; ++r)
	{
		for (std::size_t c = 0; c < m_numRows - 1; ++c)
		{
			checkPrimitiveNeighbors(r, c);
		}
	}

	std::size_t r = 0;

	// TODO: Fix this by writing an iterator for Cell object!

	for (std::size_t c = 0; c < m_numRows; ++c)
	{
		for (std::size_t id : m_grid[hashCell(m_numRows - 1, c)].m_ballIDs)
		{
			m_balls[id].position -= upTranslate;
		}
	}
	for (std::size_t c = 0; c < m_numRows; ++c)
	{
		checkPrimitiveNeighbors(r, c);
	}
	for (std::size_t c = 0; c < m_numRows; ++c)
	{
		for (std::size_t id : m_grid[hashCell(m_numRows - 1, c)].m_ballIDs)
		{
			m_balls[id].position += upTranslate;
		}
	}

	r = m_numRows - 1;

	for (std::size_t c = 0; c < m_numRows; ++c)
	{
		for (std::size_t id : m_grid[hashCell(0, c)].m_ballIDs)
		{
			m_balls[id].position += upTranslate;
		}
	}
	for (std::size_t c = 0; c < m_numRows; ++c)
	{
		checkPrimitiveNeighbors(r, c);
	}
	for (std::size_t c = 0; c < m_numRows; ++c)
	{
		for (std::size_t id : m_grid[hashCell(0, c)].m_ballIDs)
		{
			m_balls[id].position -= upTranslate;
		}
	}

}

void SpatialHashSolver::checkPrimitiveNeighbors(std::size_t r, std::size_t c)
{
	compareCells(m_grid[hashCell(r, c)], m_grid[hashCell(r, c)]);
	compareCells(m_grid[hashCell(r, c)], m_grid[hashCell(r + 1, c)]);
	compareCells(m_grid[hashCell(r, c)], m_grid[hashCell(r + 1, c + 1)]);
	compareCells(m_grid[hashCell(r, c)], m_grid[hashCell(r, c + 1)]);
	compareCells(m_grid[hashCell(r, c)], m_grid[hashCell(r + m_numRows - 1, c + 1)]);
}

void SpatialHashSolver::compareCells(Cell& c1, Cell& c2)
{
	for (std::size_t i1 = 0; i1 < c1.numBalls; ++i1)
	{
		for (std::size_t i2 = 0; i2 < c2.numBalls; ++i2)
		{
			if (c1.m_ballIDs[i1] == c2.m_ballIDs[i2])
				continue;

			if (overlap(c1.m_ballIDs[i1], c2.m_ballIDs[i2]))
				resolveCollision(c1.m_ballIDs[i1], c2.m_ballIDs[i2]);
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
	return (std::size_t)(((x - m_world.xMin) / m_world.xWidth) * (float)m_numRows);
}