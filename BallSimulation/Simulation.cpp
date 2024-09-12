/*

#include "Simulation.h"
#include "vec2.h"
#include <random>
#include <iostream>
#include <cmath>

// Constructor that randomly populations positions and velocities of balls according to user specification
Simulation::Simulation(std::vector<ball> ballData, float dt)
	: m_ballData(ballData), m_dt(dt)
{

	// Initialise m_leftSortedBalls to implement line sweep algorithm
	for (unsigned int i = 0; i < m_ballData.size(); i++)
	{

		for (int j = 0; j < m_positions[i].size(); j++)
		{
			lineSweepData data;

			data.left = m_positions[i][j].x - m_ballData[i].radius;
			data.right = m_positions[i][j].x + m_ballData[i].radius;
			data.ballTypeInd = i;
			data.vecInd = j;

			m_leftSortedBalls.push_back(data);
			m_rightSortedBalls.push_back(data);
		}
	}

	std::sort(m_leftSortedBalls.begin(), m_leftSortedBalls.end(), leftComparison);
	std::sort(m_rightSortedBalls.begin(), m_rightSortedBalls.end(), rightComparison);

	// Create space for m_sweepQueues
	for (int i = 0; i < m_ballData.size(); i++)
		m_sweepQueues.push_back({});
}

// Check for collisions, update velocities of colliding balls, then update positions of balls
void Simulation::Update(float timeStep)
{
	
	// Check for collisions using line sweep algorithm
	// If collision is found, update velocities using collide() method

	insertionSortLeft();
	insertionSortRight();
	
	for (int i = 0; i < m_ballData.size(); i++)
		m_sweepQueues[i].clear();

	vec2<float>  leftTranslate(-2.0f,  0.0f);
	vec2<float> rightTranslate( 2.0f,  0.0f);


	for (auto& b1 : m_leftSortedBalls)
	{
		const unsigned int& i1 = b1.ballTypeInd;
		const unsigned int& j1 = b1.vecInd;
		for (int i = 0; i < m_sweepQueues.size(); i++)
		{

			auto& q = m_sweepQueues[i];

			while (!q.empty() && (b1.left > q.front().right))
				q.pop_front();

			for (auto& b2 : q)
			{
				
				const unsigned int& i2 = b2.ballTypeInd;
				const unsigned int& j2 = b2.vecInd;


				checkCollision(i1, j1, i2, j2);
			}
		}

		m_sweepQueues[b1.ballTypeInd].push_back(b1);
	}

	float leftEnd = m_leftSortedBalls[0].left; // < -1.0f
	float rightEnd = m_rightSortedBalls[0].right; // > 1.0f

	for (auto& b1 : m_leftSortedBalls)
	{
		if (b1.left >= rightEnd - 2.0f)
			break;

		const unsigned int& i1 = b1.ballTypeInd;
		const unsigned int& j1 = b1.vecInd;

		m_positions[i1][j1].Add(rightTranslate);

		for (int i = 0; i < m_sweepQueues.size(); i++)
		{

			auto& q = m_sweepQueues[i];

			while (!q.empty() && (m_positions[i1][j1].x > q.front().right))
				q.pop_front();

			for (auto& b2 : q)
			{

				const unsigned int& i2 = b2.ballTypeInd;
				const unsigned int& j2 = b2.vecInd;


				checkCollision(i1, j1, i2, j2);

			}
		}
		m_positions[i1][j1].Subtract(rightTranslate);
	}

	
	int i = 0;
	while (m_rightSortedBalls[i].right >= 1.0f)
		i++;

	int j = i + 1;
	while (m_rightSortedBalls[j].right - 2.0f >= leftEnd)
		j++;

	for (auto& b1 : m_leftSortedBalls)
	{
		if (b1.left >= 0.0f)
			break;

		const unsigned int& i1 = b1.ballTypeInd;
		const unsigned int& j1 = b1.vecInd;

		m_positions[i1][j1].Add(rightTranslate);

		for (int k = i; k < j; k++)
		{
			auto& b2 = m_rightSortedBalls[k];

			const unsigned int& i2 = b2.ballTypeInd;
			const unsigned int& j2 = b2.vecInd;

			checkCollision(i1, j1, i2, j2);
		}

		m_positions[i1][j1].Subtract(rightTranslate);
	}

	//Update positions

	for (auto& b : m_leftSortedBalls)
	{
		int i = b.ballTypeInd;
		int j = b.vecInd;

		vec2<float>& p = m_positions[i][j];
		p.Add(m_velocities[i][j] * (timeStep * m_dt));

		b.left = p.x - m_ballData[i].radius;
		b.right = p.x + m_ballData[i].radius;

	}

	for (auto& b : m_rightSortedBalls)
	{
		int i = b.ballTypeInd;
		int j = b.vecInd;

		vec2<float>& p = m_positions[i][j];
		b.left = p.x - m_ballData[i].radius;
		b.right = p.x + m_ballData[i].radius;
	}

}

void Simulation::checkCollision(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2)
{

	vec2<float>    upTranslate(0.0f, 2.0f);
	vec2<float>  downTranslate(0.0f, -2.0f);

	const float r1 = m_ballData[i1].radius;
	const float r2 = m_ballData[i2].radius;

	if (distSquared(m_positions[i1][j1], m_positions[i2][j2]) <= (r1 + r2) * (r1 + r2))
	{
		collide(i1, j1, i2, j2);
	}
	else if (distSquared(m_positions[i1][j1] + upTranslate, m_positions[i2][j2]) <= (r1 + r2) * (r1 + r2))
	{
		m_positions[i1][j1].Add(upTranslate);
		collide(i1, j1, i2, j2);
		m_positions[i1][j1].Subtract(upTranslate);
	}
	else if (distSquared(m_positions[i1][j1] + downTranslate, m_positions[i2][j2]) <= (r1 + r2) * (r1 + r2))
	{
		m_positions[i1][j1].Add(downTranslate);
		collide(i1, j1, i2, j2);
		m_positions[i1][j1].Subtract(downTranslate);
	}
}

void Simulation::insertionSortLeft()
{
	for (int i = 0; i < m_leftSortedBalls.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_leftSortedBalls[j].left > m_leftSortedBalls[j+1].left)
		{
			std::swap(m_leftSortedBalls[j], m_leftSortedBalls[j + 1]);
			j--;
		}
	}
}

void Simulation::insertionSortRight()
{
	for (int i = 0; i < m_rightSortedBalls.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_rightSortedBalls[j].right < m_rightSortedBalls[j + 1].right)
		{
			std::swap(m_rightSortedBalls[j], m_rightSortedBalls[j + 1]);
			j--;
		}
	}
}

*/