#include "BruteForceSolver.hpp"

BruteForceSolver::BruteForceSolver(std::vector<balltype> ballTypes)
	: Solver(ballTypes) {}

void BruteForceSolver::solve()
/*
 * Check for collisions between every pair of balls
 * and update velocities when a collision is found
 */
{

	std::vector<vec2<float>> displacements = { { -xWorldWidth, -yWorldWidth }, {  0.0f, -yWorldWidth }, {  xWorldWidth, -yWorldWidth },
											   { -xWorldWidth,  0.0f        }, {  0.0f,  0.0f        }, {  xWorldWidth,  0.0f        },
											   { -xWorldWidth,  yWorldWidth }, {  0.0f,  yWorldWidth }, {  xWorldWidth,  yWorldWidth } };

	for (unsigned int i = 0; i < m_balls.size(); i++)
	{
		for (unsigned int j = i+1; j < m_balls.size(); j++)
		{
			for (vec2<float> displacement : displacements)
			{
				m_balls[i].position += displacement;
				if (overlap(i, j))
				{
					resolveCollision(i, j);
				}
				m_balls[i].position -= displacement;
			}
		}
	}
}