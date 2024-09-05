#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "vec2.h"

class Simulation
{
public:
	//Constructor that randomly populates initial positions and velocities of balls
	//The sum of all velocities is guaranteed to be totalVelocity
	Simulation(unsigned int ballCount, float ballRadius, float dt, vec2<float> totalVelocity);

	//Constructor that takes initial positions and velocities of balls as input
	Simulation(unsigned int ballCount, float ballRadius, float dt, std::vector<vec2<float>> positions, std::vector<vec2<float>> velocities);

	~Simulation();

	//Increment time in the simulation
	void Update(float timeStep);

	void Print()//Print ball locations for debugging
	{
		for (auto& p : m_positions)
		{
			std::cout << p.x << " " << p.y << std::endl;
		}
		std::cout << std::endl;
	}

	const std::vector<vec2<float>>& getPositions() const
	{
		return m_positions;
	}

	unsigned int getBallCount() const
	{
		return m_ballCount;
	}

	float getBallRadius() const
	{
		return m_ballRadius;
	}



private:
	//Simulation properties
	std::vector<vec2<float>> m_positions;
	std::vector<vec2<float>> m_velocities;
	float m_ballRadius;
	unsigned int m_ballCount;
	float m_dt;

	void Collide(unsigned int i, unsigned int j);
};
