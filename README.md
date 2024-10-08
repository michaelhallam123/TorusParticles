# TorusParticles

A simple C++ program for simulating particle collisions on a torus.

![image](images/image1.png)

## Build instructions

Requirements:
- [CMake](https://cmake.org/)
- [GLFW](https://www.glfw.org/)
- [Glad](https://github.com/Dav1dde/glad)
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

Any of the latter three dependencies not found by CMake will be cloned and built automatically when CMake is run. Note they may require extra dependencies in order to build correctly, see e.g. [GLFW Dependencies](https://www.glfw.org/docs/3.3/compile.html#compile_deps).

Steps:
1. Clone the git repository
```bash
git clone <github_link_here>
cd TorusParticles
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
On other operating systems:
```bash
cmake ..
cmake --build .
```

## Usage

Numbers, sizes, colors, masses and radii of particles are specified in `.json` preset files. See the files in the `presets` folder for examples.

To load a preset, drag and drop it into the executable file. When run without a preset, the executable will load `preset1.json` by default.

