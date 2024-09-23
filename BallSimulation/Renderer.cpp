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
/**
 * Each balltype has its own OpenGL vao, vbo, and ebo,
 * stored in the class members m_vaos, m_vbos, and
 * m_ebos. The vertex buffer data for the i-th balltype
 * is stored in m_vertexData[i], and the index buffer
 * data for the i-th balltype in m_indices[i].
 * 
 * A common shader is shared by all balltypes - colors
 * are changed by varying a uniform.
 */
{
    // Reserve space for vertex data
    m_vertexData.resize(m_ballTypes.size());
    m_indices.resize(m_ballTypes.size());
    m_vaos.resize(m_ballTypes.size());
    m_vbos.resize(m_ballTypes.size());
    m_ebos.resize(m_ballTypes.size());

    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
            continue;

        const unsigned int verticesPerQuad = 4;

        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;
        // If a balltype has wrapTexture set to true we draw nine copies,
        // otherwise we draw one

        // Create space for vertex data
        m_vertexData[i].resize(m_ballTypes[i].count * verticesPerQuad * vertexMultiplier);
        // m_vertexData will later be filled in draw() via updateVertexData()

        // Create vertex array object
        glGenVertexArrays(1, &m_vaos[i]);
        glBindVertexArray(m_vaos[i]);

        // Create vertex buffer object
        glGenBuffers(1, &m_vbos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, m_ballTypes[i].count * verticesPerQuad * vertexMultiplier * sizeof(GLVertex), nullptr, GL_DYNAMIC_DRAW);
        
        // Quad coordinates attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, position));
        glEnableVertexAttribArray(0);
        
        // Texture coordinates attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, textureCoords));
        glEnableVertexAttribArray(1);
        
        // Create and populate index data
        const unsigned int indicesPerQuad = 6;
        m_indices[i].reserve(m_ballTypes[i].count * indicesPerQuad * vertexMultiplier);
        for (std::size_t j = 0; j < m_ballTypes[i].count * vertexMultiplier; j++)
        {
            m_indices[i].push_back(4 * j + 0);
            m_indices[i].push_back(4 * j + 1);
            m_indices[i].push_back(4 * j + 2);
            m_indices[i].push_back(4 * j + 1);
            m_indices[i].push_back(4 * j + 2);
            m_indices[i].push_back(4 * j + 3);
        }
       
        // Create and set up element buffer object
        glGenBuffers(1, &m_ebos[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebos[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ballTypes[i].count * indicesPerQuad * vertexMultiplier * sizeof(std::size_t), m_indices[i].data(), GL_DYNAMIC_DRAW);

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
/**
 * Draws balls to screen. First, vertex data are
 * updated according to m_balls. Each balltype has
 * its own draw call, so we loop through m_ballTypes,
 * skipping over balltypes with render set to false.
 */
{
    updateVertexData();

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw balls to screen
    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
            continue;

        unsigned int vertexMultiplier = m_ballTypes[i].wrapTexture ? 9 : 1;

        m_shader.setUniform4f("u_ballColor", m_ballTypes[i].rgba);
        glBindVertexArray(m_vaos[i]);

        // Draw to screen
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0,  m_ballTypes[i].count * vertexMultiplier * sizeof(GLVertex) * 4, m_vertexData[i].data());
        glDrawElements(GL_TRIANGLES, 6 * m_ballTypes[i].count * vertexMultiplier, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(m_window.getID());

    glfwPollEvents();
}

void Renderer::updateVertexData()
/**
 * Updates GLVertex positions in m_vertexData based on ball locations
 */
{
    int k = 0;

    static const std::vector<Vec2<float>> allTranslates =
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

        const std::vector<Vec2<float>>& translates = m_ballTypes[i].wrapTexture ? allTranslates : noTranslates;

        // TO DO: See if this updating can be avoided by passing in positions directly 
        // (long term goal, main performance bottleneck is the solver)

        std::size_t j = 0;
        while (j < m_ballTypes[i].count * vertexMultiplier)
        {
            const Vec2<float>& pos = m_balls[k].position;

            for (Vec2<float> translate : translates)
            {
                // Bottom left of quad
                m_vertexData[i][4 * j + 0].position[0] = pos.x + translate.x - 1.1f * radius;
                m_vertexData[i][4 * j + 0].position[1] = pos.y + translate.y - 1.1f * radius;
                                                               
                // Top left of quad                            
                m_vertexData[i][4 * j + 1].position[0] = pos.x + translate.x - 1.1f * radius;
                m_vertexData[i][4 * j + 1].position[1] = pos.y + translate.y + 1.1f * radius;
                                                               
                // Bottom right of quad                        
                m_vertexData[i][4 * j + 2].position[1] = pos.y + translate.y - 1.1f * radius;
                m_vertexData[i][4 * j + 2].position[0] = pos.x + translate.x + 1.1f * radius;

                // Top right of quad
                m_vertexData[i][4 * j + 3].position[0] = pos.x + translate.x + 1.1f * radius;
                m_vertexData[i][4 * j + 3].position[1] = pos.y + translate.y + 1.1f * radius;

                j++;
            }
            k++;
        }
    }
}