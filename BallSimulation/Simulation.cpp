#include "Simulation.h"
#include "vec2.h"
#include <random>
#include <iostream>
#include <cmath>

Simulation::Simulation(unsigned int ballCount, float ballRadius, float dt, vec2<float> totalVelocity)
	: m_ballCount(ballCount), m_ballRadius(ballRadius), m_dt(dt)
{
	//Error checking:
	if (ballCount == 0)
		std::cout << "Error: ballCount must be a positive integer!" << std::endl;
	if (ballRadius <= 0)
		std::cout << "Error: ballRadius must be a positive float!" << std::endl;
	if (dt <= 0)
		std::cout << "Error: dt must be a positive float!" << std::endl;

	//Randomly populate position and velocity vectors:
	m_positions.reserve(ballCount);
	m_velocities.reserve(ballCount);

	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.0f, 1.0f);

	vec2<float> runningVelocity(0.0f, 0.0f);

	for (unsigned int i = 0; i < ballCount; i++)
	{

		float xVel = distribution(generator);
		float yVel = distribution(generator);
		float xPos = distribution(generator);
		float yPos = distribution(generator);

		//Normalise position to have range in [-1,1]
		xPos = (xPos - (float)(int)xPos -0.5f) * 2.0f;
		yPos = (yPos - (float)(int)yPos - 0.5f) * 2.0f;

		runningVelocity.x += xVel;
		runningVelocity.y += yVel;

		m_positions.emplace_back(xPos, yPos);
		m_velocities.emplace_back(xVel, yVel);
	}

	//Adjust final velocity so that sum of velocities is totalVelocity
	m_velocities.back().x = totalVelocity.x - runningVelocity.x;
	m_velocities.back().y = totalVelocity.y - runningVelocity.y;

}

//Constructor that takes as input a list of initial positions and velocities of balls
Simulation::Simulation(unsigned int ballCount, float ballRadius, float dt, std::vector<vec2<float>> positions, std::vector<vec2<float>> velocities)
	: m_ballCount(ballCount), m_ballRadius(ballRadius), m_dt(dt), m_positions(positions), m_velocities(velocities)
{
	//Error checking:
	if (ballCount == 0)
		std::cout << "Error: ballCount must be a positive integer!" << std::endl;
	if (ballRadius <= 0)
		std::cout << "Error: ballRadius must be a positive float!" << std::endl;
	if (dt <= 0)
		std::cout << "Error: dt must be a positive float!" << std::endl;

}

Simulation::~Simulation()
{

}

void Simulation::Update(float timeStep) //TO DO: Speed this up using hash map
{
	//Check for collisions, and update velocities using Collide() if collision found
	for (unsigned int i = 0; i < m_ballCount; i++)
	{
		for (unsigned int j = i+1; j < m_ballCount; j++)
		{
			if (distSquared(m_positions[i], m_positions[j]) <= (2.0f * m_ballRadius) * (2.0f * m_ballRadius))
				Collide(i, j);
		}
	}


	//Update positions
	for (unsigned int i = 0; i < m_ballCount; i++)
	{
		auto& p = m_positions[i];
		p.Add(m_velocities[i] * (timeStep * m_dt));

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

void Simulation::Collide(unsigned int i, unsigned int j)
{
	// Update velocities according to collision physics
	vec2<float> deltaPos = m_positions[i] - m_positions[j];
	vec2<float> deltaVel = m_velocities[i] - m_velocities[j];

	float a = - deltaPos.dot(deltaVel) / deltaPos.dot(deltaPos);

	m_velocities[i].Add(deltaPos * a);
	m_velocities[j].Add(deltaPos * (-a));

	// Dislodge balls so they don't stick together
	float dislodgeFactor = m_ballRadius / std::sqrt(deltaPos.dot(deltaPos)) - 0.5f;
	m_positions[i].Add(deltaPos * dislodgeFactor );
	m_positions[j].Add(deltaPos * (-dislodgeFactor) );
}