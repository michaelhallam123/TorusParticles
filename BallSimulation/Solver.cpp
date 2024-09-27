#include "Solver.hpp"

#include <random>
#include <iostream>
#include <cmath>

Solver::Solver(std::vector<BallType> ballTypes)
	: m_ballTypes(ballTypes), 
	  m_world(-1.0f, 1.0f, -1.0f, 1.0f) // Takes world boundary values: xMin, xMax, yMin, yMax
{
	// Error checking

	for (BallType& bt : m_ballTypes)
	{
		if (bt.mass <= 0.0f)
			std::cout << "Warning: ball mass must be positive!" << std::endl;
		if (bt.radius <= 0.0f)
			std::cout << "Warning: ball radius must be positive!" << std::endl;

	}

	// Initialise random number generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> x_posDistribution(m_world.xMin, m_world.xMax);
	std::uniform_real_distribution<float> y_posDistribution(m_world.yMin, m_world.yMax);

	// Randomly populate position and velocity data in m_balls
	for (std::size_t i = 0; i < ballTypes.size(); i++)
	{
		BallType& bt = ballTypes[i];

		float x_meanVelocity = bt.totalMomentum.x / (bt.mass * (float)bt.count);
		float y_meanVelocity = bt.totalMomentum.y / (bt.mass * (float)bt.count);

		std::normal_distribution<float> x_velDistribution(x_meanVelocity, 0.1f);
		std::normal_distribution<float> y_velDistribution(y_meanVelocity, 0.1f);

		Vec2<float> runningVelocity(0.0f, 0.0f);

		for (std::size_t j = 0; j < bt.count; j++)
		{
			float xVel = x_velDistribution(gen);
			float yVel = y_velDistribution(gen);
			Vec2<float> vel = { xVel, yVel };

			float xPos = x_posDistribution(gen);
			float yPos = y_posDistribution(gen);
			Vec2<float> pos = { xPos, yPos };

			runningVelocity.x += xVel;
			runningVelocity.y += yVel;

			Ball b;
			b.position = pos;
			b.velocity = vel;
			b.typeindex = i;

			m_balls.push_back(b);
		}

		// Adjust final velocity so that sum of velocities of balls of type bt is totalVelocity
		m_balls.back().velocity += bt.totalMomentum / bt.mass - runningVelocity;

	}
}

bool Solver::overlap(std::size_t i, std::size_t j)
{
	const Ball b1 = m_balls[i];
	const Ball b2 = m_balls[j];

	float r1 = m_ballTypes[b1.typeindex].radius;
	float r2 = m_ballTypes[b2.typeindex].radius;

	return distSquared(b1.position, b2.position) <= (r1 + r2) * (r1 + r2);
}


void Solver::resolveCollision(std::size_t i, std::size_t j)
{
	Ball& b1 = m_balls[i];
	Ball& b2 = m_balls[j];

	float m1 = m_ballTypes[b1.typeindex].mass;
	float m2 = m_ballTypes[b2.typeindex].mass;

	float r1 = m_ballTypes[b1.typeindex].radius;
	float r2 = m_ballTypes[b2.typeindex].radius;

	// Update velocities according to collision physics

	Vec2<float> deltaPos = b1.position - b2.position;
	Vec2<float> deltaVel = b1.velocity - b2.velocity;

	float a = -((2.0f * m2) / (m1 + m2)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float b = -(m1 / m2) * a;

	b1.velocity.add(deltaPos * a);
	b2.velocity.add(deltaPos * b);

	// Dislodge balls to prevent sticking

	float dislodgeFactor1 = r2 / std::sqrt(deltaPos.dot(deltaPos)) - r2/(r1+r2);
	float dislodgeFactor2 = r1 / std::sqrt(deltaPos.dot(deltaPos)) - r1/(r1+r2);
	b1.position.add(deltaPos * dislodgeFactor1);
	b2.position.subtract(deltaPos * dislodgeFactor2);
}

void Solver::update(float dt)
{
	// Check for collisions and update velocities
	solve(); 

	// Update positions
	updatePositions(dt);
}

/*
Vec2<float> acceleration(Vec2<float> position, float time)
{
	return { 40.0f*position.x, 0.0f };
}
*/

void Solver::updatePositions(float dt)
{
	static float time = 0;
	time += dt;
	for (Ball& b : m_balls)
	{
		b.position += b.velocity * dt;
		//b.velocity += acceleration(b.position, time) * dt;

		// Normalise positions to lie within world boundaries

		Vec2<float>& p = b.position;

		if (p.x < m_world.xMin)
			p.x += m_world.xWidth;
		else if (p.x > m_world.xMax)
			p.x -= m_world.xWidth;

		if (p.y < m_world.yMin)
			p.y += m_world.yWidth;
		else if (p.y > m_world.yMax)
			p.y -= m_world.yWidth;
	}
}