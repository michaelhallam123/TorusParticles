#include "SweepAndPrune1DSolver.hpp"
#include <algorithm>

SweepAndPrune1DSolver::SweepAndPrune1DSolver(std::vector<BallType> ballTypes)
	: Solver(ballTypes)
{
	// Initialise and sort m_leftSortedEnds and m_rightSortedEnds

	for (std::size_t i = 0; i < m_balls.size(); i++)
	{
		const Ball& b = m_balls[i];
		float r = m_ballTypes[b.typeindex].radius;

		Endpoints e;

		e.left  = b.position.x - r;
		e.right = b.position.x + r;
		e.ind   = i;

		m_leftSortedEnds.push_back(e);
		m_rightSortedEnds.push_back(e);
	}

	std::sort(m_leftSortedEnds.begin(),  m_leftSortedEnds.end(),  leftEndsIncreasing);
	std::sort(m_rightSortedEnds.begin(), m_rightSortedEnds.end(), rightEndsDecreasing);

	// Initialise and allocate space for sweep queues and stacks

	for (std::size_t i = 0; i < m_ballTypes.size(); i++)
	{
		m_sweepQueues.push_back({});
		m_sweepQueues.back().reserve(m_ballTypes[i].count);

		m_sweepStacks.push_back({});
		m_sweepStacks.back().reserve(m_ballTypes[i].count);
	}
}


void SweepAndPrune1DSolver::solve()
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

	for (Endpoints& e1 : m_leftSortedEnds)
	{
		for (Queue<Endpoints>& q : m_sweepQueues)
		{
			while (!q.empty() && (e1.left > q.front().right))
				q.pop();

			for (Endpoints& e2 : q)
			{
				checkCollision(e1, e2);
			}
		}
		m_sweepQueues[m_balls[e1.ind].typeindex].push(e1);
	}

	// Second sweep: deal with balls crossing right-hand boundary

	const Vec2<float> rightTranslate(m_world.xWidth, 0.0f);

	float leftEnd  = m_leftSortedEnds[0].left   + m_world.xWidth; // Left-most overlapping endpoint
	float rightEnd = m_rightSortedEnds[0].right - m_world.xWidth; // Right-most overlapping endpoint

	for (Endpoints& e1 : m_leftSortedEnds)
	{
		if (e1.left >= rightEnd)
			break;

		m_balls[e1.ind].position.add(rightTranslate);

		for (Queue<Endpoints>& q : m_sweepQueues)
		{
			while (!q.empty() && (e1.left + m_world.xWidth > q.front().right))
				q.pop();

			for (Endpoints& e2 : q)
			{
				checkCollision(e1, e2);
			}
		}
		m_balls[e1.ind].position.subtract(rightTranslate);
	}

	// Third and final sweep: deal with balls crossing left-hand boundary

	for (Endpoints& e : m_leftSortedEnds)
	{
		if (e.left >= m_world.xMin)
			break;

		m_balls[e.ind].position.add(rightTranslate);
		m_sweepStacks[m_balls[e.ind].typeindex].push_back(e);
	}

	std::size_t startind = 0;

	while (startind < m_rightSortedEnds.size() && m_rightSortedEnds[startind].right > m_world.xMax)
		startind++;

	for (std::size_t i = startind; i < m_rightSortedEnds.size(); i++)
	{
		if (m_rightSortedEnds[i].right < leftEnd)
			break;

		Endpoints& e1 = m_rightSortedEnds[i];

		for (std::vector<Endpoints>& s : m_sweepStacks)
		{

			while (!s.empty() && (e1.right - m_world.xWidth < s.back().left))
			{
				m_balls[s.back().ind].position.subtract(rightTranslate);
				s.pop_back();
			}

			for (Endpoints& e2 : s)
			{
				checkCollision(e1, e2);
			}
		}
	}

	// Return shifted balls to original position and remove from stacks

	for (std::vector<Endpoints>& s : m_sweepStacks)
	{
		while (!s.empty())
		{
			m_balls[s.back().ind].position.subtract(rightTranslate);
			s.pop_back();
		}
	}
}

void SweepAndPrune1DSolver::checkCollision(Endpoints& e1, Endpoints& e2)
/*
 * Check for a collision between balls corresponding to e1, e2
 * If a collision is detected, update velocities using resolveCollision
 */
{
	static const std::vector<Vec2<float>> verticalTranslates = { {0.0f, -m_world.yWidth}, 
		                                                         {0.0f,  0.0f          }, 
		                                                         {0.0f,  m_world.yWidth} };

	std::size_t i1 = e1.ind;
	std::size_t i2 = e2.ind;

	for (Vec2<float> translate : verticalTranslates)
	{
		m_balls[i1].position.add(translate);
		if (overlap(i1, i2))
		{
			resolveCollision(i1, i2);
		}
		m_balls[i1].position.subtract(translate);
	}
}


void SweepAndPrune1DSolver::update(float dt)
/*
 * Having solved all collisions, update positions according to velocities
 * Updates m_balls, m_leftSortedEnds, m_rightSortedEnds
 * Overrides the update method of Solver
 */
{
	// Update positions in m_balls
	Solver::update(dt);

	// Update positions in m_leftSortedEnds and m_rightSortedEnds

	for (Endpoints& e : m_leftSortedEnds)
	{
		Ball&        b = m_balls[e.ind];
		Vec2<float>& p = b.position;

		e.left  = p.x - m_ballTypes[b.typeindex].radius;
		e.right = p.x + m_ballTypes[b.typeindex].radius;
	}

	for (Endpoints& e : m_rightSortedEnds)
	{
		Ball&        b = m_balls[e.ind];
		Vec2<float>& p = b.position;

		e.left  = p.x - m_ballTypes[b.typeindex].radius;
		e.right = p.x + m_ballTypes[b.typeindex].radius;
	}
}

void SweepAndPrune1DSolver::insertionSortEnds()
/* 
 * Sort m_leftSortedEnds and m_rightSortedEnds using insertion sort
 * Sorting time is roughly O(n), as these vectors remain mostly sorted each frame
 */
{
	// Insertion sort m_leftSortedEnds (increasing by left endpoint)

	for (std::size_t i = 1; i < m_leftSortedEnds.size(); i++)
	{
		for (std::size_t j = i; j > 0; j--)
		{
			if (m_leftSortedEnds[j - 1].left > m_leftSortedEnds[j].left)
				std::swap(m_leftSortedEnds[j-1], m_leftSortedEnds[j]);
			else
				break;
		}
	}

	// Insertion sort m_rightSortedEnds (decreasing by right endpoint)

	for (std::size_t i = 1; i < m_rightSortedEnds.size(); i++)
	{
		for (std::size_t j = i; j > 0; j--)
		{
			if (m_rightSortedEnds[j - 1].right < m_rightSortedEnds[j].right)
				std::swap(m_rightSortedEnds[j-1], m_rightSortedEnds[j]);
			else
				break;
		}
	}
}

void SweepAndPrune1DSolver::clearQueues()
{
	for (Queue<Endpoints>& q : m_sweepQueues)
	{
		q.clear();
	}
}