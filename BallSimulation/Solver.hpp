#pragma once

#include <vector>

#include "Ball.hpp"
#include "BallType.hpp"
#include "World.hpp"

/**
 * An asbstract Solver class for maintaining and updating
 * the particle simulation. The solve() method is pure
 * virtual, and each derived class corresponds to a
 * unique algorithmic implementation of this method.
 * 
 * List of derived classes:
 *     BruteForceSolver
 *     SweepAndPrune1DSolver
 * 
 * The getter methods return const references to data
 * required by the renderer to draw the simulation.
 */

class Solver
{
public:
	virtual void update(float dt);

	const std::vector<BallType>& getBallTypes() const { return m_ballTypes; }
	const std::vector<Ball>&     getBalls()     const { return m_balls; }
	const World&                 getWorld()     const { return m_world; }

protected:

	Solver(std::vector<BallType> ballTypes);    

	std::vector<BallType> m_ballTypes;
	std::vector<Ball> m_balls;

	virtual bool overlap(unsigned int i, unsigned int j);  // Test whether balls at indices i, j overlap
	void resolveCollision(unsigned int i, unsigned int j); // Resolve collision between i, j
	virtual void solve() = 0;                              // Check collisions and update velocities
	void updatePositions(float dt);

	World m_world;
};