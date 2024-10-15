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
 * To load a preset file, simply drag and drop it onto the
 * executable (Windows) or add the filename as an argument to
 * the program (Linux).
 */

#include <iostream>

#include "Renderer.hpp"
#include "SpatialHashSolver.hpp"
#include "loadPreset.hpp"

int main(int argc, char* argv[])
{
	// Load preset
    Preset preset = loadPreset("preset1.json");

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

    std::cout << "Starting simulation..." << std::endl;

    const unsigned int ITERATIONS = 1'000'000;

    std::vector<float> percentages(22);

    for (int i = 0; i < percentages.size(); i++)
    {
        percentages[i] = 0.05f*i;
    }

    int percentageInd = 0;

	// Simulation loop
    for (int i = 0; i < ITERATIONS; i++)
    {
        if ((float)i / (float)ITERATIONS > percentages[percentageInd])
        {
            std::cout << (int)(100.0f*percentages[percentageInd]) << "% complete" << std::endl;
            percentageInd++;
        }
        solver.update(dt);
    }

    std::cout << "Simulation complete!" << std::endl;
    std::cout << "Press enter to close...";
    std::cin.get();

    return 0;
}