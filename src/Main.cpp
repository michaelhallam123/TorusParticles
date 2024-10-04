/**
 ****************** TORUS PARTICLE SIMULATOR ******************
 *
 * A simple program to simulate particle collisions on a torus.
 * 
 * The user can create ball types, specifying the number of
 * balls having a given radius, mass, color, and total momentum,
 * as well as whether the balls are rendered to the screen, and
 * if so, whether their textures wrap across the screen 
 * boundaries (see the preset files in res/ for examples).
 * 
 * Support is available for a brute force algorithm, a sweep
 * and prune algorithm, and a spatial hashing algorithm. The
 * best and recommended algorithm is the spatial hasher.
 */

#include <iostream>

#include "Renderer.hpp"
#include "BruteForceSolver.hpp"
#include "BruteForceMultithreadSolver.hpp"
#include "SweepAndPrune1DSolver.hpp"
#include "SpatialHashSolver.hpp"
#include "loadPreset.hpp"

int main()
{
	// Set simulation parameters
    std::vector<BallType> preset = loadPreset("presets/preset4.json");
    float dt = 0.01f;

	// Initialise simulation
    SpatialHashSolver solver(preset);

    // Initialise renderer
    unsigned int resolution = 860;
    Renderer renderer(solver, resolution);

	// Simulation loop
    while (renderer.windowOpen())
    {
        solver.update(dt);

        renderer.draw();
    }

    return 0;
}