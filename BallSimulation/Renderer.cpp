#pragma once

#include <iostream>

#include "Renderer.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLVertex.h"

// TO DO: Change m_bigVertexData to be stack allocated rather than heap allocated


Renderer::Renderer(const Simulation& simulation)
	: m_simulation(simulation), m_vbo(0), m_vao(0), m_ebo(0), m_indices(nullptr), m_smallVertexData(nullptr), m_bigVertexData(nullptr)
{
    // Reserve space for vertex data
    unsigned int ballCount = m_simulation.getBallCount();
    m_smallVertexData = new GLVertex[ballCount * 4];
    m_bigVertexData = new GLVertex[4];
	
	// Create and bind vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create, set up, and bind vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, ballCount * 8 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Create, set up, and bind element buffer object
    unsigned int* m_indices = new unsigned int[6 * ballCount];

    for (unsigned int i = 0; i < ballCount; i++)
    {
        m_indices[6 * i + 0] = 4 * i + 0;
        m_indices[6 * i + 1] = 4 * i + 1;
        m_indices[6 * i + 2] = 4 * i + 2;
        m_indices[6 * i + 3] = 4 * i + 1;
        m_indices[6 * i + 4] = 4 * i + 2;
        m_indices[6 * i + 5] = 4 * i + 3;
    }

    // Create, set up, and bind element buffer object
    glGenBuffers(1, &m_ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * ballCount * sizeof(unsigned int), m_indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2U * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create and bind gl shader program
    const char* vertexShaderPath = "../BallSimulation/shader.vs";
    const char* fragmentShaderPath = "../BallSimulation/shader.fs";

    Shader shader(vertexShaderPath, fragmentShaderPath);
    shader.Bind();
    shader.SetUniform4f("u_ballColor", 1.0f, 0.5f, 0.2f, 1.0f);
}

Renderer::~Renderer()
{
	delete[] m_smallVertexData;
    delete[] m_indices;
}

void Renderer::Draw()
{
    // Update vertex data according to particle positions

    // Small balls
    const auto& positions = m_simulation.getSmallPositions();
    unsigned int i = 0;
    float smallRadius = m_simulation.getSmallRadius();
    unsigned int ballCount = m_simulation.getBallCount();
    for (const auto& p : positions)
    {
        // Bottom left of quad
        m_smallVertexData[4 * i + 0].position[0] = p.x - 1.1f * smallRadius;
        m_smallVertexData[4 * i + 0].position[1] = p.y - 1.1f * smallRadius;
        m_smallVertexData[4 * i + 0].textureCoords[0] = 0.0f;
        m_smallVertexData[4 * i + 0].textureCoords[1] = 0.0f;

        // Top left of quad
        m_smallVertexData[4 * i + 1].position[0] = p.x - 1.1f * smallRadius;
        m_smallVertexData[4 * i + 1].position[1] = p.y + 1.1f * smallRadius;
        m_smallVertexData[4 * i + 1].textureCoords[0] = 0.0f;
        m_smallVertexData[4 * i + 1].textureCoords[1] = 1.0f;

        // Bottom right of quad
        m_smallVertexData[4 * i + 2].position[0] = p.x + 1.1f * smallRadius;
        m_smallVertexData[4 * i + 2].position[1] = p.y - 1.1f * smallRadius;
        m_smallVertexData[4 * i + 2].textureCoords[0] = 1.0f;
        m_smallVertexData[4 * i + 2].textureCoords[1] = 0.0f;

        // Top right of quad
        m_smallVertexData[4 * i + 3].position[0] = p.x + 1.1f * smallRadius;
        m_smallVertexData[4 * i + 3].position[1] = p.y + 1.1f * smallRadius;
        m_smallVertexData[4 * i + 3].textureCoords[0] = 1.0f;
        m_smallVertexData[4 * i + 3].textureCoords[1] = 1.0f;

        i++;
    }

    float bigRadius = m_simulation.getBigRadius();
    vec2<float> p = m_simulation.getBigPosition();

    // Bottom left of quad
    m_bigVertexData[0].position[0] = p.x - 1.1f * bigRadius;
    m_bigVertexData[0].position[1] = p.y - 1.1f * bigRadius;
    m_bigVertexData[0].textureCoords[0] = 0.0f;
    m_bigVertexData[0].textureCoords[1] = 0.0f;

    // Top left of quad
    m_bigVertexData[1].position[0] = p.x - 1.1f * bigRadius;
    m_bigVertexData[1].position[1] = p.y + 1.1f * bigRadius;
    m_bigVertexData[1].textureCoords[0] = 0.0f;
    m_bigVertexData[1].textureCoords[1] = 1.0f;

    // Bottom right of quad
    m_bigVertexData[2].position[0] = p.x + 1.1f * bigRadius;
    m_bigVertexData[2].position[1] = p.y - 1.1f * bigRadius;
    m_bigVertexData[2].textureCoords[0] = 1.0f;
    m_bigVertexData[2].textureCoords[1] = 0.0f;

    // Top right of quad
    m_bigVertexData[3].position[0] = p.x + 1.1f * bigRadius;
    m_bigVertexData[3].position[1] = p.y + 1.1f * bigRadius;
    m_bigVertexData[3].textureCoords[0] = 1.0f;
    m_bigVertexData[3].textureCoords[1] = 1.0f;

    // Large ball

    // Draw to screen
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * ballCount * sizeof(float), m_smallVertexData);
    glDrawElements(GL_TRIANGLES, 8 * ballCount, GL_UNSIGNED_INT, 0);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(float), m_bigVertexData);
    glDrawElements(GL_TRIANGLES, 8 * ballCount, GL_UNSIGNED_INT, 0);
    
}
