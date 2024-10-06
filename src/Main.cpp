/**
 ****************** TORUS PARTICLE SIMULATOR ******************
 *
 * A simple program to simulate particle collisions on a torus.
 * 
 * The user can create ball types, specifying the number of
 * balls having a given radius, mass, color, and total momentum,
 * as well as whether the balls are rendered to the screen, and
 * if so, whether their textures wrap across the screen 
 * boundaries (see the preset files in presets/ for examples).
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
    std::vector<BallType> preset = loadPreset("presets/preset3.json");
    float dt = 0.01f;

	// Initialise simulation
    SpatialHashSolver solver(preset);

    // Initialise renderer
    unsigned int xResolution = 1920;
    unsigned int yResolution = 1080;
    Renderer renderer(solver, xResolution, yResolution);

	// Simulation loop
    while (renderer.windowOpen())
    {
        float start = glfwGetTime();
        solver.update(dt);

        renderer.draw();

        std::cout << glfwGetTime() - start << std::endl;
    }

    return 0;
}