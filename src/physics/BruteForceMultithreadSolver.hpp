#pragma once

/**
 * A derived class of Solver implementing a simple
 * brute force algorithm.
 *
 * Checks every pair of balls for a collision. Nine
 * checks are performed per each pair of balls to
 * account for the possibility of boundary crossing
 * (in the spirit of a brute force algorithm, this
 * is intentionally inefficient)
 */

#include <vector>
#include <thread>
#include <mutex>
#include <future>

#include "Solver.hpp"

class BruteForceMultithreadSolver : public Solver
{
public:
	BruteForceMultithreadSolver(std::vector<BallType> ballTypes);
	void solve() override;
	void solveWithTranslate(const Vec2<float>& translate);
	bool overlapWithTranslate(const Vec2<float>& translate, std::size_t i, std::size_t j);
	void resolveCollisionWithTranslate(const Vec2<float>& translate, std::size_t i, std::size_t j);

	std::mutex m_mutex;
	std::vector<std::future<void>> m_futures;
};