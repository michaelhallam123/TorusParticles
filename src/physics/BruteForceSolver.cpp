#include "BruteForceSolver.hpp"

BruteForceSolver::BruteForceSolver(std::vector<BallType> ballTypes, float worldAspectRatio)
	: Solver(ballTypes, worldAspectRatio) {}

void BruteForceSolver::solve()
/*
 * Check for collisions between every pair of balls
 * and update velocities when a collision is found
 */
{

	std::vector<Vec2<float>> displacements = 
	{ { -m_world.xWidth, -m_world.yWidth }, {  0.0f, -m_world.yWidth }, {  m_world.xWidth, -m_world.yWidth },
	  { -m_world.xWidth,  0.0f           }, {  0.0f,  0.0f           }, {  m_world.xWidth,  0.0f           },
	  { -m_world.xWidth,  m_world.yWidth }, {  0.0f,  m_world.yWidth }, {  m_world.xWidth,  m_world.yWidth } };

	for (std::size_t i = 0; i < m_balls.size(); i++)
	{
		for (std::size_t j = i+1; j < m_balls.size(); j++)
		{
			for (Vec2<float> displacement : displacements)
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