#pragma once

#include "State.hpp"

class Solver
{
public:
	virtual void update(float dt);
	const State& getState() const { return m_state; }

protected:
	Solver(std::vector<balltype> ballTypes);
	State m_state;
	bool overlap(unsigned int i, unsigned int j);          // Test whether balls at indices i, j overlap
	void resolveCollision(unsigned int i, unsigned int j); // Resolve collision between i, j
	virtual void solve() = 0;                              // Check collisions and update velocities
};