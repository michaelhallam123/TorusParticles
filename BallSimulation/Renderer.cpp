#pragma once

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Shader.h"
#include "GLVertex.h"
#include "ball.h"
#include "GLDebug.h"


// Paths for gl shader programs
const char* vertexShaderPath = "../BallSimulation/shader.vs";
const char* fragmentShaderPath = "../BallSimulation/shader.fs";

// TO DO: create a GLQuad class

Renderer::Renderer(const Simulation& simulation)
	: m_simulation(simulation), m_shader(vertexShaderPath, fragmentShaderPath)
{
    // Reserve space for vertex data
    const std::vector<ball>& ballData = simulation.getBallData();
    m_vao.reserve(ballData.size());
    m_vbo.reserve(ballData.size());
    m_ebo.reserve(ballData.size());
    m_vertexData.reserve(ballData.size());
    m_indices.reserve(ballData.size());

    for (int i = 0; i < ballData.size(); i++)
    {
        m_vertexData.push_back(new GLVertex[ballData[i].count * 4]);

        // Create vertex array objects
        m_vao.push_back(0);
        GLCall(glGenVertexArrays(1, &m_vao[i]));
        GLCall(glBindVertexArray(m_vao[i]));

        // Create vertex buffer objects
        m_vbo.push_back(0);
        GLCall(glGenBuffers(1, &m_vbo[i]));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]));
        GLCall(glBufferData(GL_ARRAY_BUFFER, ballData[i].count * 4 * sizeof(GLVertex), nullptr, GL_DYNAMIC_DRAW));

        // Quad coordinates attribute
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
        GLCall(glEnableVertexAttribArray(0));
        // Texture coordinates attribute
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2U * sizeof(float))));
        GLCall(glEnableVertexAttribArray(1));

        // Create and set up index data
        m_indices.push_back(new unsigned int[6 * ballData[i].count]);
        for (unsigned int j = 0; j < ballData[i].count; j++)
        {
            m_indices[i][6 * j + 0] = 4 * j + 0;
            m_indices[i][6 * j + 1] = 4 * j + 1;
            m_indices[i][6 * j + 2] = 4 * j + 2;
            m_indices[i][6 * j + 3] = 4 * j + 1;
            m_indices[i][6 * j + 4] = 4 * j + 2;
            m_indices[i][6 * j + 5] = 4 * j + 3;
        }
       
        // Create and set up element buffer objects
        m_ebo.push_back(0);
        GLCall(glGenBuffers(1, &m_ebo[i]));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[i]));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * ballData[i].count * sizeof(unsigned int), m_indices[i], GL_DYNAMIC_DRAW));     

    }


    m_shader.Bind();


}

Renderer::~Renderer()
{
    for (auto& x : m_vertexData)
        delete[] x;
    for (auto& y : m_indices)
        delete[] y;
}

void Renderer::Draw(GLFWwindow* window)
{
    const std::vector<ball>& ballData = m_simulation.getBallData();
    const std::vector<std::vector<vec2<float>>>& positionsVector = m_simulation.getPositions();

    // Update vertex data according to particle positions
    for (int i = 0; i < ballData.size(); i++)
    {
        const auto& positions = positionsVector[i];
        float radius = ballData[i].radius;
        unsigned int j = 0;


        // TO DO: stop updating texture coords every frame, there's no reason to do this!
        for (const auto& p : positions)
        {
            // Bottom left of quad
            m_vertexData[i][4 * j + 0].position[0] = p.x - 1.1f * radius;
            m_vertexData[i][4 * j + 0].position[1] = p.y - 1.1f * radius;
            m_vertexData[i][4 * j + 0].textureCoords[0] = 0.0f;
            m_vertexData[i][4 * j + 0].textureCoords[1] = 0.0f;

            // Top left of quad
            m_vertexData[i][4 * j + 1].position[0] = p.x - 1.1f * radius;
            m_vertexData[i][4 * j + 1].position[1] = p.y + 1.1f * radius;
            m_vertexData[i][4 * j + 1].textureCoords[0] = 0.0f;
            m_vertexData[i][4 * j + 1].textureCoords[1] = 1.0f;

            // Bottom right of quad
            m_vertexData[i][4 * j + 2].position[0] = p.x + 1.1f * radius;
            m_vertexData[i][4 * j + 2].position[1] = p.y - 1.1f * radius;
            m_vertexData[i][4 * j + 2].textureCoords[0] = 1.0f;
            m_vertexData[i][4 * j + 2].textureCoords[1] = 0.0f;

            // Top right of quad
            m_vertexData[i][4 * j + 3].position[0] = p.x + 1.1f * radius;
            m_vertexData[i][4 * j + 3].position[1] = p.y + 1.1f * radius;
            m_vertexData[i][4 * j + 3].textureCoords[0] = 1.0f;
            m_vertexData[i][4 * j + 3].textureCoords[1] = 1.0f;

            j++;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw balls to screen
    for (int i = 0; i < ballData.size(); i++)
    {

        m_shader.SetUniform4f("u_ballColor", ballData[i].rgba);
        GLCall(glBindVertexArray(m_vao[i]));

        // Draw to screen
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]));
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0,  ballData[i].count * sizeof(GLVertex) * 4, m_vertexData[i]));
        GLCall(glDrawElements(GL_TRIANGLES, 6 * ballData[i].count, GL_UNSIGNED_INT, 0));
    }

    glfwSwapBuffers(window);

    glfwPollEvents();
}
