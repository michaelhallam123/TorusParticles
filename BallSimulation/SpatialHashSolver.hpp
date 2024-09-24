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
	void compareCells(Cell& c1, Cell& c2);
	void checkPrimitiveNeighbors(std::size_t r, std::size_t c);

	std::size_t hashCell(std::size_t row, std::size_t col);
	std::size_t posToCell(float x);

	// Collisions cell

};