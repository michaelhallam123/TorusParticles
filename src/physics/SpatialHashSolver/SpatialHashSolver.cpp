#include "SpatialHashSolver.hpp"

#include <cmath>
#include <iostream>
#include <chrono>

SpatialHashSolver::SpatialHashSolver(Preset preset)
	: Solver(preset) 
{
	std::size_t numRows = 1 + static_cast<std::size_t>(std::sqrt(static_cast<double>(m_balls.size())));

	m_numRows    = numRows;
	m_numCols    = numRows;
	m_cellWidth  = m_world.xWidth / static_cast<float>(m_numCols);
	m_cellHeight = m_world.yWidth / static_cast<float>(m_numRows);

	m_grid.resize(m_numRows * m_numCols); // Number of cells is order m_balls.size()
}

void SpatialHashSolver::solve()
{
	clearCells();

	populateCells();

	checkCollisions();
}

void SpatialHashSolver::clearCells()
{
	for (Cell& c : m_grid)
		c.clear();
}


void SpatialHashSolver::populateCells()
{
	m_futures.clear();

	for (unsigned int i = 0; i < NUM_THREADS; i++)
	{
		std::size_t indLower = std::min(m_balls.size(), i * (m_balls.size() / NUM_THREADS + 1));
		std::size_t indUpper = std::min(m_balls.size(), (i + 1) * (m_balls.size() / NUM_THREADS + 1));

		m_futures.push_back(
			std::async(
				std::launch::async, 
				[this, indLower, indUpper](){return populateCellsInRange(indLower, indUpper);} 
			)
		);
	}

	for (std::future<void>& future : m_futures)
		future.wait();
}

void SpatialHashSolver::populateCellsInRange(std::size_t indLower, std::size_t indUpper)
/*
 * Iterate through entries of m_balls with indices
 * in the specified range [indLower, indUpper), storing
 * their info in m_grid.
 */
{
	for (std::size_t i = indLower; i < indUpper; i++)
	{
		const Ball& b = m_balls[i];

		const float& r = m_ballTypes[b.typeindex].radius;

		float xLeft  = b.position.x - r;
		float xRight = b.position.x + r;
		float yLow   = b.position.y - r;
		float yHigh  = b.position.y + r;

		for (int r = yPosToRow(yLow); r <= yPosToRow(yHigh); r++)
		{
			for (int c = xPosToCol(xLeft); c <= xPosToCol(xRight); c++)
			{
				Offset offset = NONE;
				int row = r;
				int col = c;

				if (r < 0)
				{
					row += m_numRows;
					offset = UP;
				}
				else if (r >= m_numRows)
				{
					row -= m_numRows;
					offset = DOWN;
				}

				if (c < 0)
				{
					col += m_numCols;
					offset = (Offset)(offset + RIGHT);
				}
				else if (c >= m_numCols)
				{
					col -= m_numCols;
					offset = (Offset)(offset + LEFT);
				}

				m_grid[hashCell(row, col)].addBall({i, offset});
			}
		}
	}
}

void SpatialHashSolver::checkCollisions()
/**
 * Iterate through each cell checking for collisions
 */
{
	m_futures.clear();

	for (unsigned int i = 0; i < NUM_THREADS; i++)
	{
		std::size_t rowLower = std::min(m_numRows, i * (m_numRows / NUM_THREADS + 1));
		std::size_t rowUpper = std::min(m_numRows, (i + 1) * (m_numRows / NUM_THREADS + 1));

		m_futures.push_back(
			std::async(
				std::launch::async, 
				[this, rowLower, rowUpper](){return checkCollisionsInRange(rowLower, rowUpper);} 
			)
		);
	}

	for (std::future<void>& future : m_futures)
		future.wait();
}

void SpatialHashSolver::checkCollisionsInRange(std::size_t rowLower, std::size_t rowUpper)
{
	for (std::size_t r = rowLower; r < std::min(rowUpper, m_numRows); r++)
	{
		for (std::size_t c = 0; c < m_numCols; c++)
		{
			findCollisionsInCell(m_grid[hashCell(r, c)]);
		}
	}
}

void SpatialHashSolver::findCollisionsInCell(Cell& cell)
{
	for (std::size_t i1 = 0; i1 < cell.numBalls; i1++)
	{
		BallInfo& info1 = cell.ballList[i1];
		for (std::size_t i2 = i1 + 1; i2 < cell.numBalls; i2++)
		{
			BallInfo& info2 = cell.ballList[i2];
			if (overlap(info1, info2))
				resolveCollision(info1, info2);
		}
	}
}

bool SpatialHashSolver::overlap(BallInfo& info1, BallInfo& info2)
{
	Ball& b1 = m_balls[info1.ballID];
	Ball& b2 = m_balls[info2.ballID];

	Vec2<float> translate1 = offsetToTranslate(info1.offset);
	Vec2<float> translate2 = offsetToTranslate(info2.offset);

	float& r1 = m_ballTypes[b1.typeindex].radius;
	float& r2 = m_ballTypes[b2.typeindex].radius;

	return distSquared(b1.position + translate1, b2.position + translate2) <= (r1 + r2) * (r1 + r2);
}

Vec2<float> SpatialHashSolver::offsetToTranslate(Offset& offset)
{
	switch (offset)
	{
		case NONE:       return Vec2<float>{           0.0f,  0.0f          }; break;
		case LEFT:       return Vec2<float>{-m_world.xWidth,  0.0f          }; break;
		case RIGHT:      return Vec2<float>{ m_world.xWidth,  0.0f          }; break;
		case DOWN:       return Vec2<float>{           0.0f, -m_world.yWidth}; break;
		case DOWN_LEFT:  return Vec2<float>{-m_world.xWidth, -m_world.yWidth}; break;
		case DOWN_RIGHT: return Vec2<float>{ m_world.xWidth, -m_world.yWidth}; break;
		case UP:         return Vec2<float>{           0.0f,  m_world.yWidth}; break;
		case UP_LEFT:    return Vec2<float>{-m_world.xWidth,  m_world.yWidth}; break;
		case UP_RIGHT:   return Vec2<float>{ m_world.xWidth,  m_world.yWidth}; break;
	}

	return Vec2<float>{0.0f, 0.0f};
}

void SpatialHashSolver::resolveCollision(BallInfo& info1, BallInfo& info2)
{
	std::lock_guard<std::mutex> lock(m_mutex); // Prevent simultaneously editing balls in different threads

	Ball& b1 = m_balls[info1.ballID];
	Ball& b2 = m_balls[info2.ballID];
	
	Vec2<float> translate1 = offsetToTranslate(info1.offset);
	Vec2<float> translate2 = offsetToTranslate(info2.offset);
	
	b1.position += translate1;
	b2.position += translate2;

	Solver::resolveCollision(b1, b2);

	b1.position -= translate1;
	b2.position -= translate2;
}


std::size_t SpatialHashSolver::hashCell(std::size_t row, std::size_t col)
{
	return (row % m_numRows) * m_numCols + (col % m_numCols);
}

int SpatialHashSolver::yPosToRow(float y)
{
	return static_cast<int>(
		std::floor(
			((y - m_world.yMin) / m_world.yWidth) * static_cast<float>(m_numRows)
		)
	);
}

int SpatialHashSolver::xPosToCol(float x)
{
	return static_cast<int>(
		std::floor(
			((x - m_world.xMin) / m_world.xWidth) * static_cast<float>(m_numCols)
		)
	);
}