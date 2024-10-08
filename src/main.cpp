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
 * best and recommended algorithm is the spatial hash.
 */

#include <iostream>

#include "Renderer.hpp"
#include "BruteForceSolver.hpp"
#include "BruteForceMultithreadSolver.hpp"
#include "SweepAndPrune1DSolver.hpp"
#include "SpatialHashSolver.hpp"
#include "loadPreset.hpp"
#include "Preset.hpp"

int main(int argc, char* argv[])
{
	// Set simulation parameters
    Preset preset;

    if (argc == 1)
        preset = loadPreset("preset1.json");
    else if (argc == 2)
        preset = loadPreset(argv[1]);
    else
    {
        std::cout << "Error: program accepts at most one argument" << std::endl;
        std::cout << "Terminating program..." << std::endl;
        std::cin.get();
        return -1;
    }

    if (!preset.loadSuccessful)
    {
        std::cout << "Error: failed to load preset" << std::endl;
        std::cout << "Terminating program..." << std::endl;
        std::cin.get();
        return -2;
    }

    float dt = preset.dt;

	// Initialise simulation
    SpatialHashSolver solver(preset);

    // Initialise renderer
    unsigned int xResolution = 1280;
    unsigned int yResolution = 720;
    Renderer renderer(solver, preset, xResolution, yResolution);

	// Simulation loop
    while (renderer.windowOpen())
    {
        solver.update(dt);

        renderer.draw();
    }

    return 0;
}