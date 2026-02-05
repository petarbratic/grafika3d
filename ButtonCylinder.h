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
#include "GLCommon.h"
struct ButtonVertexPhong {
    float x, y, z;     // pos
    float nx, ny, nz;  // normal
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
ButtonMesh CreateButtonCylinder(float radius, float height, int segments);

void DestroyButtonMesh(ButtonMesh& m);