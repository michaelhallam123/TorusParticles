#include "BruteForceSolver/BruteForceSolver.hpp"

BruteForceSolver::BruteForceSolver(Preset preset)
	: Solver(preset) {}

void BruteForceSolver::solve()
/*
 * Check for collisions between every pair of balls
 * and update velocities when a collision is found
 */
{
	static const std::vector<Vec2<float>> displacements = 
	{ 
		{ -m_world.xWidth, -m_world.yWidth }, 
		{            0.0f, -m_world.yWidth }, 
		{  m_world.xWidth, -m_world.yWidth },
		{ -m_world.xWidth,            0.0f }, 
		{            0.0f,            0.0f }, 
		{  m_world.xWidth,            0.0f },
		{ -m_world.xWidth,  m_world.yWidth }, 
		{            0.0f,  m_world.yWidth }, 
		{  m_world.xWidth,  m_world.yWidth } 
	};

	for (std::size_t i = 0; i < m_balls.size(); i++)
	{
		Ball& b1 = m_balls[i];
		for (std::size_t j = i+1; j < m_balls.size(); j++)
		{
			Ball& b2 = m_balls[j];
			for (Vec2<float> displacement : displacements)
			{
				b1.position += displacement;
				if (overlap(b1, b2))
				{
					resolveCollision(b1, b2);
				}
				b1.position -= displacement;
			}
		}
	}
}