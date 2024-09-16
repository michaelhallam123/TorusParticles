#pragma once

#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <iostream>

#include "Solver.hpp"
#include "balltype.hpp"

// Struct to store position and index data for the prune and sweep algorithm
struct endpoint
{
	float        pos; // Position in [-1,1] of the endpoint (max or min x or y coordinate) of a ball
	unsigned int ind; // Index of the ball in m_ballData
	char         lr;  // 'l' if a left endpoint, 'r' if a right endpoint

	bool operator<(const endpoint& rhs) const { return pos < rhs.pos; }
};


template<>
struct std::hash<std::pair<unsigned int, unsigned int>>
{
	std::size_t operator()(const std::pair<unsigned int, unsigned int>& p) const
	{
		return std::hash<unsigned int>{}(p.first) ^ std::hash<unsigned int>{}(p.second);
	}
};


class PruneAndSweep2DSolver : public Solver
{
public:
	PruneAndSweep2DSolver(std::vector<balltype> ballTypes);
	void solve() override;
	void update(float dt) override;

private:
	std::deque<endpoint> m_xSorted;
	std::deque<endpoint> m_ySorted;
	std::unordered_set<std::pair<unsigned int, unsigned int>> m_overlaps;                     // Store here if both are overlapping
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> m_overlapCounts; // 0, 1, or 2

	void wrapPositions(std::deque<endpoint>& sorted);

	void sortAndTrack(std::deque<endpoint>& unsorted, unsigned int l, unsigned int r);
};
