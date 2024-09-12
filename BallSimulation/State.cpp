#include "State.hpp"
#include <random>

// TO DO: ensure that no balls are overlapping in the initial state: move small balls away from large balls

State::State(std::vector<balltype> ballTypes)
	: ballTypes(ballTypes)
{
	// TO DO: Error checking (parameters within correct bounds, e.g. mass > 0)

	// Initialise random number generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> posDistribution(-1.0f, 1.0f);

	// Randomly populate position and velocity data in m_balls
	for ( unsigned int i = 0; i < ballTypes.size(); i++ )
	{
		balltype& bt = ballTypes[i];

		float x_meanVelocity = bt.totalMomentum.x / (bt.mass * (float)bt.count);
		float y_meanVelocity = bt.totalMomentum.y / (bt.mass * (float)bt.count);

		std::normal_distribution<float> x_velDistribution(x_meanVelocity, 1.0f);
		std::normal_distribution<float> y_velDistribution(y_meanVelocity, 1.0f);

		vec2<float> runningVelocity(0.0f, 0.0f);

		for ( unsigned int j = 0; j < bt.count; j++ )
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

			balls.push_back(b);
		}

		// Adjust final velocity so that sum of velocities of balls of type bt is totalVelocity
		balls.back().velocity += bt.totalMomentum / bt.mass - runningVelocity;
	}
}