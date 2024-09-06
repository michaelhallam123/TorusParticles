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
	Simulation(unsigned int ballCount, float smallRadius, float bigRadius, float dt, vec2<float> totalVelocity);

	/*
	//Constructor that takes initial positions and velocities of balls as input
	Simulation(unsigned int ballCount, float ballRadius, float dt, std::vector<vec2<float>> positions, std::vector<vec2<float>> velocities);
	*/
	~Simulation();

	//Increment time in the simulation
	void Update(float timeStep);

	void Print()//Print ball locations for debugging
	{
		for (auto& p : m_smallPositions)
		{
			std::cout << p.x << " " << p.y << std::endl;
		}
		std::cout << std::endl;
	}

	// Getter methods
	inline const std::vector<vec2<float>>& getSmallPositions() const { return m_smallPositions; }
	inline const			  vec2<float>& getBigPosition()	   const { return m_bigPosition; }
	inline					  unsigned int getBallCount()	   const { return m_ballCount; }
	inline							 float getSmallRadius()	   const { return m_smallRadius; }

private:
	//Simulation properties
	std::vector<vec2<float>> m_smallPositions;
	std::vector<vec2<float>> m_smallVelocities;
	vec2<float> m_bigPosition;
	vec2<float> m_bigVelocity;
	float m_smallRadius;
	float m_bigRadius;
	unsigned int m_ballCount;
	float m_dt;

	// Collide small balls with indices i, j in m_smallPositions
	void collideSmalls(unsigned int i, unsigned int j); 
};
