// RightBodyBox.cpp
// Korpus desne strane + "dekla" (drugi kvadar) sa kosinom (chamfer) na cosku.
// Sve je "copypaste-ready".
// Parametri za podesavanje su u sekciji TUNING.

#include "RightBodyBox.h"
#include <GL/glew.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



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
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        glm::vec3 n = faceNormal(a, b, c);
        out.push_back({ a.x,a.y,a.z, n.x,n.y,n.z, 0,0 });
        out.push_back({ b.x,b.y,b.z, n.x,n.y,n.z, 1,0 });
        out.push_back({ c.x,c.y,c.z, n.x,n.y,n.z, 1,1 });
    }

    static void pushQuad(std::vector<Vtx>& out,
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
    {
        // (a,b,c) + (a,c,d)
        pushTri(out, a, b, c);
        pushTri(out, a, c, d);
    }

    // Axis-aligned box sa auto normalama (zavisno od winding-a).
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

    // Polygon (convex) triangulacija fan metodom:
    // (p0,p1,p2), (p0,p2,p3), ...
    static void pushPolyFan(std::vector<Vtx>& out, const std::vector<glm::vec3>& p)
    {
        if (p.size() < 3) return;
        for (size_t i = 1; i + 1 < p.size(); i++)
            pushTri(out, p[0], p[i], p[i + 1]);
    }
}

