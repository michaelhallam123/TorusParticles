#pragma once

#include "Solver.hpp"


class SpatialHashSolver : public Solver
{
public:
	SpatialHashSolver(std::vector<BallType> ballTypes);
};