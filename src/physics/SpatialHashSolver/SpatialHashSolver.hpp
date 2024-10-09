#pragma once

#include <future>
#include <mutex>

#include "Solver.hpp"

#include "Cell.hpp"


/**
 * A derived class of Solver implementing a spatial hash 
 * algorithm.
 * 
 * The world space is subdivided into a grid of cells.
 * At the beginning of each frame, each cell in the grid
 * is populated with the balls it shares x- and y-overlaps 
 * with, including whether how the ball must be translated to 
 * reach that position. Collisions between balls are then 
 * checked for pairs of balls in each cell.
 * 
 * Multithreading is used to split populating and collision
 * checking across threads.
 */

class SpatialHashSolver : public Solver
{
public:
	SpatialHashSolver(Preset preset);

private:
	std::vector<Cell> m_grid;
	std::size_t       m_numRows;
	std::size_t       m_numCols;

	void solve() override;

	void clearCells();

	void populateCells();
	void populateCellsInRange(std::size_t rowLower, std::size_t rowUpper);

	void checkCollisions();
	void checkCollisionsInRange(std::size_t rowLower, std::size_t rowUpper);

	void findCollisionsInCell(Cell& c1);

	// Methods for hashing ball positions
	std::size_t hashCell(std::size_t row, std::size_t col);
	int xPosToCol(float x);
	int yPosToRow(float y);

	// Multithreading compatible methods
	bool overlap(BallInfo& info1, BallInfo& info2);
	Vec2<float> offsetToTranslate(Offset& offset);
	void resolveCollision(BallInfo& info1, BallInfo& info2);

	// Multithreading data
	static const unsigned int NUM_THREADS = 10;
	std::mutex m_mutex;
	std::vector<std::future<void>> m_futures;
};