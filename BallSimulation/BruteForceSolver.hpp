#pragma once

#include <vector>

#include "Solver.hpp"

class BruteForceSolver : public Solver
{
public:
	BruteForceSolver(std::vector<balltype> ballTypeData);
	void solve() override;
};