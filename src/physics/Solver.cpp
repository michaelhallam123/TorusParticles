#include "Solver.hpp"

#include <random>
#include <cmath>

Solver::Solver(Preset preset)
	: m_ballTypes(preset.ballTypes), 
	  m_world(preset.worldAspectRatio)
{
	// Initialise random number generators for ball positions
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> x_posDistribution(m_world.xMin, m_world.xMax);
	std::uniform_real_distribution<float> y_posDistribution(m_world.yMin, m_world.yMax);

	// Randomly populate position and velocity data in m_balls
	for (std::size_t i = 0; i < m_ballTypes.size(); i++)
	{
		BallType& bt = m_ballTypes[i];

		float x_meanVelocity = bt.totalMomentum.x / (bt.mass * (float)bt.count);
		float y_meanVelocity = bt.totalMomentum.y / (bt.mass * (float)bt.count);

		std::normal_distribution<float> x_velDistribution(x_meanVelocity, 0.12f);
		std::normal_distribution<float> y_velDistribution(y_meanVelocity, 0.12f);

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

bool Solver::overlap(const Ball& b1, const Ball& b2)
{
	const float& r1 = m_ballTypes[b1.typeindex].radius;
	const float& r2 = m_ballTypes[b2.typeindex].radius;

	return distSquared(b1.position, b2.position) <= (r1 + r2) * (r1 + r2);
}


void Solver::resolveCollision(Ball& b1, Ball& b2)
{

	const float& m1 = m_ballTypes[b1.typeindex].mass;
	const float& m2 = m_ballTypes[b2.typeindex].mass;

	const float& r1 = m_ballTypes[b1.typeindex].radius;
	const float& r2 = m_ballTypes[b2.typeindex].radius;

	// Update velocities according to collision physics
	Vec2<float> deltaPos = b1.position - b2.position;
	Vec2<float> deltaVel = b1.velocity - b2.velocity;

	float collisionCoefficient1 = -((2.0f * m2) / (m1 + m2)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float collisionCoefficient2 = -(m1 / m2) * collisionCoefficient1;

	b1.velocity.add(deltaPos * collisionCoefficient1);
	b2.velocity.add(deltaPos * collisionCoefficient2);

	// Dislodge balls to prevent sticking
	float dislodgeFactor1 = r2 / std::sqrt(deltaPos.dot(deltaPos)) - r2/(r1+r2);
	float dislodgeFactor2 = r1 / std::sqrt(deltaPos.dot(deltaPos)) - r1/(r1+r2);
	b1.position += deltaPos * dislodgeFactor1;
	b2.position -= deltaPos * dislodgeFactor2;
}

void Solver::update(float dt)
{
	// Check for collisions and update velocities if a collision occurs
	solve(); 

	updatePositions(dt);
}

void Solver::updatePositions(float dt)
{
	for (Ball& b : m_balls)
	{
		Vec2<float>& position = b.position;

		position += b.velocity * dt;

		// Normalise positions to lie within world boundaries
		if (position.x < m_world.xMin)
			position.x += m_world.xWidth;
		else if (position.x > m_world.xMax)
			position.x -= m_world.xWidth;

		if (position.y < m_world.yMin)
			position.y += m_world.yWidth;
		else if (position.y > m_world.yMax)
			position.y -= m_world.yWidth;
	}
}