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
	BruteForceMultithreadSolver(Preset preset);
	void solve() override;
	void solveWithTranslate(const Vec2<float>& translate);
	bool overlapWithTranslate(const Vec2<float>& translate, const Ball& b1, const Ball& b2);
	void resolveCollisionWithTranslate(const Vec2<float>& translate, Ball& b1, Ball& b2);

	// Multithreading data
	std::mutex m_mutex;
	std::vector<std::future<void>> m_futures;
};