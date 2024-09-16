#include "PruneAndSweep1DSolver.hpp"
#include "ball.hpp"

#include <algorithm>

PruneAndSweep1DSolver::PruneAndSweep1DSolver(std::vector<balltype> ballTypes)
	: Solver(ballTypes)
{
	// Initialise m_leftSortedBalls to implement line sweep algorithm
	for (unsigned int i = 0; i < m_balls.size(); i++)
	{
		const ball& b = m_balls[i];
		float r = m_ballTypes[b.typeindex].radius;

		lineSweepData data;

		data.left  = b.position.x - r;
		data.right = b.position.x + r;
		data.ind   = i;

		m_leftSortedBalls.push_back(data);
		m_rightSortedBalls.push_back(data);
	}

	std::sort(m_leftSortedBalls.begin(),  m_leftSortedBalls.end(),  leftComparison);
	std::sort(m_rightSortedBalls.begin(), m_rightSortedBalls.end(), rightComparison);

	// Initialise sweep queues

	for (int i = 0; i < m_ballTypes.size(); i++)
	{
		m_sweepQueues.push_back({});
		m_sweepQueues.back().reserve(m_ballTypes[i].count);
		m_sweepQueuesStarts.push_back(0);
	}
}


void PruneAndSweep1DSolver::solve()
{
	insertionSortLeft();
	insertionSortRight();

	const vec2<float>  leftTranslate(-2.0f, 0.0f);
	const vec2<float> rightTranslate( 2.0f, 0.0f);

	const std::vector<vec2<float>> verticalTranslates = { {0.0f, -2.0f}, {0.0f, 0.0f}, {0.0f, 2.0f} };

	for (int i = 0; i < m_sweepQueues.size(); i++)
	{
		m_sweepQueues[i].clear();
		m_sweepQueuesStarts[i] = 0;
	}

	// First sweep (bulk of calculations, excluding left and right overlaps)

	for (auto& b1 : m_leftSortedBalls)
	{
		const unsigned int& i1 = b1.ind;
		for (int i = 0; i < m_sweepQueues.size(); i++)
		{
			auto& q = m_sweepQueues[i];
			unsigned int& start = m_sweepQueuesStarts[i];

			while (start < q.size() && (b1.left > q[start].right))
				start++;

			for (int j = start; j < q.size(); j++)
			{
				auto& b2 = q[j];

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
		m_sweepQueues[m_balls[b1.ind].typeindex].push_back(b1);
	}

	// Second sweep (deal with overlaps on right-hand side and all balls on left-hand side)
	// Note right hand overlaps are already stored in the queues after the first sweep

	float leftEnd = m_leftSortedBalls[0].left;    // potentially less than -1.0f
	float rightEnd = m_rightSortedBalls[0].right; // potentially greater than 1.0f

	for (auto& b1 : m_leftSortedBalls)
	{
		if (b1.left >= rightEnd - 2.0f)
			break;

		const unsigned int& i1 = b1.ind;

		m_balls[i1].position.Add(rightTranslate);
		b1.left += 2.0f;

		for (int i = 0; i < m_sweepQueues.size(); i++)
		{
			auto& q = m_sweepQueues[i];
			unsigned int& start = m_sweepQueuesStarts[i];

			while (start < q.size() && (b1.left > q[start].right))
				start++;

			for (int j = start; j < q.size(); j++)
			{
				auto& b2 = q[j];
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
		b1.left -= 2.0f;
	}

	// Third sweep (deal with overlaps on left-hand side and non-overlaps on right-hand side)
	// Queues are empty at this point

	// Find balls on right-hand side that do not cross boundary, but may overlap with left overlaps

	static int i = 0;
	while (i > 0 && m_rightSortedBalls[i].right < 1.0f)
		i--;
	while (m_rightSortedBalls[i].right >= 1.0f)
		i++;

	static int j = i + 1;
	std::cout << j << std::endl;
	while (m_rightSortedBalls[j].right < leftEnd + 2.0f)
		j--;
	while (m_rightSortedBalls[j].right >= leftEnd + 2.0f)
		j++;

	for (auto& b1 : m_leftSortedBalls)
	{
		if (b1.left >= 0.0f)
			break;

		const unsigned int& i1 = b1.ind;

		m_balls[i1].position.Add(rightTranslate);

		for (int k = i; k < j; k++)
		{
			auto& b2 = m_rightSortedBalls[k];

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

		m_balls[i1].position.Subtract(rightTranslate);
	}
}

void PruneAndSweep1DSolver::update(float dt)
{
	//Update positions

	Solver::update(dt);

	for (auto& b : m_leftSortedBalls)
	{
		int i = b.ind;

		vec2<float>& p = m_balls[i].position;

		b.left = p.x - m_ballTypes[m_balls[i].typeindex].radius;
		b.right = p.x + m_ballTypes[m_balls[i].typeindex].radius;

	}

	for (auto& b : m_rightSortedBalls)
	{
		int i = b.ind;

		vec2<float>& p = m_balls[i].position;

		b.left = p.x - m_ballTypes[m_balls[i].typeindex].radius;
		b.right = p.x + m_ballTypes[m_balls[i].typeindex].radius;
	}

}

void PruneAndSweep1DSolver::insertionSortLeft()
{
	for (int i = 1; i < m_leftSortedBalls.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_leftSortedBalls[j].left > m_leftSortedBalls[j + 1].left)
		{
			std::swap(m_leftSortedBalls[j], m_leftSortedBalls[j + 1]);
			j--;
		}
	}
}

void PruneAndSweep1DSolver::insertionSortRight()
{
	for (int i = 1; i < m_rightSortedBalls.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_rightSortedBalls[j].right < m_rightSortedBalls[j + 1].right)
		{
			std::swap(m_rightSortedBalls[j], m_rightSortedBalls[j + 1]);
			j--;
		}
	}
}