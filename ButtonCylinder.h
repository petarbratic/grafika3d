// ButtonCylinder.h
// Jednostavan generator "spljoštenog valjka" (dugmeta) za OpenGL 3.3
// Format verteksa je kompatibilan sa tvojim shaderima:
// layout(location=0) vec3 pos, location=1 vec4 col, location=2 vec2 tex
//
// Koriscenje (primer):
//   #include "ButtonCylinder.h"
//   ButtonMesh btn = CreateButtonCylinder(0.08f, 0.03f, 32, {0.9f,0.9f,0.9f,1.0f});
//   // u renderu: glBindVertexArray(btn.VAO); glDrawArrays(GL_TRIANGLES, 0, btn.vertexCount);

#pragma once
#include <vector>
#include <cmath>
#include <GL/glew.h>

struct BtnColor {
    float r, g, b, a;
};

struct ButtonVertex {
    float x, y, z;     // pos
    float r, g, b, a;  // color
    float s, t;        // tex
};

struct ButtonMesh {
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLsizei vertexCount = 0;
};

// Generise valjak oko Y ose, centriran u (0,0,0)
// radius - poluprecnik
// height - debljina (po Y)
// segments - broj segmenata (>= 3), npr. 24/32/48
inline ButtonMesh CreateButtonCylinder(float radius, float height, int segments, BtnColor color)
{
    if (segments < 3) segments = 3;

    const float yTop = +height * 0.5f;
    const float yBot = -height * 0.5f;
    const float PI = 3.14159265358979323846f;

    std::vector<ButtonVertex> v;
    v.reserve(segments * 12); // top(3) + bot(3) + side(6) po segmentu

    auto push = [&](float x, float y, float z, float s, float t) {
        v.push_back(ButtonVertex{ x, y, z, color.r, color.g, color.b, color.a, s, t });
        };

    for (int i = 0; i < segments; i++)
    {
        float a0 = (float)i / (float)segments * 2.0f * PI;
        float a1 = (float)(i + 1) / (float)segments * 2.0f * PI;

        float x0 = std::cos(a0) * radius;
        float z0 = std::sin(a0) * radius;
        float x1 = std::cos(a1) * radius;
        float z1 = std::sin(a1) * radius;

        // GORNJI DISK (triangulacija iz centra)
        // Tex koordinate: mapiranje kruga u [0,1]x[0,1]
        push(0.0f, yTop, 0.0f, 0.5f, 0.5f);
        push(x0, yTop, z0, 0.5f + (x0 / (2.0f * radius)), 0.5f + (z0 / (2.0f * radius)));
        push(x1, yTop, z1, 0.5f + (x1 / (2.0f * radius)), 0.5f + (z1 / (2.0f * radius)));

        // DONJI DISK (obrnut winding da "spolja" gleda nadole)
        push(0.0f, yBot, 0.0f, 0.5f, 0.5f);
        push(x1, yBot, z1, 0.5f + (x1 / (2.0f * radius)), 0.5f + (z1 / (2.0f * radius)));
        push(x0, yBot, z0, 0.5f + (x0 / (2.0f * radius)), 0.5f + (z0 / (2.0f * radius)));

        // OMOTAC (2 trougla)
        // Tex: s ide oko valjka, t po visini
        float s0 = (float)i / (float)segments;
        float s1 = (float)(i + 1) / (float)segments;

        // trougao 1
        push(x0, yTop, z0, s0, 1.0f);
        push(x0, yBot, z0, s0, 0.0f);
        push(x1, yBot, z1, s1, 0.0f);

        // trougao 2
        push(x0, yTop, z0, s0, 1.0f);
        push(x1, yBot, z1, s1, 0.0f);
        push(x1, yTop, z1, s1, 1.0f);
    }

    ButtonMesh mesh{};
    mesh.vertexCount = (GLsizei)v.size();

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(ButtonVertex), v.data(), GL_STATIC_DRAW);

    // layout(location=0) in vec3 inPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // layout(location=1) in vec4 inCol;
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // layout(location=2) in vec2 inTex;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return mesh;
}

inline void DestroyButtonMesh(ButtonMesh& m)
{
    if (m.VBO) glDeleteBuffers(1, &m.VBO);
    if (m.VAO) glDeleteVertexArrays(1, &m.VAO);
    m.VAO = 0; m.VBO = 0; m.vertexCount = 0;
}
