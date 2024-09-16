
#include "PruneAndSweep2DSolver.hpp"

#include <algorithm>
#include <iostream>

PruneAndSweep2DSolver::PruneAndSweep2DSolver(std::vector<balltype> ballTypes)
	: Solver(ballTypes)
{
	for (unsigned int i = 0; i < m_balls.size(); i++)
	{
		ball& b = m_balls[i];
		float radius = m_ballTypes[b.typeindex].radius;

		m_xSorted.push_back({ b.position.x - radius, i, 'l'});
		m_xSorted.push_back({ b.position.x + radius, i, 'r' });
		m_ySorted.push_back({ b.position.y - radius, i, 'l'});
		m_ySorted.push_back({ b.position.y + radius, i, 'r'});
	}

	std::sort(m_xSorted.begin(), m_xSorted.end());
	std::sort(m_ySorted.begin(), m_ySorted.end());

	// Check for overlapping boxes without wrapping
	for (unsigned int i = 0; i < m_balls.size(); i++)
	{
		ball b1 = m_balls[i];
		float r1 = m_ballTypes[b1.typeindex].radius;
		for (unsigned int j = i + 1; j < m_balls.size(); j++)
		{
			ball b2 = m_balls[j];
			float r2 = m_ballTypes[b2.typeindex].radius;

			// Check x-overlap between balls i and j

			if (b2.position.x - r2 < b1.position.x - r1 && b1.position.x - r1 < b2.position.x + r2 ||
				b1.position.x - r1 < b2.position.x - r2 && b2.position.x - r2 < b1.position.x + r1)
			{
				m_overlapCounts[{i, j}]++;
			}

			// Check y-overlap between balls i and j

			if (b2.position.y - r2 < b1.position.y - r1 && b1.position.y - r1 < b2.position.y + r2 ||
				b1.position.y - r1 < b2.position.y - r2 && b2.position.y - r2 < b1.position.y + r1)
			{
				m_overlapCounts[{i, j}]++;
				if (m_overlapCounts[{i, j}] == 2)
					m_overlaps.insert({ i, j });
			}
		}
	}

	wrapPositions(m_xSorted);
	wrapPositions(m_ySorted);
}

void PruneAndSweep2DSolver::solve()
{
	sortAndTrack(m_xSorted, 0, m_xSorted.size());
	sortAndTrack(m_ySorted, 0, m_ySorted.size());

	wrapPositions(m_xSorted);
	wrapPositions(m_ySorted);

	// TO DO: incorporate this into wrap positions
	sortAndTrack(m_xSorted, 0, m_xSorted.size());
	sortAndTrack(m_ySorted, 0, m_ySorted.size());

	for (auto& p : m_overlaps)
	{
		if (overlap(p.first, p.second))
		{
			resolveCollision(p.first, p.second);
		}
	}
}

// Deal with case where endpoints go beyond boundaries

void PruneAndSweep2DSolver::wrapPositions(std::deque<endpoint>& sorted)
{
	std::vector<endpoint> lefts;
	std::vector<endpoint> rights;

	int leftCount = 0;
	while (leftCount < sorted.size() && sorted[leftCount].pos < -1.0f)
		leftCount++;

	int rightCount = 0;
	while (rightCount < sorted.size() && sorted[sorted.size() - 1 - rightCount].pos > 1.0f)
		rightCount++;

	lefts.reserve(leftCount);
	rights.reserve(rightCount);

	while (!sorted.empty() && sorted.front().pos < -1.0f)
	{
		sorted.front().pos += 2.0f;
		lefts.push_back(sorted.front());
		sorted.pop_front();
	}
	while (!sorted.empty() && sorted.back().pos > 1.0f)
	{
		sorted.back().pos -= 2.0f;
		rights.push_back(sorted.back());
		sorted.pop_back();
	}

	for (endpoint& x : lefts)
	{
		for (endpoint& y : rights)
		{
			std::pair<unsigned int, unsigned int> p;

			if (x.ind < y.ind)
				p = std::make_pair(x.ind, y.ind);
			else
				p = std::make_pair(y.ind, x.ind);

			if (x.lr == 'l' && y.lr == 'r')
			{
				m_overlapCounts[p]++;
				if (m_overlapCounts[p] == 2)
					m_overlaps.insert(p);
			}
			else if (x.lr == 'r' && y.lr == 'l')
			{
				m_overlapCounts[p]--;
				m_overlaps.erase(p);
			}
		}
	}

	for (int i = 0; i < lefts.size(); i++)
	{
		sorted.push_back(lefts[i]);
	}

	for (int i = 0; i < rights.size(); i++)
	{
		sorted.push_front(rights[i]);
	}
}

// Sort unsorted between indices [l,r), and track overlaps

void PruneAndSweep2DSolver::sortAndTrack(std::deque<endpoint>& unsorted, unsigned int l, unsigned int r)
{
	for (unsigned int i = l+1; i < r; i++)
	{
		for (unsigned int j = i; j > l; j--)
		{
			if (unsorted[j].pos < unsorted[j - 1].pos)
			{
				std::pair<unsigned int, unsigned int> p;

				if (unsorted[j].ind < unsorted[j-1].ind)
					p = std::make_pair(unsorted[j].ind, unsorted[j - 1].ind);
				else
					p = std::make_pair(unsorted[j - 1].ind, unsorted[j].ind);

				if (unsorted[j - 1].lr == 'l' && unsorted[j].lr == 'r')
				{
					m_overlapCounts[p]--;
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

void PruneAndSweep2DSolver::update(float dt)
{
	// Update positions in m_balls using base class updater

	Solver::update(dt);

	// Update positions in sorted lists
	
	for (endpoint& p : m_xSorted)
	{
		p.pos += dt * m_balls[p.ind].velocity.x;
	}
	for (endpoint& p : m_ySorted)
	{
		p.pos += dt * m_balls[p.ind].velocity.y;
	}
}