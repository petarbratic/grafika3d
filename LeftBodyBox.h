// LeftBodyBox.h
// Jednostavan box (bez dekle), sa normalama, kompatibilan sa Phong shaderom (pos+normal+uv).
#pragma once
#include "GLCommon.h"

struct LeftBodyBoxMesh
{
    GLuint VAO = 0;
    GLuint VBO = 0;
    int vertexCount = 0;
};

// Box centriran u (0,0,0), dimenzije 2*sx, 2*sy, 2*sz
LeftBodyBoxMesh CreateLeftBodyBox(float sx, float sy, float sz);
void DestroyLeftBodyBox(LeftBodyBoxMesh& m);
