/*
#include "PruneAndSweepSolver.hpp"

#include <algorithm>

PruneAndSweepSolver::PruneAndSweepSolver(std::vector<balltype> ballTypes)
	: Solver(ballTypes)
{
	for (unsigned int i = 0; i < m_state.balls.size(); i++)
	{
		ball& b = m_state.balls[i];
		float radius = m_state.getRadius(i);

		m_xSorted.push_back({ b.position.x - radius, i, 'l'});
		m_xSorted.push_back({ b.position.x + radius, i, 'r' });
		m_ySorted.push_back({ b.position.y - radius, i, 'l'});
		m_ySorted.push_back({ b.position.y + radius, i, 'r'});
	}

	std::sort(m_xSorted.begin(), m_xSorted.end());
	std::sort(m_ySorted.begin(), m_ySorted.end());

	wrapPositions(m_xSorted);
	wrapPositions(m_ySorted);

	// Check for overlaps
	for (unsigned int i = 0; i < m_state.balls.size(); i++)
	{
		ball b1 = m_state.balls[i];
		float r1 = m_state.getRadius(i);
		for (unsigned int j = i+1; j < m_state.balls.size(); i++)
		{
			ball b2 = m_state.balls[j];
			float r2 = m_state.getRadius(j);

		}
	}
}

void PruneAndSweepSolver::solve()
{

}

void PruneAndSweepSolver::wrapPositions(std::deque<endpoint>& sorted)
{
	std::deque<endpoint> lefts;
	std::deque<endpoint> rights;

	while (!sorted.empty() && sorted.front().pos < -1.0f)
	{
		sorted.front().pos += 2.0f;
		lefts.push_back(sorted.front());
		sorted.pop_front();
	}
	while (!sorted.empty() && sorted.back().pos > 1.0f)
	{
		sorted.back().pos -= 2.0f;
		rights.push_front(sorted.back());
		sorted.pop_back();
	}

	while (!lefts.empty())
	{
		sorted.push_back(lefts.front());
		lefts.pop_front();
	}
	while (!rights.empty())
	{
		sorted.push_front(rights.back());
		rights.pop_back();
	}
}

// Sort unsorted between indices [l,r), and track overlaps

void PruneAndSweepSolver::sortAndTrack(std::deque<endpoint>& unsorted, unsigned int l, unsigned int r)
{
	for (unsigned int i = l+1; i < r; i++)
	{
		for (unsigned int j = i; i > l; j--)
		{
			if (unsorted[j].pos < unsorted[j - 1].pos)
			{
				auto p = std::make_pair(unsorted[j].ind, unsorted[j-1].ind);
				if (unsorted[j - 1].lr == 'l' && unsorted[j].lr == 'r')
				{
					m_overlapCounts[p]--;
					if (m_overlapCounts[p] == 1)
						m_overlaps.erase(p);
				}
				else if (unsorted[j - 1].lr == 'r' && unsorted[j].lr == 'l')
				{
					m_overlapCounts[p]++;
					if (m_overlapCounts[p] == 2)
						m_overlaps.insert(p);
				}
				std::swap(unsorted[j - 1], unsorted[j]);
			}
			else
				break;
		}
	}
}
*/