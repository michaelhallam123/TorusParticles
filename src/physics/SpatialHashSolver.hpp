#pragma once

#include <future>
#include <mutex>

#include "Solver.hpp"

#include "Cell.hpp"


class SpatialHashSolver : public Solver
{
public:
	SpatialHashSolver(std::vector<BallType> ballTypes);

private:
	std::vector<Cell> m_grid;
	std::size_t       m_numRows;   // Number of rows in grid, equals number of columns
	float             m_cellWidth;

	void solve() override;
	void clearCells();
	void populateCells();
	void checkCollisions();
	void checkCollisionsInRange(std::size_t rowLower, std::size_t rowUpper);
	void findCollisionsInCell(Cell& c1);

	std::size_t hashCell(std::size_t row, std::size_t col);
	int posToCell(float x);

	bool overlap(BallInfo& info1, BallInfo& info2);

	Vec2<float> offsetToTranslate(Offset& offset);

	void resolveCollision(BallInfo& info1, BallInfo& info2);

	std::mutex m_mutex;
	std::vector<std::future<void>> m_futures;

};