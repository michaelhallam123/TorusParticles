#include "Simulation.h"
#include "vec2.h"
#include <random>
#include <iostream>
#include <cmath>

Simulation::Simulation(unsigned int ballCount, float smallRadius, float bigRadius, float smallMass, float bigMass, float dt, vec2<float> totalVelocity)
	: m_ballCount(ballCount), m_smallRadius(smallRadius), m_bigRadius(bigRadius),
	  m_smallMass(smallMass), m_bigMass(bigMass), m_bigPosition({ 0.0f, 0.0f }), m_dt(dt)
{
	// Error checking
	if (ballCount == 0)
		std::cout << "Error: ballCount must be a positive integer!" << std::endl;
	if (smallRadius <= 0)
		std::cout << "Error: smallRadius must be a positive float!" << std::endl;
	if (bigRadius <= 0)
		std::cout << "Error: bigRadius must be a positive float!" << std::endl;
	if (dt <= 0)
		std::cout << "Error: dt must be a positive float!" << std::endl;

	// Randomly populate position and velocity vectors
	m_smallPositions.reserve(ballCount);
	m_smallVelocities.reserve(ballCount);

	float x_meanVelocity = totalVelocity.x / (float)ballCount;
	float y_meanVelocity = totalVelocity.y / (float)ballCount;

	// Initialise random number generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> posDistribution(-1.0f, 1.0f);
	std::normal_distribution<float> x_velDistribution(x_meanVelocity, 1.0f);
	std::normal_distribution<float> y_velDistribution(y_meanVelocity, 1.0f);

	vec2<float> runningVelocity(0.0f, 0.0f);

	for (unsigned int i = 0; i < ballCount; i++)
	{
		// Generate velocity and positions of small balls
		float xVel = x_velDistribution(gen);
		float yVel = y_velDistribution(gen);
		vec2<float> vel = { xVel, yVel };

		float xPos = posDistribution(gen);
		float yPos = posDistribution(gen);

		vec2<float> pos = { xPos, yPos };

		// Ensure small balls do not spawn inside large ball
		while (pos.dot(pos) < (m_bigRadius + m_smallRadius) * (m_bigRadius + m_smallRadius))
		{
			xPos = posDistribution(gen);
			yPos = posDistribution(gen);
			pos = { xPos, yPos };
		}

		runningVelocity.x += xVel;
		runningVelocity.y += yVel;

		m_smallPositions.push_back(pos);
		m_smallVelocities.push_back(vel);
	}

	//Adjust final velocity so that sum of velocities is totalVelocity
	m_smallVelocities.back().x = totalVelocity.x - runningVelocity.x;
	m_smallVelocities.back().y = totalVelocity.y - runningVelocity.y;

}


Simulation::~Simulation()
{

}

void Simulation::Update(float timeStep) // TO DO: Speed this up
{
	// Check for collisions between small balls, and update velocities accordingly
	for (unsigned int i = 0; i < m_ballCount; i++)
	{
		for (unsigned int j = i+1; j < m_ballCount; j++)
		{
			if (distSquared(m_smallPositions[i], m_smallPositions[j]) <= (2.0f * m_smallRadius) * (2.0f * m_smallRadius))
				collideSmalls(i, j);
		}
	}

	// Check for collisions between large and small balls, and update velocities accordingly
	for (unsigned int i = 0; i < m_ballCount; i++)
	{
		if (distSquared(m_bigPosition, m_smallPositions[i]) <= (m_smallRadius + m_bigRadius) * (m_smallRadius + m_bigRadius))
		{
			collideBig(i);
		}
	}


	//Update positions
	for (unsigned int i = 0; i < m_ballCount; i++)
	{
		vec2<float>& p = m_smallPositions[i];
		p.Add(m_smallVelocities[i] * (timeStep * m_dt));

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

	vec2<float>& p = m_bigPosition;
	p.Add(m_bigVelocity * (timeStep * m_dt));

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

void Simulation::collideSmalls(unsigned int i, unsigned int j)
{
	// Update velocities according to collision physics
	vec2<float> deltaPos = m_smallPositions[i] - m_smallPositions[j];
	vec2<float> deltaVel = m_smallVelocities[i] - m_smallVelocities[j];

	float a = - deltaPos.dot(deltaVel) / deltaPos.dot(deltaPos);

	m_smallVelocities[i].Add(deltaPos * a);
	m_smallVelocities[j].Add(deltaPos * (-a));

	// Dislodge balls so they don't stick together
	float dislodgeFactor = m_smallRadius / std::sqrt(deltaPos.dot(deltaPos)) - 0.5f;
	m_smallPositions[i].Add(deltaPos * dislodgeFactor );
	m_smallPositions[j].Add(deltaPos * (-dislodgeFactor) );
}

void Simulation::collideBig(unsigned int i)
{
	// Update velocities according to collision physics
	vec2<float> deltaPos = m_smallPositions[i] - m_bigPosition;
	vec2<float> deltaVel = m_smallVelocities[i] - m_bigVelocity;

	float a = -((2.0f * m_bigMass) / (m_bigMass + m_smallMass)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float b = -(m_smallMass / m_bigMass) * a;

	m_smallVelocities[i].Add(deltaPos * a);
	m_bigVelocity.Add(deltaPos * b);

	// Dislodge balls so they don't stick together
	float dislodgeFactor = (m_smallRadius + m_bigRadius) / std::sqrt(deltaPos.dot(deltaPos)) - 1.0f;
	m_smallPositions[i].Add(deltaPos * dislodgeFactor);
}