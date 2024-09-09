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
		m_velocities[i].back().x = b.totalMomentum.x / b.mass - runningVelocity.x;
		m_velocities[i].back().y = b.totalMomentum.y / b.mass - runningVelocity.y;
	}
}

// Check for collisions, update velocities of colliding balls, then update positions of balls
void Simulation::Update(float timeStep)
{
	// Loop pretty ugly now, but implementing a proper algorithm should make this cleaner
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

	//Update positions
	for (unsigned int i1 = 0; i1 < m_ballData.size(); i1++)
	{
		for (unsigned int j1 = 0; j1 < m_positions[i1].size(); j1++)
		{
			vec2<float>& p = m_positions[i1][j1];
			p.Add(m_velocities[i1][j1] * (timeStep * m_dt));

			// Normalise positions to lie in [-1,1]x[-1,1]
			if (p.x < -1.0f)
				p.x += 2.0f;
			else if (p.x > 1.0f)
				p.x -= 2.0f;

			if (p.y < -1.0f)
				p.y += 2.0f;
			else if (p.y > 1.0f)
				p.y -= 2.0f;
		}
	}
}

// Update velocities of balls that are colliding
void Simulation::collide(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2)
{
	const ball& b1 = m_ballData[i1];
	const ball& b2 = m_ballData[i2];

	// Small becomes i1, j1, big becomes i2, j2
	// Update velocities according to collision physics
	vec2<float> deltaPos = m_positions[i1][j1] - m_positions[i2][j2];
	vec2<float> deltaVel = m_velocities[i1][j1] - m_velocities[i2][j2];

	float a = -((2.0f * b2.mass) / (b1.mass + b2.mass)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float b = -(b1.mass / b2.mass) * a;

	m_velocities[i1][j1].Add(deltaPos * a);
	m_velocities[i2][j2].Add(deltaPos * b);

	// Dislodge balls so they don't stick together
	float dislodgeFactor = 0.5f * (b1.radius + b2.radius) / std::sqrt(deltaPos.dot(deltaPos)) - 0.5f;
	m_positions[i1][j1].Add(deltaPos * dislodgeFactor);
	m_positions[i2][j2].Subtract(deltaPos * dislodgeFactor);
}