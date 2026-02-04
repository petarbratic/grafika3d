#pragma once
#include "GLCommon.h"

struct RightBodyBoxMesh {
    GLuint VAO = 0;
    GLuint VBO = 0;
    int vertexCount = 0;
};

RightBodyBoxMesh CreateRightBodyBox(float sx, float sy, float sz);
void DestroyRightBodyBox(RightBodyBoxMesh& m);