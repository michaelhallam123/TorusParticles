#pragma once

#include "Solver.hpp"
#include "balltype.hpp"
#include "lineSweepData.h"

#include <vector>

class PruneAndSweep1DSolver : public Solver
{
public:
	PruneAndSweep1DSolver(std::vector<balltype> ballTypes);
	void solve() override;
	void update(float dt) override;

private:
	std::vector<std::vector<lineSweepData>> m_sweepQueues;
	std::vector<unsigned int>               m_sweepQueuesStarts;
	std::vector<lineSweepData>              m_leftSortedBalls;
	std::vector<lineSweepData>              m_rightSortedBalls;

	void insertionSortLeft();
	void insertionSortRight();
};