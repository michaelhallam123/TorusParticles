#include "Solver.hpp"

#include <random>
#include <cmath>

#include <iostream>
#include <string>


Solver::Solver(Preset preset)
	: m_ballTypes(preset.ballTypes), 
	  m_world(preset.worldAspectRatio)
{
	// Initialise random number generators for ball positions
	std::mt19937 gen;
	gen.seed(1963);
	std::uniform_real_distribution<float> x_posDistribution(m_world.xMin, m_world.xMax);
	std::uniform_real_distribution<float> y_posDistribution(m_world.yMin, m_world.yMax);

	// Randomly populate position and velocity data in m_balls
	for (std::size_t i = 0; i < m_ballTypes.size(); i++)
	{
		BallType& balltype = m_ballTypes[i];

		float x_meanVelocity = balltype.totalMomentum.x / (balltype.mass * (float)balltype.count);
		float y_meanVelocity = balltype.totalMomentum.y / (balltype.mass * (float)balltype.count);

		std::normal_distribution<float> x_velDistribution(x_meanVelocity, 0.12f);
		std::normal_distribution<float> y_velDistribution(y_meanVelocity, 0.12f);

		Vec2<float> runningVelocity(0.0f, 0.0f);

		for (std::size_t j = 0; j < balltype.count; j++)
		{
			float xVel = x_velDistribution(gen);
			float yVel = y_velDistribution(gen);
			Vec2<float> vel = { xVel, yVel };

			float xPos = x_posDistribution(gen);
			float yPos = y_posDistribution(gen);
			Vec2<float> pos = { xPos, yPos };

			runningVelocity.x += xVel;
			runningVelocity.y += yVel;

			Ball ball;
			ball.position = pos;
			ball.velocity = vel;
			ball.typeindex = i;

			m_balls.push_back(ball);
		}

		// Adjust final velocity so that sum of velocities of balls of type bt is totalVelocity
		m_balls.back().velocity += balltype.totalMomentum / balltype.mass - runningVelocity;
	}

	float kineticEnergy = 0.0f;

	for (std::size_t i = 1; i < m_balls.size(); i++)
	{
		kineticEnergy += 0.5f * m_ballTypes[1].mass * m_balls[i].velocity.dot(m_balls[i].velocity);
	}

	std::cout << "Total kinetic energy: " << kineticEnergy << std::endl;

	m_file.open("output.csv");

	if (!m_file.is_open())
		std::cout << "Error: could not open output.csv" << std::endl;

	m_file << "x,y\n"; // Headers of data
}

bool Solver::overlap(const Ball& ball1, const Ball& ball2)
{
	const float& radius1 = m_ballTypes[ball1.typeindex].radius;
	const float& radius2 = m_ballTypes[ball2.typeindex].radius;

	return distSquared(ball1.position, ball2.position) <= (radius1 + radius2) * (radius1 + radius2);
}


void Solver::resolveCollision(Ball& ball1, Ball& ball2)
{

	const float& mass1 = m_ballTypes[ball1.typeindex].mass;
	const float& mass2 = m_ballTypes[ball2.typeindex].mass;

	const float& radius1 = m_ballTypes[ball1.typeindex].radius;
	const float& radius2 = m_ballTypes[ball2.typeindex].radius;

	// Update velocities according to collision physics
	Vec2<float> deltaPos = ball1.position - ball2.position;
	Vec2<float> deltaVel = ball1.velocity - ball2.velocity;

	float collisionCoefficient1 = -((2.0f * mass2) / (mass1 + mass2)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float collisionCoefficient2 = -(mass1 / mass2) * collisionCoefficient1;

	ball1.velocity += deltaPos * collisionCoefficient1;
	ball2.velocity += deltaPos * collisionCoefficient2;

	// Dislodge balls to prevent sticking
	float dislodgeFactor1 = radius2 / std::sqrt(deltaPos.dot(deltaPos)) - radius2 / (radius1 + radius2);
	float dislodgeFactor2 = radius1 / std::sqrt(deltaPos.dot(deltaPos)) - radius1 / (radius1 + radius2);
	ball1.position += deltaPos * dislodgeFactor1;
	ball2.position -= deltaPos * dislodgeFactor2;
}

void Solver::update(float dt)
{
	// Check for collisions and update velocities if a collision occurs
	solve(); 

	updatePositions(dt);

	m_xPos += m_balls[0].velocity.x * dt;
	m_yPos += m_balls[0].velocity.y * dt;

	if (++m_stepCounter == m_recordSteps)
	{
		m_stepCounter = 0;
		m_file << std::to_string(m_xPos) << "," << std::to_string(m_yPos) << "\n";
	}
}

void Solver::updatePositions(float dt)
{
	for (Ball& ball : m_balls)
	{
		Vec2<float>& position = ball.position;

		position += ball.velocity * dt;

		// Normalise positions to lie within world boundaries
		if (position.x < m_world.xMin)
			position.x += m_world.xWidth;
		else if (position.x >= m_world.xMax)
			position.x -= m_world.xWidth;

		if (position.y < m_world.yMin)
			position.y += m_world.yWidth;
		else if (position.y >= m_world.yMax)
			position.y -= m_world.yWidth;
	}
}