#pragma once

#include <vector>
#include <fstream>

#include "Preset.hpp"
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
 *     BruteForceSolver (REMOVED)
 *     BruteForceMultithreadSolver (REMOVED)
 *     SweepAndPrune1DSolver (REMOVED)
 *     SpatialHashSolver
 */

class Solver
{
public:
	virtual void update(float dt); // Perform a full simulation cycle: check particle collisions, 
	                               // update velocities of colliding particles, then update
	                               // positions of each particle.

	const std::vector<BallType>& getBallTypes() const { return m_ballTypes; }
	const std::vector<Ball>&     getBalls()     const { return m_balls; }
	const World&                 getWorld()     const { return m_world; }

protected:

	Solver(Preset preset);    

	std::vector<BallType> m_ballTypes;
	std::vector<Ball>     m_balls;

	virtual void solve() = 0;                     // Check collisions and update velocities

	bool overlap(const Ball& ball1, const Ball& ball2); // Test whether balls b1 and b2 overlap
	void resolveCollision(Ball& ball1, Ball& ball2);    // Resolve collision between b1 and b2
	void updatePositions(float dt);               // Update positions of particles

	World m_world;

	// For recording experiment data
	std::ofstream m_file;
	float m_xPos = 0.0f;
	float m_yPos = 0.0f;
	int m_stepCounter = 0;  // Count how many steps the simulation has done
	int m_recordSteps = 10; // Every m_recordSteps, write m_xPos and m_yPos to m_file
};