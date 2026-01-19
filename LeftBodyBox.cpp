// LeftBodyBox.cpp
// Jednostavan box (bez dekle), auto normale, pos+normal+uv layout: (0 pos), (1 normal), (2 uv)

#include "LeftBodyBox.h"

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace
{
    struct Vtx
    {
        float px, py, pz; // position
        float nx, ny, nz; // normal
        float u, v;       // uv (nebitno, ali ostavljeno)
    };

    static glm::vec3 faceNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        return glm::normalize(glm::cross(b - a, c - a));
    }

    static void pushTri(std::vector<Vtx>& out,
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
        const glm::vec2& ua, const glm::vec2& ub, const glm::vec2& uc)
    {
        glm::vec3 n = faceNormal(a, b, c);
        out.push_back({ a.x,a.y,a.z, n.x,n.y,n.z, ua.x,ua.y });
        out.push_back({ b.x,b.y,b.z, n.x,n.y,n.z, ub.x,ub.y });
        out.push_back({ c.x,c.y,c.z, n.x,n.y,n.z, uc.x,uc.y });
    }

    static void pushQuad(std::vector<Vtx>& out,
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
    {
        // jednostavan uv
        glm::vec2 ua(0, 0), ub(1, 0), uc(1, 1), ud(0, 1);
        pushTri(out, a, b, c, ua, ub, uc);
        pushTri(out, a, c, d, ua, uc, ud);
    }

    static void pushBox(std::vector<Vtx>& out,
        float x0, float x1, float y0, float y1, float z0, float z1)
    {
        // Front (+Z)
        pushQuad(out,
            { x0,y0,z1 }, { x1,y0,z1 }, { x1,y1,z1 }, { x0,y1,z1 });

        // Back (-Z)
        pushQuad(out,
            { x1,y0,z0 }, { x0,y0,z0 }, { x0,y1,z0 }, { x1,y1,z0 });

        // Right (+X)
        pushQuad(out,
            { x1,y0,z0 }, { x1,y1,z0 }, { x1,y1,z1 }, { x1,y0,z1 });

        // Left (-X)
        pushQuad(out,
            { x0,y0,z1 }, { x0,y1,z1 }, { x0,y1,z0 }, { x0,y0,z0 });

        // Top (+Y)
        pushQuad(out,
            { x0,y1,z0 }, { x0,y1,z1 }, { x1,y1,z1 }, { x1,y1,z0 });

        // Bottom (-Y)
        pushQuad(out,
            { x0,y0,z0 }, { x1,y0,z0 }, { x1,y0,z1 }, { x0,y0,z1 });
    }
}

LeftBodyBoxMesh CreateLeftBodyBox(float sx, float sy, float sz)
{
    std::vector<Vtx> verts;
    verts.reserve(36);

    // box centriran u (0,0,0)
    pushBox(verts, -sx, sx, -sy, sy, -sz, sz);

    LeftBodyBoxMesh m{};
    m.vertexCount = (int)verts.size();

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);

    glBindVertexArray(m.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(verts.size() * sizeof(Vtx)), verts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return m;
}

void DestroyLeftBodyBox(LeftBodyBoxMesh& m)
{
    if (m.VBO) glDeleteBuffers(1, &m.VBO);
    if (m.VAO) glDeleteVertexArrays(1, &m.VAO);
    m = {};
}
