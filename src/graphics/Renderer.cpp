#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.hpp"

Renderer::Renderer(const Solver& solver, Preset preset, unsigned int xResolution, unsigned int yResolution)
	: m_ballTypes(solver.getBallTypes()), 
      m_balls(solver.getBalls()), 
      m_world(solver.getWorld()),
      m_window(solver, xResolution, yResolution),
      m_shader("shaders/shader.vs", preset.antialiasing ? "shaders/shaderAA.fs" : "shaders/shaderNoAA.fs"),
      m_texCoordsVBO(0),
      m_offsetsVBO(0)
{
    // Reserve space for vertex data
    m_VAOs.resize(m_ballTypes.size());
    m_positionVBOs.resize(m_ballTypes.size());

    std::array<Vec2<float>, VERTICES_PER_QUAD> baseQuad = 
    {
        // Lower left triangle
        Vec2<float>{-1.0f, -1.0f},
        Vec2<float>{-1.0f,  1.0f},
        Vec2<float>{ 1.0f, -1.0f},

        // Upper right triangle
        Vec2<float>{ 1.0f, -1.0f},
        Vec2<float>{-1.0f,  1.0f},
        Vec2<float>{ 1.0f,  1.0f}
    };

    // Set texture coordinates (nine copies of baseQuad)
    for (std::size_t i = 0; i < m_texCoords.size(); i++)
    {
        m_texCoords[i] = baseQuad[i % baseQuad.size()];
    }

    // Setup vertex buffer object for texture coordinates
    glGenBuffers(1, &m_texCoordsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
    glBufferData(GL_ARRAY_BUFFER,                        // Target
                 m_texCoords.size()*sizeof(Vec2<float>), // Size (in bytes)
                 (void*)m_texCoords.data(),              // Data
                 GL_STATIC_DRAW                          // Usage
                );

    std::array<Vec2<float>, MAX_QUADS> baseOffsets = 
    {
        Vec2<float>{           0.0f,            0.0f}, // Use only this offset when wrapTexture==false
        Vec2<float>{-m_world.xWidth, -m_world.yWidth},
        Vec2<float>{-m_world.xWidth,            0.0f},
        Vec2<float>{-m_world.xWidth,  m_world.yWidth},
        Vec2<float>{           0.0f, -m_world.yWidth},
        Vec2<float>{           0.0f,  m_world.yWidth},
        Vec2<float>{ m_world.xWidth, -m_world.yWidth},
        Vec2<float>{ m_world.xWidth,            0.0f},
        Vec2<float>{ m_world.xWidth,  m_world.yWidth}
    };

    // Set offsets (in normalized device coordinates)
    // First quad has offset baseOffsets[0], second quad has baseOffsets[1], etc.
    for (std::size_t i = 0; i < m_offsets.size(); i++)
    {
        m_offsets[i] = baseOffsets[i / VERTICES_PER_QUAD];
    }

    // Setup vertex buffer object for offsets
    glGenBuffers(1, &m_offsetsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_offsetsVBO);
    glBufferData(GL_ARRAY_BUFFER,                      // Target
                 m_offsets.size()*sizeof(Vec2<float>), // Size (in bytes)
                 (void*)m_offsets.data(),              // Data
                 GL_STATIC_DRAW                        // Usage
                );

    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
            continue;

        setVertexAttributes(i);
    }

    // Bind the shader program
    m_shader.bind();

    // Set uniform to transform world coords to screen coords in "shaders/shader.vs"
    m_shader.setUniform4f("u_worldToScreenTransform", 1/m_world.xMax, 1/m_world.yMax, 1.0f, 1.0f);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::setVertexAttributes(std::size_t ballTypeIndex)
{
    std::size_t i = ballTypeIndex;

    // Create vertex array object
    glGenVertexArrays(1, &m_VAOs[i]);
    glBindVertexArray(m_VAOs[i]);

    // Create vertex buffer object for ball positions
    glGenBuffers(1, &m_positionVBOs[i]);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBOs[i]);
    glBufferData(GL_ARRAY_BUFFER,                     // Target
                    m_ballTypes[i].count * sizeof(Ball), // Size (in bytes)
                    nullptr,                             // Data
                    GL_DYNAMIC_DRAW                      // Usage
                );
    
    // Texture coordinates attribute (maps to a_texCoord in shader.vs)
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
    glVertexAttribPointer(0,                   // Index
                            2,                   // Size
                            GL_FLOAT,            // Type
                            GL_FALSE,            // Normalized
                            sizeof(Vec2<float>), // Stride
                            0                    // Offset
                            );
    glVertexAttribDivisor(0, 0);
    
    
    // Offset attribute (maps to a_offset in shader.vs)
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_offsetsVBO);
    glVertexAttribPointer(1,                   // Index
                            2,                   // Size
                            GL_FLOAT,            // Type
                            GL_FALSE,            // Normalized
                            sizeof(Vec2<float>), // Stride
                            0                    // Offset
                            );
    glVertexAttribDivisor(1, 0); 
    

    // Center attribute (maps to a_center in shader.vs)
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBOs[i]);
    glVertexAttribPointer(2,                              // Index
                            2,                              // Size
                            GL_FLOAT,                       // Type
                            GL_FALSE,                       // Normalized
                            sizeof(Ball),                   // Stride
                            (void*)offsetof(Ball, position) // Offset
                            );
    glVertexAttribDivisor(2, 1);
}

void Renderer::draw()
/**
 * Loop through m_ballTypes and draw all balls of a given
 * BallType to the screen in a single instanced draw call.
 */
{
    glClear(GL_COLOR_BUFFER_BIT);

    std::size_t startIndex = 0; // Keep track of the starting index of the i-th BallType in m_balls

    // Draw balls to screen
    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
            continue;

        // Draw 9 translated copies when wrapTexture==true, 1 copy when wrapTexture==false
        unsigned int numCopies = m_ballTypes[i].wrapTexture ? 9 : 1;

        m_shader.setUniform4f("u_ballColor", m_ballTypes[i].rgba);
        m_shader.setUniform1f("u_radius", m_ballTypes[i].radius);
        
        // Draw to screen
        glBindVertexArray(m_VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionVBOs[i]);
        glBufferSubData(GL_ARRAY_BUFFER,                          // Target
                        0,                                        // Offset
                        m_ballTypes[i].count * sizeof(Ball),      // Size (in bytes)
                        m_balls.data() + startIndex               // Data
                       );
        glDrawArraysInstanced(GL_TRIANGLES, 0, VERTICES_PER_QUAD * numCopies,  m_ballTypes[i].count);

        // Update start index in m_balls
        startIndex += m_ballTypes[i].count;
    }

    m_window.update();
}