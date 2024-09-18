#pragma once

#include <vector>
#include "ball.hpp"
#include "balltype.hpp"

// Note: solver is a pure virtual class, so must be constructed via a derived class

// List of derived classes:
	// BruteForceSolver
	// PruneAndSweep1DSolver

class Solver
{
public:
	virtual void update(float dt);

	const std::vector<balltype>& getBallTypes() const { return m_ballTypes; }
	const std::vector<ball>&     getBalls()     const { return m_balls; }

protected:

	Solver(std::vector<balltype> ballTypes);    

	std::vector<balltype> m_ballTypes;
	std::vector<ball> m_balls;

	virtual bool overlap(unsigned int i, unsigned int j);  // Test whether balls at indices i, j overlap
	void resolveCollision(unsigned int i, unsigned int j); // Resolve collision between i, j
	virtual void solve() = 0;                              // Check collisions and update velocities

	float xWorldMin = -1.0f;
	float xWorldMax =  1.0f;
	float yWorldMin = -1.0f;
	float yWorldMax =  1.0f;
	float xWorldWidth = 2.0f;
	float yWorldWidth = 2.0f;
};