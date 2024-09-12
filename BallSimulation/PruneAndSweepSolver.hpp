/*#pragma once

#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <utility>

#include "Solver.hpp"
#include "balltype.hpp"

// Struct to store position and index data for the prune and sweep algorithm
struct endpoint
{
	float        pos; // Position in [-1,1] of the endpoint of a ball
	unsigned int ind; // Index of the ball in m_ballData
	char         lr;  // 'l' if a left endpoint, 'r' if a right endpoint
};

class PruneAndSweepSolver : public Solver
{
public:
	PruneAndSweepSolver(std::vector<balltype> ballTypes);
	void solve() override;

private:
	std::deque<endpoint> m_xSorted;
	std::deque<endpoint> m_ySorted;
	std::unordered_set<std::pair<int, int>> m_overlaps;           // Store here if both are overlapping
	std::unordered_map<std::pair<int, int>, int> m_overlapCounts; // 0, 1, or 2

	void wrapPositions(std::deque<endpoint>& sorted);

	void sortAndTrack(std::deque<endpoint>& unsorted, unsigned int l, unsigned int r);
};
*/