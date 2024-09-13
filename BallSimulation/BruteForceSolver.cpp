#include "BruteForceSolver.hpp"

BruteForceSolver::BruteForceSolver(std::vector<balltype> ballTypes)
	: Solver(ballTypes)
{

}

void BruteForceSolver::solve()
{
	std::vector<ball>& balls = m_state.balls;

	std::vector<vec2<float>> displacements = { { -2.0f, -2.0f }, {  0.0f, -2.0f }, {  2.0f, -2.0f },
											   { -2.0f,  0.0f }, {  0.0f,  0.0f }, {  2.0f,  0.0f },
											   { -2.0f,  2.0f }, {  0.0f,  2.0f }, {  2.0f,  2.0f } };

	for (int i = 0; i < balls.size(); i++)
	{
		for (int j = i+1; j < balls.size(); j++)
		{
			for (vec2<float> displacement : displacements)
			{
				balls[i].position += displacement;
				if (overlap(i, j))
				{
					resolveCollision(i, j);
				}
				balls[i].position -= displacement;
			}
		}
	}
}