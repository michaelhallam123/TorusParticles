#include "PruneAndSweep1DSolver.hpp"
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

	std::sort(m_leftSortedEnds.begin(),  m_leftSortedEnds.end(),  leftEndsIncreasing);
	std::sort(m_rightSortedEnds.begin(), m_rightSortedEnds.end(), rightEndsDecreasing);

	// Initialise and allocate space for sweep queues and stacks

	for (int i = 0; i < m_ballTypes.size(); i++)
	{
		m_sweepQueues.push_back({});
		m_sweepQueues.back().reserve(m_ballTypes[i].count);

		m_sweepStacks.push_back({});
		m_sweepStacks.back().reserve(m_ballTypes[i].count);
	}
}


void PruneAndSweep1DSolver::solve()
/*
 * Check for collisions between balls using a sweep and prune algorithm
 * along the x-axis, and update velocities accordingly. 
 *
 * To deal with balls crossing the left and right hand boundaries of 
 * world space, three sweeps are performed. The first two sweeps 
 * (dealing with the bulk of balls, and balls crossing the right-hand 
 * boundary) are from left to right, using FIFO queues stored in 
 * m_sweepQueues. The final sweep (dealing with balls crossing the 
 * left-hand boundary) is from right to left, using stacks stored in 
 * m_sweepStacks.
 */
{
	// Sort m_leftSortedEnds and m_rightSortedEnds

	insertionSortEnds();

	// Prepare queues in m_sweepQueues to be filled

	clearQueues();

	// First sweep: deal with bulk of calculations, ignoring balls crossing left and right boundaries

	for (endpoints& e1 : m_leftSortedEnds)
	{
		for (queue<endpoints>& q : m_sweepQueues)
		{
			while (!q.empty() && (e1.left > q.front().right))
				q.pop();

			for (endpoints& e2 : q)
			{
				checkCollision(e1, e2);
			}
		}
		m_sweepQueues[m_balls[e1.ind].typeindex].push(e1);
	}

	// Second sweep: deal with balls crossing right-hand boundary

	const vec2<float> rightTranslate(xWorldWidth, 0.0f);

	float leftEnd  = m_leftSortedEnds[0].left   + xWorldWidth; // Left-most overlapping endpoint
	float rightEnd = m_rightSortedEnds[0].right - xWorldWidth; // Right-most overlapping endpoint

	for (endpoints& e1 : m_leftSortedEnds)
	{
		if (e1.left >= rightEnd)
			break;

		m_balls[e1.ind].position.Add(rightTranslate);

		for (queue<endpoints>& q : m_sweepQueues)
		{
			while (!q.empty() && (e1.left + xWorldWidth > q.front().right))
				q.pop();

			for (endpoints& e2 : q)
			{
				checkCollision(e1, e2);
			}
		}
		m_balls[e1.ind].position.Subtract(rightTranslate);
	}

	// Third and final sweep: deal with balls crossing left-hand boundary

	for (endpoints& e : m_leftSortedEnds)
	{
		if (e.left >= 0)
			break;

		m_balls[e.ind].position.Add(rightTranslate);
		m_sweepStacks[m_balls[e.ind].typeindex].push_back(e);
	}

	int startind = 0;

	while (startind < m_rightSortedEnds.size() && m_rightSortedEnds[startind].right > xWorldMax)
		startind++;

	for (int i = startind; i < m_rightSortedEnds.size(); i++)
	{
		if (m_rightSortedEnds[i].right < leftEnd)
			break;

		endpoints& e1 = m_rightSortedEnds[i];
		const unsigned int& i1 = e1.ind;

		for (std::vector<endpoints>& s : m_sweepStacks)
		{

			while (!s.empty() && (e1.right - xWorldWidth < s.back().left))
			{
				m_balls[s.back().ind].position.Subtract(rightTranslate);
				s.pop_back();
			}

			for (endpoints& e2 : s)
			{
				checkCollision(e1, e2);
			}
		}
	}

	// Return shifted balls to original position and remove from stacks

	for (std::vector<endpoints>& s : m_sweepStacks)
	{
		while (!s.empty())
		{
			m_balls[s.back().ind].position.Subtract(rightTranslate);
			s.pop_back();
		}
	}
}

void PruneAndSweep1DSolver::checkCollision(endpoints& e1, endpoints& e2)
/*
 * Check for a collision between balls corresponding to e1, e2
 * If a collision is detected, update velocities using resolveCollision
 */
{
	static const std::vector<vec2<float>> verticalTranslates = { {0.0f, -yWorldWidth}, {0.0f, 0.0f}, {0.0f,yWorldWidth} };

	const unsigned int& i1 = e1.ind;
	const unsigned int& i2 = e2.ind;

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


void PruneAndSweep1DSolver::update(float dt)
/*
 * Having solved all collisions, update positions according to velocities
 * Updates m_balls, m_leftSortedEnds, m_rightSortedEnds
 * Overrides the update method of Solver
 */
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

void PruneAndSweep1DSolver::insertionSortEnds()
/* 
 * Sort m_leftSortedEnds and m_rightSortedEnds using insertion sort
 * Sorting time is roughly O(n), as these vectors remain mostly sorted each frame
 */
{
	// Insertion sort m_leftSortedEnds (increasing by left endpoint)

	for (int i = 1; i < m_leftSortedEnds.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_leftSortedEnds[j].left > m_leftSortedEnds[j + 1].left)
		{
			std::swap(m_leftSortedEnds[j], m_leftSortedEnds[j + 1]);
			j--;
		}
	}

	// Insertion sort m_rightSortedEnds (decreasing by right endpoint)

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