#pragma once

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Shader.h"
#include "GLVertex.h"
#include "balltype.hpp"
#include "GLDebug.h"

// Paths for gl shader programs
const char* vertexShaderPath = "../BallSimulation/shader.vs";
const char* fragmentShaderPath = "../BallSimulation/shader.fs";

Renderer::Renderer(const Solver& solver, unsigned int resolution)
	: m_ballTypes(solver.getBallTypes()), 
      m_balls(solver.getBalls()), 
      m_world(solver.getWorld()),
      m_window(resolution),
      m_shader(vertexShaderPath, fragmentShaderPath)
{
    // Reserve space for vertex data
    m_vao.reserve(m_ballTypes.size());
    m_vbo.reserve(m_ballTypes.size());
    m_ebo.reserve(m_ballTypes.size());
    m_vertexData.reserve(m_ballTypes.size());
    m_indices.reserve(m_ballTypes.size());

    for (int i = 0; i < m_ballTypes.size(); i++)
    {
        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;
        m_vertexData.push_back(std::vector<GLVertex>(m_ballTypes[i].count * 4 * vertexMultiplier));

        // Create vertex array objects
        m_vao.push_back(0);
        GLCall(glGenVertexArrays(1, &m_vao[i]));
        GLCall(glBindVertexArray(m_vao[i]));

        // Create vertex buffer objects
        m_vbo.push_back(0);
        GLCall(glGenBuffers(1, &m_vbo[i]));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]));
        GLCall(glBufferData(GL_ARRAY_BUFFER, m_ballTypes[i].count * 4 * vertexMultiplier * sizeof(GLVertex), nullptr, GL_DYNAMIC_DRAW));

        // Quad coordinates attribute
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
        GLCall(glEnableVertexAttribArray(0));
        // Texture coordinates attribute
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2U * sizeof(float))));
        GLCall(glEnableVertexAttribArray(1));

        // Create and set up index data
        m_indices.push_back({});
        m_indices.back().reserve(6 * m_ballTypes[i].count * vertexMultiplier);
        for (unsigned int j = 0; j < m_ballTypes[i].count * vertexMultiplier; j++)
        {
            m_indices.back().push_back(4 * j + 0);
            m_indices.back().push_back(4 * j + 1);
            m_indices.back().push_back(4 * j + 2);
            m_indices.back().push_back(4 * j + 1);
            m_indices.back().push_back(4 * j + 2);
            m_indices.back().push_back(4 * j + 3);
        }
       
        // Create and set up element buffer objects
        m_ebo.push_back(0);
        GLCall(glGenBuffers(1, &m_ebo[i]));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[i]));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * m_ballTypes[i].count * vertexMultiplier * sizeof(unsigned int), &m_indices.back()[0], GL_DYNAMIC_DRAW));

    }

    m_shader.Bind();

    // Set texture coordinates
    for (int i = 0; i < m_ballTypes.size(); i++)
    {
        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;

        for (int j = 0; j < m_ballTypes[i].count * vertexMultiplier; j++)
        {
            // Bottom left of quad
            m_vertexData[i][4 * j + 0].textureCoords[0] = 0.0f;
            m_vertexData[i][4 * j + 0].textureCoords[1] = 0.0f;

            // Top left of quad
            m_vertexData[i][4 * j + 1].textureCoords[0] = 0.0f;
            m_vertexData[i][4 * j + 1].textureCoords[1] = 1.0f;

            // Bottom right of quad
            m_vertexData[i][4 * j + 2].textureCoords[0] = 1.0f;
            m_vertexData[i][4 * j + 2].textureCoords[1] = 0.0f;

            // Top right of quad
            m_vertexData[i][4 * j + 3].textureCoords[0] = 1.0f;
            m_vertexData[i][4 * j + 3].textureCoords[1] = 1.0f;
        }
    }
}

void Renderer::Draw()
{
    int k = 0;

    static std::vector<vec2<float>> offsets = { { -m_world.xWidth, -m_world.yWidth }, {  0.0f, -m_world.yWidth }, {  m_world.xWidth, -m_world.yWidth },
                                                { -m_world.xWidth,  0.0f           }, {  0.0f,  0.0f           }, {  m_world.xWidth,  0.0f           },
                                                { -m_world.xWidth,  m_world.yWidth }, {  0.0f,  m_world.yWidth }, {  m_world.xWidth,  m_world.yWidth } };

    // Update vertex data according to particle positions
    for (int i = 0; i < m_ballTypes.size(); i++)
    {
        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;

        float radius = m_ballTypes[i].radius;

        // TO DO: See if this updating can be avoided by passing in positions directly 
        // (long term goal, main performance bottleneck is the solver)

        // TO DO: Fix the terrible code below
        int j = 0;
        while (j < m_ballTypes[i].count * vertexMultiplier)
        {
            const vec2<float>& p = m_balls[k].position;

            if (vertexMultiplier == 1)
            {
                // Bottom left of quad
                m_vertexData[i][4 * j + 0].position[0] = p.x - 1.1f * radius;
                m_vertexData[i][4 * j + 0].position[1] = p.y - 1.1f * radius;

                // Top left of quad
                m_vertexData[i][4 * j + 1].position[0] = p.x - 1.1f * radius;
                m_vertexData[i][4 * j + 1].position[1] = p.y + 1.1f * radius;

                // Bottom right of quad
                m_vertexData[i][4 * j + 2].position[0] = p.x + 1.1f * radius;
                m_vertexData[i][4 * j + 2].position[1] = p.y - 1.1f * radius;

                // Top right of quad
                m_vertexData[i][4 * j + 3].position[0] = p.x + 1.1f * radius;
                m_vertexData[i][4 * j + 3].position[1] = p.y + 1.1f * radius;

                j++;
            }
            else
            {
                for (vec2<float> offset : offsets)
                {
                    // Bottom left of quad
                    m_vertexData[i][4 * j + 0].position[0] = p.x + offset.x - 1.1f * radius;
                    m_vertexData[i][4 * j + 0].position[1] = p.y + offset.y - 1.1f * radius;

                    // Top left of quad
                    m_vertexData[i][4 * j + 1].position[0] = p.x + offset.x - 1.1f * radius;
                    m_vertexData[i][4 * j + 1].position[1] = p.y + offset.y + 1.1f * radius;

                    // Bottom right of quad
                    m_vertexData[i][4 * j + 2].position[0] = p.x + offset.x + 1.1f * radius;
                    m_vertexData[i][4 * j + 2].position[1] = p.y + offset.y - 1.1f * radius;

                    // Top right of quad
                    m_vertexData[i][4 * j + 3].position[0] = p.x + offset.x + 1.1f * radius;
                    m_vertexData[i][4 * j + 3].position[1] = p.y + offset.y + 1.1f * radius;

                    j++;
                }
            }
            
            k++;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw balls to screen
    for (int i = 0; i < m_ballTypes.size(); i++)
    {
        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;

        m_shader.SetUniform4f("u_ballColor", m_ballTypes[i].rgba);
        GLCall(glBindVertexArray(m_vao[i]));

        // Draw to screen
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]));
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0,  m_ballTypes[i].count * vertexMultiplier * sizeof(GLVertex) * 4, &m_vertexData[i][0]));
        GLCall(glDrawElements(GL_TRIANGLES, 6 * m_ballTypes[i].count * vertexMultiplier, GL_UNSIGNED_INT, 0));
    }

    glfwSwapBuffers(m_window.getID());

    glfwPollEvents();

    m_window.update();
}
