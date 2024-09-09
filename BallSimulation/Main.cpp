//TO DO:
    // Fix antialiasing so that balls actually touch on collision - may need to alter texture coordinates for this
    // Also antialiasing still looks somewhat jagged
    // Allow for screen resizing
    // Separate physics from framerate (!)
    // Speed up simulation using caching to get large number of particles
    // Make circles loop smoothly from one side of the screen to the opposite (probably much easier using shader)
    // Add gui for easier control of parameters
    // Speed up simulation using multithreading
    // Add pause and step through simulation functionality
    // Add cute window icon
    // Make the main file cleaner, maybe just give simulation a run method containing the while loop

#include <iostream>

#include "vec2.h"
#include "ball.h"
#include "Simulation.h"
#include "Renderer.h"
#include "Window.h"

int main()
{
    // Create window (this should be tied to the renderer class - otherwise might create renderer before window which would be bad)
    int resolution = 860;
    Window window(resolution);

	// Set simulation parameters:
	float dt = 0.2f;

    ball b1;
    b1.radius = 0.2f;
    b1.mass = 0.5f;
    b1.count = 4;
    b1.rgba = { 1.0f, 0.5f, 0.2f, 1.0f };
    b1.totalMomentum = { 0.0f, 0.0f };

    ball b2;
    b2.radius = 0.1f;
    b2.mass = 0.1f;
    b2.count = 4;
    b2.rgba = { 0.0f, 0.5f, 0.8f, 1.0f };
    b2.totalMomentum = { 0.0f, 0.0f };

    ball b3;
    b3.radius = 0.02f;
    b3.mass = 0.1f;
    b3.count = 200;
    b3.rgba = { 1.0f, 0.5f, 0.8f, 1.0f };
    b3.totalMomentum = { 0.0f, 0.0f };

	// Initialise simulation and renderer
    Simulation simulation({b1, b2, b3}, dt);
    Renderer renderer(simulation);

    float lastTime = 0.0f;

	// Run simulation
    while (window.isOpen())
    {
        float time = glfwGetTime();
        float timeStep = time - lastTime;
        lastTime = time;

        // Draw simulation to window
        renderer.Draw(window.getID());

        // Update window events, swap buffers
        window.update();

        // Update physics simulation
        simulation.Update(timeStep);

        //simulation.print();
    }

    return 0;

}