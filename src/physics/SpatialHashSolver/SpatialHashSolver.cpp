#include "SpatialHashSolver.hpp"

#include <cmath>
#include <iostream>
#include <chrono>

SpatialHashSolver::SpatialHashSolver(Preset preset)
	: Solver(preset) 
{
	m_numRows = 1 + static_cast<std::size_t>(
		m_world.yMax * std::sqrt(
			static_cast<double>(m_balls.size())
		)
	);

	m_numCols = 1 + static_cast<std::size_t>(
		m_world.xMax * std::sqrt(
			static_cast<double>(m_balls.size())
		)
	);

	m_grid.resize(m_numRows * m_numCols); // Number of cells is of order m_balls.size()
}

void SpatialHashSolver::solve()
{
	clearCells();

	populateCells();

	checkCollisions();
}

void SpatialHashSolver::clearCells()
{
	for (Cell& cell : m_grid)
		cell.clear();
}


void SpatialHashSolver::populateCells()
/**
 * Split task of populating cells across multiple
 * threads. Each thread then places balls in m_balls
 * with indices in the range [indLower, indUpper) into
 * cells
 */
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
		const Ball& ball = m_balls[i];

		const float& radius = m_ballTypes[ball.typeindex].radius;

		float xLeft  = ball.position.x - radius;
		float xRight = ball.position.x + radius;
		float yLow   = ball.position.y - radius;
		float yHigh  = ball.position.y + radius;

		for (int rowRaw = yPosToRow(yLow); rowRaw <= yPosToRow(yHigh); rowRaw++)
		{
			for (int colRaw = xPosToCol(xLeft); colRaw <= xPosToCol(xRight); colRaw++)
			{
				Offset offset = NONE;
				int row = rowRaw;
				int col = colRaw;

				if (row < 0)
				{
					row += m_numRows;
					offset = UP;
				}
				else if (row >= m_numRows)
				{
					row -= m_numRows;
					offset = DOWN;
				}

				if (col < 0)
				{
					col += m_numCols;
					offset = (Offset)(offset + RIGHT);
				}
				else if (col >= m_numCols)
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
 * Split task of checking collisions across multiple
 * threads. Each thread checks cells with row index
 * in the range [rowLower, rowUpper).
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
/**
 * Check collisions between pairs of balls within cells
 * having row number in the range [rowLower, rowUpper).
 */
{
	for (std::size_t row = rowLower; row < std::min(rowUpper, m_numRows); row++)
	{
		for (std::size_t col = 0; col < m_numCols; col++)
		{
			findCollisionsInCell(m_grid[hashCell(row, col)]);
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
	Ball& ball1 = m_balls[info1.ballID];
	Ball& ball2 = m_balls[info2.ballID];

	Vec2<float> translate1 = offsetToTranslate(info1.offset);
	Vec2<float> translate2 = offsetToTranslate(info2.offset);

	float& radius1 = m_ballTypes[ball1.typeindex].radius;
	float& radius2 = m_ballTypes[ball2.typeindex].radius;

	return distSquared(ball1.position + translate1, ball2.position + translate2) <= (radius1 + radius2) * (radius1 + radius2);
}

Vec2<float> SpatialHashSolver::offsetToTranslate(Offset& offset)
{
	switch (offset)
	{
		case NONE:       return Vec2<float>{           0.0f,            0.0f}; break;
		case LEFT:       return Vec2<float>{-m_world.xWidth,            0.0f}; break;
		case RIGHT:      return Vec2<float>{ m_world.xWidth,            0.0f}; break;
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

	Ball& ball1 = m_balls[info1.ballID];
	Ball& ball2 = m_balls[info2.ballID];
	
	Vec2<float> translate1 = offsetToTranslate(info1.offset);
	Vec2<float> translate2 = offsetToTranslate(info2.offset);
	
	ball1.position += translate1;
	ball2.position += translate2;

	Solver::resolveCollision(ball1, ball2);

	ball1.position -= translate1;
	ball2.position -= translate2;
}


std::size_t SpatialHashSolver::hashCell(std::size_t row, std::size_t col)
{
	return row * m_numCols + col;
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