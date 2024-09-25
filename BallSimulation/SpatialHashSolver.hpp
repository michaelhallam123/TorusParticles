#pragma once

#include "Solver.hpp"

#include "Cell.hpp"


class SpatialHashSolver : public Solver
{
public:
	SpatialHashSolver(std::vector<BallType> ballTypes);

private:
	std::vector<Cell> m_grid;
	std::size_t m_numRows;      // Number of rows in grid, equals number of columns
	float m_cellWidth;

	void solve() override;
	void clearCells();
	void populateCells();
	void checkCollisions();
	void findCollisionsInCell(Cell& c1);

	std::size_t hashCell(std::size_t row, std::size_t col);
	std::size_t posToCell(float x);

	void shiftUp(std::size_t r, std::size_t c);
	void shiftDown(std::size_t r, std::size_t c);
	void shiftLeft(std::size_t r, std::size_t c);
	void shiftRight(std::size_t r, std::size_t c);

	// Collisions cell

};