// ButtonCylinder.h
// Generator "spljoštenog valjka" (dugmeta) spreman za Phong (pozicija + normala)
// Kompatibilno sa tvojim phong shaderom:
//   layout(location=0) vec3 inPos
//   layout(location=1) vec3 inNor
//
// Koriscenje:
//   ButtonMesh btn = CreateButtonCylinderPhong(0.08f, 0.03f, 32);
//   // u renderu:
//   glBindVertexArray(btn.VAO);
//   glDrawArrays(GL_TRIANGLES, 0, btn.vertexCount);

#pragma once
#include <vector>
#include <cmath>
#include <GL/glew.h>

struct ButtonVertexPhong {
    float x, y, z;   // pos
    float nx, ny, nz; // normal
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
inline ButtonMesh CreateButtonCylinder(float radius, float height, int segments)
{
    if (segments < 3) segments = 3;

    const float yTop = +height * 0.5f;
    const float yBot = -height * 0.5f;
    const float PI = 3.14159265358979323846f;

    std::vector<ButtonVertexPhong> v;
    v.reserve(segments * 12); // top(3) + bot(3) + side(6) po segmentu

    auto push = [&](float x, float y, float z, float nx, float ny, float nz) {
        v.push_back(ButtonVertexPhong{ x, y, z, nx, ny, nz });
        };

    auto normSide = [&](float x, float z) {
        float len = std::sqrt(x * x + z * z);
        if (len <= 1e-8f) return std::pair<float, float>(0.0f, 0.0f);
        return std::pair<float, float>(x / len, z / len);
        };

    for (int i = 0; i < segments; i++)
    {
        float a0 = (float)i / (float)segments * 2.0f * PI;
        float a1 = (float)(i + 1) / (float)segments * 2.0f * PI;

        float x0 = std::cos(a0) * radius;
        float z0 = std::sin(a0) * radius;
        float x1 = std::cos(a1) * radius;
        float z1 = std::sin(a1) * radius;

        // --- TOP DISK (normal gore)
        // winding: (center, p0, p1) gleda ka +Y
        push(0.0f, yTop, 0.0f, 0.0f, +1.0f, 0.0f);
        push(x0, yTop, z0, 0.0f, +1.0f, 0.0f);
        push(x1, yTop, z1, 0.0f, +1.0f, 0.0f);

        // --- BOTTOM DISK (normal dole)
        // winding obrnut da spolja gleda ka -Y
        push(0.0f, yBot, 0.0f, 0.0f, -1.0f, 0.0f);
        push(x1, yBot, z1, 0.0f, -1.0f, 0.0f);
        push(x0, yBot, z0, 0.0f, -1.0f, 0.0f);

        // --- SIDE (radijalne normale)
        auto n0 = normSide(x0, z0);
        auto n1 = normSide(x1, z1);

        // trougao 1
        push(x0, yTop, z0, n0.first, 0.0f, n0.second);
        push(x0, yBot, z0, n0.first, 0.0f, n0.second);
        push(x1, yBot, z1, n1.first, 0.0f, n1.second);

        // trougao 2
        push(x0, yTop, z0, n0.first, 0.0f, n0.second);
        push(x1, yBot, z1, n1.first, 0.0f, n1.second);
        push(x1, yTop, z1, n1.first, 0.0f, n1.second);
    }

    ButtonMesh mesh{};
    mesh.vertexCount = (GLsizei)v.size();

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(ButtonVertexPhong), v.data(), GL_STATIC_DRAW);

    // layout(location=0) in vec3 inPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ButtonVertexPhong), (void*)0);
    glEnableVertexAttribArray(0);

    // layout(location=1) in vec3 inNor;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ButtonVertexPhong), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
