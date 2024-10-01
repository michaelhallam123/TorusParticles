#pragma once

#include "Solver.hpp"
#include "BallType.hpp"
#include "Endpoints.hpp"
#include "Queue.hpp"
#include <vector>

/*
 * A subclass of Solver implementing a 1-dimensional sweep
 * and prune algorithm to detect collisions between balls.
 */

class SweepAndPrune1DSolver : public Solver
{
public:
	SweepAndPrune1DSolver(std::vector<BallType> ballTypes);
	void solve()          override;
	void update(float dt) override;

private:
	std::vector<Queue<Endpoints>>       m_sweepQueues; // Custom queue implementation in Queue.hpp
	std::vector<std::vector<Endpoints>> m_sweepStacks;
	std::vector<Endpoints>              m_leftSortedEnds;
	std::vector<Endpoints>              m_rightSortedEnds;

	void insertionSortEnds();

	void clearQueues();

	void checkCollision(Endpoints& e1, Endpoints& e2);
};