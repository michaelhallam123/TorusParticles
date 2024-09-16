#pragma once

// Struct containing data required to implement the line sweep algorithm
struct lineSweepData
{
	float left;       // Leftmost endpoint of the ball
	float right;      // Rightmost endpoint of the ball
	unsigned int ind; // Index of the ball in m_balls
};

// Custom comparators for lineSweepData
struct
{
	bool operator()(const lineSweepData& a, const lineSweepData& b) const { return a.left < b.left; }
}
leftComparison;

struct
{
	bool operator()(const lineSweepData& a, const lineSweepData& b) const { return a.right > b.right; }
}
rightComparison;