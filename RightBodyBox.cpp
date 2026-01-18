#include "RightBodyBox.h"

RightBodyBoxMesh CreateRightBodyBox(float sx, float sy, float sz)
{
    // centrirana kutija, dimenzije 2*sx, 2*sy, 2*sz
    // format verteksa: pos(3), normal(3), uv(2) -> ukupno 8 floatova
    // (uv ti nije bitan, ali ga daj jer shader verovatno ocekuje)
    float x = sx, y = sy, z = sz;


    // ----------------------
    // DEKLA (drugi mali kvadar) - TUNING
    float dx = 0.95f;   // polu-sirina dekle (X)
    float dy = 0.20f;   // polu-visina  dekle (Y)
    float dz = 0.24f;   // polu-debljina dekle (Z)

    // centar dekle u odnosu na centar korpusa
    float cx = x - dx * 0.8f -0.2; // gore/dole
    float cy = 0.0f +0.7;            // gurni je ka desnoj strani (+X)
    float cz = z + dz * 0.6f; // malo ka napred (+Z)

    // granice (min/max) dekle
    float x0 = cx - dx, x1 = cx + dx;
    float y0 = cy - dy, y1 = cy + dy;
    float z0 = cz - dz, z1 = cz + dz;



    float v[] = {
        // +Z (front)
        -x,-y, z,  0,0,1,  0,0,
         x,-y, z,  0,0,1,  1,0,
         x, y, z,  0,0,1,  1,1,
        -x,-y, z,  0,0,1,  0,0,
         x, y, z,  0,0,1,  1,1,
        -x, y, z,  0,0,1,  0,1,

        // -Z (back)
        -x,-y,-z,  0,0,-1, 0,0,
         x, y,-z,  0,0,-1, 1,1,
         x,-y,-z,  0,0,-1, 1,0,
        -x,-y,-z,  0,0,-1, 0,0,
        -x, y,-z,  0,0,-1, 0,1,
         x, y,-z,  0,0,-1, 1,1,

         // +X (right)
          x,-y,-z,  1,0,0,  0,0,
          x, y,-z,  1,0,0,  0,1,
          x, y, z,  1,0,0,  1,1,
          x,-y,-z,  1,0,0,  0,0,
          x, y, z,  1,0,0,  1,1,
          x,-y, z,  1,0,0,  1,0,

          // -X (left)
          -x,-y,-z, -1,0,0,  0,0,
          -x, y, z, -1,0,0,  1,1,
          -x, y,-z, -1,0,0,  0,1,
          -x,-y,-z, -1,0,0,  0,0,
          -x,-y, z, -1,0,0,  1,0,
          -x, y, z, -1,0,0,  1,1,

          // +Y (top)
          -x, y,-z,  0,1,0,  0,0,
          -x, y, z,  0,1,0,  0,1,
           x, y, z,  0,1,0,  1,1,
          -x, y,-z,  0,1,0,  0,0,
           x, y, z,  0,1,0,  1,1,
           x, y,-z,  0,1,0,  1,0,

           // -Y (bottom)
           -x,-y,-z,  0,-1,0, 0,0,
            x,-y, z,  0,-1,0, 1,1,
           -x,-y, z,  0,-1,0, 0,1,
           -x,-y,-z,  0,-1,0, 0,0,
            x,-y,-z,  0,-1,0, 1,0,
            x,-y, z,  0,-1,0, 1,1,



            // ============================================================
            // DEKLA BOX (drugi kvadar) - 36 verteksa, format: pos(3), nor(3), uv(2)
            // +Z (front)
            x0, y0, z1,   0,0,1,   0,0,
            x1, y0, z1,   0,0,1,   1,0,
            x1, y1, z1,   0,0,1,   1,1,
            x0, y0, z1,   0,0,1,   0,0,
            x1, y1, z1,   0,0,1,   1,1,
            x0, y1, z1,   0,0,1,   0,1,

            // -Z (back)
            x0, y0, z0,   0,0,-1,  0,0,
            x1, y1, z0,   0,0,-1,  1,1,
            x1, y0, z0,   0,0,-1,  1,0,
            x0, y0, z0,   0,0,-1,  0,0,
            x0, y1, z0,   0,0,-1,  0,1,
            x1, y1, z0,   0,0,-1,  1,1,

            // +X (right)
            x1, y0, z0,   1,0,0,   0,0,
            x1, y1, z0,   1,0,0,   0,1,
            x1, y1, z1,   1,0,0,   1,1,
            x1, y0, z0,   1,0,0,   0,0,
            x1, y1, z1,   1,0,0,   1,1,
            x1, y0, z1,   1,0,0,   1,0,

            // -X (left)
            x0, y0, z0,  -1,0,0,   0,0,
            x0, y1, z1,  -1,0,0,   1,1,
            x0, y1, z0,  -1,0,0,   0,1,
            x0, y0, z0,  -1,0,0,   0,0,
            x0, y0, z1,  -1,0,0,   1,0,
            x0, y1, z1,  -1,0,0,   1,1,

            // +Y (top)
            x0, y1, z0,   0,1,0,   0,0,
            x0, y1, z1,   0,1,0,   0,1,
            x1, y1, z1,   0,1,0,   1,1,
            x0, y1, z0,   0,1,0,   0,0,
            x1, y1, z1,   0,1,0,   1,1,
            x1, y1, z0,   0,1,0,   1,0,

            // -Y (bottom)
            x0, y0, z0,   0,-1,0,  0,0,
            x1, y0, z1,   0,-1,0,  1,1,
            x0, y0, z1,   0,-1,0,  0,1,
            x0, y0, z0,   0,-1,0,  0,0,
            x1, y0, z0,   0,-1,0,  1,0,
            x1, y0, z1,   0,-1,0,  1,1,
    };

    RightBodyBoxMesh m;
    m.vertexCount = (int)(sizeof(v) / sizeof(float) / 8);

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);

    glBindVertexArray(m.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

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
