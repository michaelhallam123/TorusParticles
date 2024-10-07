#pragma once

/**
 * A derived class of Solver implementing a simple
 * brute force algorithm.
 * 
 * Checks every pair of balls for a collision. Nine
 * checks are performed per each pair of balls to 
 * account for the possibility of boundary crossing
 * (in the spirit of a brute force algorithm, this
 * is intentionally inefficient)
 */

#include <vector>

#include "Solver.hpp"

class BruteForceSolver : public Solver
{
public:
	BruteForceSolver(std::vector<BallType> ballTypes, float worldAspectRatio);
	void solve() override;
};