#pragma once

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.hpp"
#include "GLDebug.hpp"

// Filepaths for glsl shader programs
static const char* vertexShaderPath = "../BallSimulation/shader.vs";
static const char* fragmentShaderPath = "../BallSimulation/shader.fs";

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

    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
        {
            // Fill slots in opengl objects with dummy data and move to next balltype
            m_vertexData.push_back({});
            m_vao.push_back(0);
            m_vbo.push_back(0);
            m_indices.push_back({});
            m_ebo.push_back(0);
            continue;
        }
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
        for (std::size_t j = 0; j < m_ballTypes[i].count * vertexMultiplier; j++)
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
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * m_ballTypes[i].count * vertexMultiplier * sizeof(std::size_t), &m_indices.back()[0], GL_DYNAMIC_DRAW));

        // Set texture coordinates
        for (std::size_t j = 0; j < m_ballTypes[i].count * vertexMultiplier; j++)
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

    // Bind the shader program
    m_shader.bind();
}

void Renderer::draw()
{
    int k = 0;

    static const std::vector<Vec2<float>> translates = 
        { { -m_world.xWidth, -m_world.yWidth }, {  0.0f, -m_world.yWidth }, {  m_world.xWidth, -m_world.yWidth },
          { -m_world.xWidth,  0.0f           }, {  0.0f,  0.0f           }, {  m_world.xWidth,  0.0f           },
          { -m_world.xWidth,  m_world.yWidth }, {  0.0f,  m_world.yWidth }, {  m_world.xWidth,  m_world.yWidth } };

    static const std::vector<Vec2<float>> noTranslates = { {0.0f, 0.0f} };

    // Update vertex data according to particle positions
    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
        {
            k += m_ballTypes[i].count;
            continue;
        }
        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;

        float radius = m_ballTypes[i].radius;

        const std::vector<Vec2<float>>& offsets = m_ballTypes[i].wrapTexture ? translates : noTranslates;

        // TO DO: See if this updating can be avoided by passing in positions directly 
        // (long term goal, main performance bottleneck is the solver)

        std::size_t j = 0;
        while (j < m_ballTypes[i].count * vertexMultiplier)
        {
            const Vec2<float>& p = m_balls[k].position;

            for (Vec2<float> offset : offsets)
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
            k++;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw balls to screen
    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
            continue;
        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;

        m_shader.setUniform4f("u_ballColor", m_ballTypes[i].rgba);
        GLCall(glBindVertexArray(m_vao[i]));

        // Draw to screen
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]));
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0,  m_ballTypes[i].count * vertexMultiplier * sizeof(GLVertex) * 4, &m_vertexData[i][0]));
        GLCall(glDrawElements(GL_TRIANGLES, 6 * m_ballTypes[i].count * vertexMultiplier, GL_UNSIGNED_INT, 0));
    }

    glfwSwapBuffers(m_window.getID());

    glfwPollEvents();
}
