#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "vec2.h"
#include "ball.h"

class Simulation
{
public:
	Simulation(std::vector<ball> ballData, float dt);

	//Increment time in the simulation
	void Update(float timeStep);

	// Getter methods
	inline const std::vector<std::vector<vec2<float>>>& getPositions() const { return m_positions; }
	inline unsigned int getBallCount(unsigned int i) const { return m_ballData[i].count; }
	inline float getRadius(unsigned int i) const { return m_ballData[i].radius; }
	inline const std::vector<ball>& getBallData() const { return m_ballData; }

	void print()
	{
		for (auto p : m_positions[0])
			std::cout << p.x << " " << p.y << std::endl;
		std::cout << std::endl;
	}

private:
	// Simulation properties
	std::vector<ball> m_ballData;
	std::vector<std::vector<vec2<float>>> m_positions;
	std::vector<std::vector<vec2<float>>> m_velocities;
		// May be better to change these to 1D vectors - try this later and see what's faster
		// Would involve either an index fetcher function or just storing all indices in a vector
		// Latter is faster but potentially wastes a lot of memory for no reason
	float m_dt;

	// Collide balls with indices (i1,j1) and (i2, j2) in m_positions
	void collide(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2);

	
};
