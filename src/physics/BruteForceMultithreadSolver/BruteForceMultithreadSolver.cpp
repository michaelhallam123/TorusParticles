#include "BruteForceMultithreadSolver/BruteForceMultithreadSolver.hpp"

#include <cmath>

BruteForceMultithreadSolver::BruteForceMultithreadSolver(Preset preset)
	: Solver(preset) {}

void BruteForceMultithreadSolver::solve()
/*
 * Check for collisions between every pair of balls
 * and update velocities when a collision is found
 */
{
	std::vector<Vec2<float>> translates =
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

	m_futures.clear();

	for (Vec2<float> translate : translates)
	{
		m_futures.push_back(
			std::async(
				std::launch::async, 
				[this, translate]() { return solveWithTranslate(translate); }
			)
		);
	}

	for (std::future<void>& future : m_futures)
		future.wait();
}

void BruteForceMultithreadSolver::solveWithTranslate(const Vec2<float>& translate)
{
	for (std::size_t i1 = 0; i1 < m_balls.size(); i1++)
	{
		Ball& b1 = m_balls[i1];
		for (std::size_t i2 = i1 + 1; i2 < m_balls.size(); i2++)
		{
			Ball& b2 = m_balls[i2];
			if (overlapWithTranslate(translate, b1, b2))
			{
				resolveCollisionWithTranslate(translate, b1, b2);
			}
		}
	}
}

void BruteForceMultithreadSolver::resolveCollisionWithTranslate(const Vec2<float>& translate, Ball& b1, Ball& b2)
{
	std::lock_guard<std::mutex> lock(m_mutex); // Prevent simultaneously editing balls in different threads

	b1.position += translate;
	
	Solver::resolveCollision(b1, b2);

	b1.position -= translate;
}


bool BruteForceMultithreadSolver::overlapWithTranslate(const Vec2<float>& translate, const Ball& b1, const Ball& b2)
{
	const Vec2<float>& p1 = b1.position;
	const Vec2<float>& p2 = b2.position;

	const float& r1 = m_ballTypes[b1.typeindex].radius;
	const float& r2 = m_ballTypes[b2.typeindex].radius;

	return distSquared(p1 + translate, p2) <= (r1 + r2) * (r1 + r2);
}