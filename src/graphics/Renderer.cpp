#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.hpp"
#include "GLDebug.hpp"

// Filepaths for glsl shader programs
static const char* vertexShaderPath = "shaders/shader.vs";
static const char* fragmentShaderPath = "shaders/shader.fs";

Renderer::Renderer(const Solver& solver, unsigned int resolution)
	: m_ballTypes(solver.getBallTypes()), 
      m_balls(solver.getBalls()), 
      m_world(solver.getWorld()),
      m_window(resolution),
      m_shader(vertexShaderPath, fragmentShaderPath),
      m_texCoordsVBO(0),
      m_offsetsVBO(0)
/**
 * Each balltype has its own OpenGL vao, vbo, and ebo,
 * stored in the class members m_vaos, m_vbos, and
 * m_ebos. The vertex buffer data for the i-th balltype
 * is stored in m_vertexData[i], and the index buffer
 * data for the i-th balltype in m_indices[i].
 * 
 * A common shader is shared by all balltypes - colors
 * are changed by iterating over balltypes and varying
 * the uniform u_color.
 */
{
    // Reserve space for vertex data
    m_VAOs.resize(m_ballTypes.size());
    m_positionVBOs.resize(m_ballTypes.size());

    std::array<Vec2<float>, 6> baseQuad = 
    {
        // First triangle
        Vec2<float>{-1.0f, -1.0f},
        Vec2<float>{-1.0f,  1.0f},
        Vec2<float>{ 1.0f, -1.0f},
        // Second triangle
        Vec2<float>{ 1.0f, -1.0f},
        Vec2<float>{-1.0f,  1.0f},
        Vec2<float>{ 1.0f,  1.0f}
    };

    // Set texture coordinates (nine copies of baseQuad)
    for (std::size_t i = 0; i < m_texCoords.size(); i++)
    {
        m_texCoords[i] = baseQuad[i % 6];
    }

    // Setup vertex buffer object for texture coordinates
    glGenBuffers(1, &m_texCoordsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
    glBufferData(GL_ARRAY_BUFFER,                        // Target
                 m_texCoords.size()*sizeof(Vec2<float>), // Size (in bytes)
                 (void*)m_texCoords.data(),              // Data
                 GL_STATIC_DRAW                          // Usage
                );

    std::array<Vec2<float>, 9> baseOffsets = 
    {
        Vec2<float>{ 0.0f,  0.0f}, // Use only this offset when BallType has wrapTexture == false
        Vec2<float>{-2.0f, -2.0f},
        Vec2<float>{-2.0f,  0.0f},
        Vec2<float>{-2.0f,  2.0f},
        Vec2<float>{ 0.0f, -2.0f},
        Vec2<float>{ 0.0f,  2.0f},
        Vec2<float>{ 2.0f, -2.0f},
        Vec2<float>{ 2.0f,  0.0f},
        Vec2<float>{ 2.0f,  2.0f}
    };

    // Set offsets (in normalized device coordinates)
    for (std::size_t i = 0; i < m_offsets.size(); i++)
    {
        m_offsets[i] = baseOffsets[i/6];
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

        unsigned int numCopies = m_ballTypes[i].wrapTexture ? 9 : 1;
        // Draw 9 copies when wrapTexture==true, 1 copy when wrapTexture==false

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
    glClear(GL_COLOR_BUFFER_BIT);

    std::size_t startIndex = 0;

    // Draw balls to screen
    for (std::size_t i = 0; i < m_ballTypes.size(); i++)
    {
        if (m_ballTypes[i].render == false)
            continue;

        unsigned int numCopies = m_ballTypes[i].wrapTexture ? 9 : 1;

        m_shader.setUniform4f("u_ballColor", m_ballTypes[i].rgba);
        m_shader.setUniform1f("u_radius", m_ballTypes[i].radius);
        glBindVertexArray(m_VAOs[i]);

        // Draw to screen
        glBindBuffer(GL_ARRAY_BUFFER, m_positionVBOs[i]);
        glBufferSubData(GL_ARRAY_BUFFER,                          // Target
                        0,                                        // Offset
                        m_ballTypes[i].count * sizeof(Ball),      // Size (in bytes)
                        m_balls.data() + startIndex               // Data
                       );
        //glDrawElements(GL_TRIANGLES, 6 * m_ballTypes[i].count * numCopies, GL_UNSIGNED_INT, 0);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6 * numCopies,  m_ballTypes[i].count);
        startIndex += m_ballTypes[i].count;
    }

    glfwSwapBuffers(m_window.getID());

    glfwPollEvents();
}
