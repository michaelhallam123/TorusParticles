#pragma once

#include <vector>
#include "ball.hpp"
#include "balltype.hpp"
#include "World.hpp"

// Abstract Solver class

// List of derived classes:
	// BruteForceSolver
	// PruneAndSweep1DSolver

class Solver
{
public:
	virtual void update(float dt);

	const std::vector<balltype>& getBallTypes() const { return m_ballTypes; }
	const std::vector<ball>&     getBalls()     const { return m_balls; }
	const World&                 getWorld()     const { return m_world; }

protected:

	Solver(std::vector<balltype> ballTypes);    

	std::vector<balltype> m_ballTypes;
	std::vector<ball> m_balls;

	virtual bool overlap(unsigned int i, unsigned int j);  // Test whether balls at indices i, j overlap
	void resolveCollision(unsigned int i, unsigned int j); // Resolve collision between i, j
	virtual void solve() = 0;                              // Check collisions and update velocities

	World m_world;
};