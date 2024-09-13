#include "Solver.hpp"

Solver::Solver(std::vector<balltype> ballTypes)
	: m_state(ballTypes) {}

bool Solver::overlap(unsigned int i, unsigned int j)
{
	float r1 = m_state.getRadius(i);
	float r2 = m_state.getRadius(j);

	ball& b1 = m_state.balls[i];
	ball& b2 = m_state.balls[j];

	return distSquared(b1.position, b2.position) <= (r1 + r2) * (r1 + r2);
}


void Solver::resolveCollision(unsigned int i, unsigned int j)
{
	ball& b1 = m_state.balls[i];
	ball& b2 = m_state.balls[j];

	float m1 = m_state.getMass(i);
	float m2 = m_state.getMass(j);

	float r1 = m_state.getRadius(i);
	float r2 = m_state.getRadius(j);

	// Update velocities according to collision physics

	vec2<float> deltaPos = b1.position - b2.position;
	vec2<float> deltaVel = b1.velocity - b2.velocity;

	float a = -((2.0f * m2) / (m1 + m2)) * (deltaVel.dot(deltaPos) / deltaPos.dot(deltaPos));
	float b = -(m1 / m2) * a;

	b1.velocity.Add(deltaPos * a);
	b2.velocity.Add(deltaPos * b);

	// Dislodge balls to prevent sticking

	float dislodgeFactor = 0.5f * (r1 + r2) / std::sqrt(deltaPos.dot(deltaPos)) - 0.5f;
	b1.position.Add(deltaPos * dislodgeFactor);
	b2.position.Subtract(deltaPos * dislodgeFactor);
}

void Solver::update(float dt)
{
	// Check for collisions and update velocities

	solve(); 

	// Update positions

	for (ball& b : m_state.balls)
	{
		b.position += b.velocity * dt;

		// Normalise positions to lie in [-1,1]x[-1,1]

		vec2<float>& p = b.position;
		if (p.x < -1.0f)
			p.x += 2.0f;
		else if (p.x > 1.0f)
			p.x -= 2.0f;

		if (p.y < -1.0f)
			p.y += 2.0f;
		else if (p.y > 1.0f)
			p.y -= 2.0f;
	}
}