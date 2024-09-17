#include "PruneAndSweep1DSolver.hpp"
#include "ball.hpp"

#include <algorithm>

PruneAndSweep1DSolver::PruneAndSweep1DSolver(std::vector<balltype> ballTypes)
	: Solver(ballTypes)
{
	// Initialise and sort m_leftSortedEnds and m_rightSortedEnds

	for (unsigned int i = 0; i < m_balls.size(); i++)
	{
		const ball& b = m_balls[i];
		float r = m_ballTypes[b.typeindex].radius;

		endpoints e;

		e.left  = b.position.x - r;
		e.right = b.position.x + r;
		e.ind   = i;

		m_leftSortedEnds.push_back(e);
		m_rightSortedEnds.push_back(e);
	}

	std::sort(m_leftSortedEnds.begin(),  m_leftSortedEnds.end(),  leftComparison);
	std::sort(m_rightSortedEnds.begin(), m_rightSortedEnds.end(), rightComparison);

	// Initialise empty sweep queues and stacks (one for each balltype)

	for (int i = 0; i < m_ballTypes.size(); i++)
	{
		m_sweepQueues.push_back({});
		m_sweepQueues.back().reserve(m_ballTypes[i].count);

		m_sweepStacks.push_back({});
		m_sweepStacks.back().reserve(m_ballTypes[i].count);
	}
}


void PruneAndSweep1DSolver::solve()
{
	// Insertion sort m_leftSortedEnds and m_rightSorted balls
	// By temporal coherence (i.e. balls moving very little between frames), sorting time is roughly O(n)
	insertionSort();

	const vec2<float>  leftTranslate(-2.0f, 0.0f);
	const vec2<float> rightTranslate( 2.0f, 0.0f);

	const std::vector<vec2<float>> verticalTranslates = { {0.0f, -2.0f}, {0.0f, 0.0f}, {0.0f, 2.0f} };

	clearQueues();

	// First sweep (bulk of calculations, excluding left and right overlaps)

	for (endpoints& b1 : m_leftSortedEnds)
	{
		const unsigned int& i1 = b1.ind;
		for (queue<endpoints>& q : m_sweepQueues)
		{

			while (!q.empty() && (b1.left > q.front().right))
				q.pop();

			for (endpoints& b2 : q)
			{
				const unsigned int& i2 = b2.ind;

				for (vec2<float> translate : verticalTranslates)
				{
					m_balls[i1].position.Add(translate);
					if (overlap(i1, i2))
					{
						resolveCollision(i1, i2);
					}
					m_balls[i1].position.Subtract(translate);
				}
			}
		}
		m_sweepQueues[m_balls[b1.ind].typeindex].push(b1);
	}

	// Second sweep (deal with overlaps on right-hand side and all balls on left-hand side)
	// Note right hand overlaps are already stored in the queues after the first sweep

	float leftEnd = m_leftSortedEnds[0].left;    // potentially less than -1.0f
	float rightEnd = m_rightSortedEnds[0].right; // potentially greater than 1.0f

	for (endpoints& b1 : m_leftSortedEnds)
	{
		if (b1.left >= rightEnd - 2.0f)
			break;

		const unsigned int& i1 = b1.ind;

		m_balls[i1].position.Add(rightTranslate);

		for (queue<endpoints>& q : m_sweepQueues)
		{

			while (!q.empty() && (b1.left + 2.0f > q.front().right))
				q.pop();

			for (endpoints& b2 : q)
			{
				const unsigned int& i2 = b2.ind;
				for (vec2<float> translate : verticalTranslates)
				{
					m_balls[i1].position.Add(translate);
					if (overlap(i1, i2))
					{
						resolveCollision(i1, i2);

						m_balls[i1].position.Subtract(translate);

						break;
					}
					m_balls[i1].position.Subtract(translate);
				}
			}
		}
		m_balls[i1].position.Subtract(rightTranslate);
	}

	// Third and final sweep (deal with overlaps on left-hand side and non-overlaps on right-hand side)
	// Queues are empty at this point

	// Find balls on right-hand side that do not cross boundary, but may overlap with left overlaps

	for (endpoints& b : m_leftSortedEnds)
	{
		if (b.left >= 0)
			break;

		// Shift ball to the right
		m_balls[b.ind].position.Add(rightTranslate);

		// Add ball to queue
		m_sweepStacks[m_balls[b.ind].typeindex].push_back(b);
	}

	int startind = 0;

	while (startind < m_rightSortedEnds.size() && m_rightSortedEnds[startind].right > 1.0f)
		startind++;

	for (int i = startind; i < m_rightSortedEnds.size(); i++)
	{
		if (m_rightSortedEnds[i].right < leftEnd + 2.0f)
			break;

		endpoints& b1 = m_rightSortedEnds[i];
		const unsigned int& i1 = b1.ind;

		for (std::vector<endpoints>& s : m_sweepStacks)
		{

			while (!s.empty() && (b1.right - 2.0f < s.back().left))
			{
				// Return shifted ball to original position
				int ind = s.back().ind;
				m_balls[ind].position.Subtract(rightTranslate);

				s.pop_back();
			}

			for (endpoints& b2 : s)
			{
				const unsigned int& i2 = b2.ind;

				for (vec2<float> translate : verticalTranslates)
				{
					m_balls[i1].position.Add(translate);
					if (overlap(i1, i2))
					{
						resolveCollision(i1, i2);

						m_balls[i1].position.Subtract(translate);

						break;
					}
					m_balls[i1].position.Subtract(translate);
				}
			}
		}
	}

	// Return shifted balls to original position
	for (std::vector<endpoints>& s : m_sweepStacks)
	{
		while (!s.empty())
		{
			m_balls[s.back().ind].position.Subtract(rightTranslate);
			s.pop_back();
		}
	}
}


void PruneAndSweep1DSolver::update(float dt)
{
	// Update positions in m_balls

	Solver::update(dt);

	// Update positions in m_leftSortedEnds and m_rightSortedEnds

	for (endpoints& e : m_leftSortedEnds)
	{
		ball        & b = m_balls[e.ind];
		vec2<float> & p = b.position;

		e.left  = p.x - m_ballTypes[b.typeindex].radius;
		e.right = p.x + m_ballTypes[b.typeindex].radius;
	}

	for (endpoints& e : m_rightSortedEnds)
	{
		ball        & b = m_balls[e.ind];
		vec2<float> & p = b.position;

		e.left  = p.x - m_ballTypes[b.typeindex].radius;
		e.right = p.x + m_ballTypes[b.typeindex].radius;
	}
}

void PruneAndSweep1DSolver::insertionSort()
{
	// Insertion sort m_leftSortedEnds

	for (int i = 1; i < m_leftSortedEnds.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_leftSortedEnds[j].left > m_leftSortedEnds[j + 1].left)
		{
			std::swap(m_leftSortedEnds[j], m_leftSortedEnds[j + 1]);
			j--;
		}
	}

	// Insertion sort m_rightSortedEnds

	for (int i = 1; i < m_rightSortedEnds.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_rightSortedEnds[j].right < m_rightSortedEnds[j + 1].right)
		{
			std::swap(m_rightSortedEnds[j], m_rightSortedEnds[j + 1]);
			j--;
		}
	}
}

void PruneAndSweep1DSolver::clearQueues()
{
	for (queue<endpoints>& q : m_sweepQueues)
	{
		q.clear();
	}
}