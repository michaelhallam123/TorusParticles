#include "Simulation.h"
#include "vec2.h"
#include <random>
#include <iostream>
#include <cmath>

// Constructor that randomly populations positions and velocities of balls according to user specification
Simulation::Simulation(std::vector<ball> ballData, float dt)
	: m_ballData(ballData), m_dt(dt)
{
	// TO DO: Error checking (parameters within correct bounds, e.g. mass > 0)

	// Reserve space in vectors for balls
	unsigned int ballTypesCount = m_ballData.size();
	m_positions.reserve(ballTypesCount);
	m_velocities.reserve(ballTypesCount);

	// Initialise random number generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> posDistribution(-1.0f, 1.0f);

	// Populate position and velocity data
	for (int i = 0; i < ballTypesCount; i++)
	{
		m_positions.push_back({});
		m_velocities.push_back({});

		const ball& b = ballData[i];
		m_positions[i].reserve(b.count);
		m_velocities[i].reserve(b.count);

		float x_meanVelocity = b.totalMomentum.x / (b.mass * (float)b.count);
		float y_meanVelocity = b.totalMomentum.y / (b.mass * (float)b.count);
		std::normal_distribution<float> x_velDistribution(x_meanVelocity, 1.0f);
		std::normal_distribution<float> y_velDistribution(y_meanVelocity, 1.0f);

		vec2<float> runningVelocity(0.0f, 0.0f);

		for (unsigned int j = 0; j < b.count; j++)
		{
			float xVel = x_velDistribution(gen);
			float yVel = y_velDistribution(gen);
			vec2<float> vel = { xVel, yVel };

			float xPos = posDistribution(gen);
			float yPos = posDistribution(gen);
			vec2<float> pos = { xPos, yPos };

			runningVelocity.x += xVel;
			runningVelocity.y += yVel;

			m_positions[i].push_back(pos);
			m_velocities[i].push_back(vel);
		}

		// Adjust final velocity so that sum of velocities is totalVelocity
		m_velocities[i].back().x += b.totalMomentum.x / b.mass - runningVelocity.x;
		m_velocities[i].back().y += b.totalMomentum.y / b.mass - runningVelocity.y;
	}

	// Initialise m_sortedSweepData to implement line sweep algorithm
	for (unsigned int i = 0; i < m_ballData.size(); i++)
	{
		m_sortedSweepData.push_back({});

		for (int j = 0; j < m_positions[i].size(); j++)
		{
			lineSweepData data;

			data.left = m_positions[i][j].x - m_ballData[i].radius;
			data.right = m_positions[i][j].x + m_ballData[i].radius;
			data.ballTypeInd = i;
			data.vecInd = j;

			m_sortedSweepData.push_back(data);
		}
	}

	std::sort(m_sortedSweepData.begin(), m_sortedSweepData.end(), leftComparison);

	// Create space for m_sweepQueues
	for (int i = 0; i < m_ballData.size(); i++)
		m_sweepQueues.push_back({});

}

// Check for collisions, update velocities of colliding balls, then update positions of balls
void Simulation::Update(float timeStep)
{
	// Check for collisions using line sweep algorithm
	// If collision is found, update velocities using collide() method

	insertionSort();
	
	for (int i = 0; i < m_ballData.size(); i++)
		m_sweepQueues[i].clear();

	for (auto& b1 : m_sortedSweepData)
	{
		for (int i = 0; i < m_sweepQueues.size(); i++)
		{
			auto& q = m_sweepQueues[i];


			while (!q.empty() && (b1.left > q.front().right))
				q.pop_front();


			for (auto& b2 : q)
			{
				const unsigned int& i1 = b1.ballTypeInd;
				const unsigned int& i2 = b2.ballTypeInd;

				const unsigned int& j1 = b1.vecInd;
				const unsigned int& j2 = b2.vecInd;

				const float& r1 = m_ballData[i1].radius;
				const float& r2 = m_ballData[i2].radius;

				if (distSquared(m_positions[i1][j1], m_positions[i2][j2]) <= (r1 + r2) * (r1 + r2))
				{
					collide(i1, j1, i2, j2);
				}
			}
		}

		m_sweepQueues[b1.ballTypeInd].push_back(b1);
	}

	/*
	// Check for collisions and update velocities
	for (unsigned int i1 = 0; i1 < m_ballData.size(); i1++)
	{
		const ball& b1 = m_ballData[i1];
		for (unsigned int j1 = 0; j1 < m_positions[i1].size(); j1++)
		{
			// Check collisions with balls of same type
			for (unsigned int j2 = j1 + 1; j2 < m_positions[i1].size(); j2++)
			{
				if (distSquared(m_positions[i1][j1], m_positions[i1][j2]) <= (2.0f * b1.radius) * (2.0f * b1.radius))
					collide(i1, j1, i1, j2);
			}

			// Check collisions with balls of different type
			for (unsigned int i2 = i1 + 1; i2 < m_ballData.size(); i2++)
			{
				const ball& b2 = m_ballData[i2];
				for (int j2 = 0; j2 < m_positions[i2].size(); j2++)
				{
					if (distSquared(m_positions[i1][j1], m_positions[i2][j2]) <= (b1.radius + b2.radius) * (b1.radius + b2.radius))
						collide(i1, j1, i2, j2);
				}
			}
		}
	}
	*/

	//Update positions

	for (auto& b : m_sortedSweepData)
	{
		int i = b.ballTypeInd;
		int j = b.vecInd;

		vec2<float>& p = m_positions[i][j];
		p.Add(m_velocities[i][j] * (timeStep * m_dt));

		// Normalise positions to lie in [-1,1]x[-1,1]
		if (p.x < -1.0f)
			p.x += 2.0f;
		else if (p.x > 1.0f)
			p.x -= 2.0f;

		if (p.y < -1.0f)
			p.y += 2.0f;
		else if (p.y > 1.0f)
			p.y -= 2.0f;

		b.left = p.x - m_ballData[i].radius;
		b.right = p.x + m_ballData[i].radius;

	}
}

void Simulation::insertionSort()
{
	for (int i = 0; i < m_sortedSweepData.size(); i++)
	{
		int j = i - 1;

		while (j >= 0 && m_sortedSweepData[j].left > m_sortedSweepData[j+1].left)
		{
			std::swap(m_sortedSweepData[j], m_sortedSweepData[j + 1]);
			j--;
		}
	}
}

// Update velocities of balls that are colliding
void Simulation::collide(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2)
{
	const ball& b1 = m_ballData[i1];
	const ball& b2 = m_ballData[i2];

	// Update velocities according to collision physics
	vec2<float> deltaPos = m_positions[i1][j1] - m_positions[i2][j2];
	vec2<float> deltaVel = m_velocities[i1][j1] - m_velocities[i2][j2];

	float a = -((2.0f * b2.mass) / (b1.mass + b2.mass)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float b = -(b1.mass / b2.mass) * a;

	m_velocities[i1][j1].Add(deltaPos * a);
	m_velocities[i2][j2].Add(deltaPos * b);

	// Dislodge balls so they don't stick together
	float dislodgeFactor =  (b1.radius + b2.radius) / std::sqrt(deltaPos.dot(deltaPos)) - 1.0f;
	m_positions[i1][j1].Add(deltaPos * dislodgeFactor);
}