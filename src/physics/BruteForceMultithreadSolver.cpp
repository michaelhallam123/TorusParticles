#include "BruteForceMultithreadSolver.hpp"

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
	{ { -m_world.xWidth, -m_world.yWidth }, {  0.0f, -m_world.yWidth }, {  m_world.xWidth, -m_world.yWidth },
	  { -m_world.xWidth,  0.0f           }, {  0.0f,  0.0f           }, {  m_world.xWidth,  0.0f           },
	  { -m_world.xWidth,  m_world.yWidth }, {  0.0f,  m_world.yWidth }, {  m_world.xWidth,  m_world.yWidth } };

	m_futures.clear();
	for (Vec2<float> translate : translates)
	{
		m_futures.push_back(std::async(std::launch::async, [this, translate]() { return solveWithTranslate(translate); }));
	}

	for (std::future<void>& future : m_futures)
		future.wait();
}

void BruteForceMultithreadSolver::solveWithTranslate(const Vec2<float>& translate)
{
	for (std::size_t i = 0; i < m_balls.size(); i++)
	{
		for (std::size_t j = i + 1; j < m_balls.size(); j++)
		{
			if (overlapWithTranslate(translate, i, j))
			{
				resolveCollisionWithTranslate(translate, i, j);
			}
		}
	}
}

void BruteForceMultithreadSolver::resolveCollisionWithTranslate(const Vec2<float>& translate, std::size_t i, std::size_t j)
{
	std::lock_guard<std::mutex> lock(m_mutex); // Prevent simultaneously editing balls in different threads

	Ball& b1 = m_balls[i];
	Ball& b2 = m_balls[j];

	b1.position += translate;

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

	float dislodgeFactor1 = r2 / std::sqrt(deltaPos.dot(deltaPos)) - r2 / (r1 + r2);
	float dislodgeFactor2 = r1 / std::sqrt(deltaPos.dot(deltaPos)) - r1 / (r1 + r2);
	b1.position.add(deltaPos * dislodgeFactor1);
	b2.position.subtract(deltaPos * dislodgeFactor2);

	b1.position -= translate;
}


bool BruteForceMultithreadSolver::overlapWithTranslate(const Vec2<float>& translate, std::size_t i, std::size_t j)
{
	Vec2<float>& p1 = m_balls[i].position;
	Vec2<float>& p2 = m_balls[j].position;

	float& r1 = m_ballTypes[m_balls[i].typeindex].radius;
	float& r2 = m_ballTypes[m_balls[j].typeindex].radius;

	return distSquared(p1 + translate, p2) <= (r1 + r2) * (r1 + r2);
}