RightBodyBoxMesh CreateRightBodyBox(float sx, float sy, float sz)
{
    // KORPUS: centrirana kutija, dimenzije 2*sx, 2*sy, 2*sz
    float x = sx, y = sy, z = sz;

    // ============================================================
    // TUNING PARAMETRI (MENJAJ OVO)

    // --- DEKLA (drugi kvadar) dimenzije ---
    float dx = 0.95f; // polu-sirina  (X)
    float dy = 0.20f; // polu-visina  (Y)
    float dz = 0.24f; // polu-debljina (Z)

    // --- Centar dekle u odnosu na centar korpusa (lokalni model prostor) ---
    // Napomena: zbog rotacija u sceni, ose mogu "delovati" zamenjeno na ekranu.
    // Ovde su ovo ciste lokalne ose modela:
    float cx = x - dx * 0.8f - 0.2f; // lokalni X
    float cy = 0.0f + 0.7f;          // lokalni Y
    float cz = z + dz * 0.6f;        // lokalni Z

    // --- KOSINA (chamfer) na "top-front-right" cosku dekle ---
    // Sece ugao blizu (x1,y1,z1) u X i Z smeru, kroz celu visinu (od y0 do y1).
    // Ako stavis 0 -> nema kosine.
    float chX = 0.0f; // koliko se odsece po X (0..dx)
    float chZ = 0.0f; // koliko se odsece po Z (0..dz)

    // ============================================================

    // Clamp da ne probijes dimenzije
    if (chX < 0.0f) chX = 0.0f;
    if (chZ < 0.0f) chZ = 0.0f;
    if (chX > dx)   chX = dx;
    if (chZ > dz)   chZ = dz;

    // granice (min/max) dekle
    float x0 = cx - dx, x1 = cx + dx;
    float y0 = cy - dy, y1 = cy + dy;
    float z0 = cz - dz, z1 = cz + dz;

    // Ako nema kosine -> obican kvadar
    bool useChamfer = (chX > 0.0f && chZ > 0.0f);

    std::vector<Vtx> verts;
    verts.reserve(36 + 200);

    // ============================================================
    // 1) KORPUS (osnovni box)
    pushBox(verts, -x, x, -y, y, -z, z);

    // ============================================================
    // 2) DEKLA (drugi box) - sa ili bez kosine
    if (!useChamfer)
    {
        pushBox(verts, x0, x1, y0, y1, z0, z1);
    }
    else
    {
        // Osnovne tacke dekle (bez kosine)
        glm::vec3 P000(x0, y0, z0);
        glm::vec3 P100(x1, y0, z0);
        glm::vec3 P110(x1, y1, z0);
        glm::vec3 P010(x0, y1, z0);

        glm::vec3 P001(x0, y0, z1);
        glm::vec3 P101(x1, y0, z1);
        glm::vec3 P111(x1, y1, z1);
        glm::vec3 P011(x0, y1, z1);

        // Chamfer tačke (odsecen cosak oko P111, kroz celu visinu)
        glm::vec3 A(x1 - chX, y1, z1);       // na front ivici gore (pomereno po X)
        glm::vec3 B(x1, y1, z1 - chZ);// na right ivici gore (pomereno po Z)
        glm::vec3 A0(x1 - chX, y0, z1);       // isto to dole
        glm::vec3 B0(x1, y0, z1 - chZ);// isto to dole

        // ---- Back (-Z): ne dira kosina
        // outward -Z
        pushQuad(verts, { x1,y0,z0 }, { x0,y0,z0 }, { x0,y1,z0 }, { x1,y1,z0 });

        // ---- Left (-X): ne dira kosina
        pushQuad(verts, { x0,y0,z1 }, { x0,y1,z1 }, { x0,y1,z0 }, { x0,y0,z0 });

        // ---- Front (+Z): "odseci" desno gore -> koristi A i A0 umesto x1,..,z1
        // quad: (x0,y0,z1) (x1-chX,y0,z1) (x1-chX,y1,z1) (x0,y1,z1)
        pushQuad(verts,
            { x0,y0,z1 }, { x1 - chX,y0,z1 }, { x1 - chX,y1,z1 }, { x0,y1,z1 });

        // ---- Right (+X): "odseci" napred gore -> koristi B i B0 umesto ..,z1
        // Da bi normala bila +X, winding mora biti:
        // (x1,y0,z0) (x1,y1,z0) (x1,y1,z1-chZ) (x1,y0,z1-chZ)
        pushQuad(verts,
            { x1,y0,z0 }, { x1,y1,z0 }, { x1,y1,z1 - chZ }, { x1,y0,z1 - chZ });

        // ---- Top (+Y): postaje poligon sa 5 temena (odsečen cosak)
        // CCW gledano odozgo (+Y):
        // (x0,z0) -> (x0,z1) -> A -> B -> (x1,z0)
        {
            std::vector<glm::vec3> top;
            top.reserve(5);
            top.push_back({ x0, y1, z0 });
            top.push_back({ x0, y1, z1 });
            top.push_back(A);
            top.push_back(B);
            top.push_back({ x1, y1, z0 });
            pushPolyFan(verts, top);
        }

        // ---- Bottom (-Y): takodje poligon sa 5 temena (jer je kosina kroz celu visinu)
        // CCW gledano odozdo (-Y):
        // (x0,z0) -> (x1,z0) -> B0 -> A0 -> (x0,z1)
        {
            std::vector<glm::vec3> bot;
            bot.reserve(5);
            bot.push_back({ x0, y0, z0 });
            bot.push_back({ x1, y0, z0 });
            bot.push_back(B0);
            bot.push_back(A0);
            bot.push_back({ x0, y0, z1 });
            pushPolyFan(verts, bot);
        }

        // ---- Chamfer strana: nova kosa ploha (quad) koja spaja A0-B0-B-A
        // Winding izabran tako da normal ide ka +X i +Z (napolje).
        pushQuad(verts, A0, B0, B, A);

        // Napomena:
        // Ovo je "full-height" chamfer. Ako kasnije budeš hteo kosinu samo gore
        // (da dole ostane pravi ugao), onda se menja bottom i chamfer ploha.
        (void)P000; (void)P100; (void)P110; (void)P010;
        (void)P001; (void)P101; (void)P111; (void)P011;
    }

    // ============================================================
    // Upload u GPU
    RightBodyBoxMesh m{};
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

void DestroyRightBodyBox(RightBodyBoxMesh& m)
{
    if (m.VBO) glDeleteBuffers(1, &m.VBO);
    if (m.VAO) glDeleteVertexArrays(1, &m.VAO);
    m = {};
}
