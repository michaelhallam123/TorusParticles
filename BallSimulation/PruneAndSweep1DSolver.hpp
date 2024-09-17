#pragma once

#include "Solver.hpp"
#include "balltype.hpp"
#include "endpoints.hpp"
#include "queue.hpp"

#include <vector>

class PruneAndSweep1DSolver : public Solver
{
public:
	PruneAndSweep1DSolver(std::vector<balltype> ballTypes);
	void solve() override;
	void update(float dt) override;

private:
	std::vector<queue<endpoints>>       m_sweepQueues; // Custom queue implementation in queue.hpp
	std::vector<std::vector<endpoints>> m_sweepStacks;
	std::vector<endpoints>              m_leftSortedEnds;
	std::vector<endpoints>              m_rightSortedEnds;

	void insertionSort();

	void clearQueues();
};