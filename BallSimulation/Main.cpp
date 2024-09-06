//TO DO:
    // Fix antialiasing so that balls actually touch on collision - may need to alter texture coordinates for this
    // Also antialiasing still looks somewhat jagged
    // Allow for screen resizing
    // Separate physics from framerate (!)
    // Speed up simulation using caching to get large number of particles
    // Add in a larger particle that is the subject of Brownian motion
    // Make circles loop smoothly from one side of the screen to the opposite (probably much easier using shader)
    // Add mass to balls
    // Add gui for easier control of parameters
    // Speed up simulation using multithreading
    // Add pause and step through simulation functionality
    // Add cute window icon
    // Make a struct for storing simulation parameters
    // Make a struct for storing graphics parameters
    // Make the main file cleaner, maybe just give simulation a run method containing the while loop

// C++ includes
#include <iostream>

// External includes
// Project includes
#include "vec2.h"
#include "Simulation.h"
#include "Renderer.h"
#include "Window.h"

int main()
{
    // Create window (this should be tied to the renderer class - otherwise might create renderer before window which would be bad)
    int resolution = 860;
    Window window(resolution);

	// Set simulation parameters:
    // TO DO: make a struct for simulation parameters
	unsigned int ballCount = 300; 
	float smallRadius = 0.02f;
    float bigRadius = 0.1f;
    float smallMass = 0.5f;
    float bigMass = (bigRadius / smallRadius) * (bigRadius / smallRadius) * smallMass;
	float dt = 0.1f;
	vec2<float> totalVelocity = { 10.0f, 10.0f };

	// Initialise simulation and renderer
	Simulation simulation(ballCount, smallRadius, bigRadius, smallMass, bigMass, dt, totalVelocity);
    Renderer renderer(simulation);

    float lastTime = 0.0f;

	// Run simulation
    while (window.isOpen())
    {
        float time = glfwGetTime();
        float timeStep = time - lastTime;
        lastTime = time;

        // Draw simulation to window
        renderer.Draw();

        // Update window events, swap buffers
        window.update();

        // Update physics simulation
        simulation.Update(timeStep);
    }

    return 0;

}