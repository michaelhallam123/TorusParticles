#include "Solver.hpp"

#include <random>
#include <iostream>

Solver::Solver(std::vector<balltype> ballTypes)
	: m_ballTypes(ballTypes)
{
	// TO DO: Error checking (parameters within correct bounds, e.g. mass > 0)

	// Initialise random number generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> posDistribution(-1.0f, 1.0f);

	// Randomly populate position and velocity data in m_balls
	for (unsigned int i = 0; i < ballTypes.size(); i++)
	{
		balltype& bt = ballTypes[i];

		float x_meanVelocity = bt.totalMomentum.x / (bt.mass * (float)bt.count);
		float y_meanVelocity = bt.totalMomentum.y / (bt.mass * (float)bt.count);

		std::normal_distribution<float> x_velDistribution(x_meanVelocity, 1.0f);
		std::normal_distribution<float> y_velDistribution(y_meanVelocity, 1.0f);

		vec2<float> runningVelocity(0.0f, 0.0f);

		for (unsigned int j = 0; j < bt.count; j++)
		{
			float xVel = x_velDistribution(gen);
			float yVel = y_velDistribution(gen);
			vec2<float> vel = { xVel, yVel };

			float xPos = posDistribution(gen);
			float yPos = posDistribution(gen);
			vec2<float> pos = { xPos, yPos };

			runningVelocity.x += xVel;
			runningVelocity.y += yVel;

			ball b;
			b.position = pos;
			b.velocity = vel;
			b.typeindex = i;

			m_balls.push_back(b);
		}

		// Adjust final velocity so that sum of velocities of balls of type bt is totalVelocity
		m_balls.back().velocity += bt.totalMomentum / bt.mass - runningVelocity;

	}
}

bool Solver::overlap(unsigned int i, unsigned int j)
{
	const ball b1 = m_balls[i];
	const ball b2 = m_balls[j];

	float r1 = m_ballTypes[b1.typeindex].radius;
	float r2 = m_ballTypes[b2.typeindex].radius;


	return distSquared(b1.position, b2.position) <= (r1 + r2) * (r1 + r2);
}


void Solver::resolveCollision(unsigned int i, unsigned int j)
{
	ball& b1 = m_balls[i];
	ball& b2 = m_balls[j];

	float m1 = m_ballTypes[b1.typeindex].mass;
	float m2 = m_ballTypes[b2.typeindex].mass;

	float r1 = m_ballTypes[b1.typeindex].radius;
	float r2 = m_ballTypes[b2.typeindex].radius;

	// Update velocities according to collision physics

	vec2<float> deltaPos = b1.position - b2.position;
	vec2<float> deltaVel = b1.velocity - b2.velocity;

	float a = -((2.0f * m2) / (m1 + m2)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float b = -(m1 / m2) * a;

	b1.velocity.Add(deltaPos * a);
	b2.velocity.Add(deltaPos * b);

	// Dislodge balls to prevent sticking

	float dislodgeFactor = 0.5f * (r1 + r2) / std::sqrt(deltaPos.dot(deltaPos)) - 0.5f;
	b1.position.Add(deltaPos * dislodgeFactor);
	b2.position.Subtract(deltaPos * dislodgeFactor);
}

void Solver::update(float dt)
{
	// Check for collisions and update velocities

	solve(); 

	// Update positions

	for (ball& b : m_balls)
	{
		b.position += b.velocity * dt;

		// Normalise positions to lie in [-1,1]x[-1,1]

		vec2<float>& p = b.position;
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