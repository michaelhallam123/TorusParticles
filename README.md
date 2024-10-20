# TorusParticles: Experiment

This branch of the project [TorusParticles](https://github.com/michaelhallam123/TorusParticles.git) is used only to simulate the results used in the analysis [ParticleMotionTSA](https://github.com/michaelhallam123/ParticleMotionTSA). The following instructions pertain only to producing the data used in the analysis; for more general uses and information about the program, view the `main` branch. 

![image](images/image1.png)

## Build instructions

### Requirements
- [CMake](https://cmake.org/)
- [GLFW](https://www.glfw.org/)
- [Glad](https://github.com/Dav1dde/glad)
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

The latter three dependencies will be cloned and built automatically when CMake is run. Note they may require extra dependencies in order to build correctly, see e.g. [GLFW Dependencies](https://www.glfw.org/docs/3.3/compile.html#compile_deps).

### Steps
1. Clone the git repository and switch to the correct branch.
```bash
git clone https://github.com/michaelhallam123/TorusParticles.git
cd TorusParticles
git checkout brownian-motion-experiment
```
2. Make a build directory
```bash
mkdir build
cd build
```
3. Build the project with cmake

On Windows:
```bash
cmake ..
cmake --build . --config Release
```
On Linux:
```bash
cmake ..
cmake --build .
```

The executable is located in `build/bin` (`build/bin/Release` on Windows).

If building with Visual Studio instead, the shaders folder and preset files must be moved to the same directory as the solution file, and TorusParticles must be set as the startup project.

## Usage

Running the executable will produce a file `output.csv`, containing the position data for the large particle.

Multithreading is disabled to ensure reproducibility. To enable multithreading, edit the class member `NUM_THREADS` in `SpatialHashSolver.hpp`.