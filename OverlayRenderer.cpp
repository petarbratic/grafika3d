#include "GLCommon.h"
#include "OverlayRenderer.h"

#include "Util.h"

bool OverlayRenderer::init(const char* texturePath, float w, float aspectH_over_W)
{
    m_program = createShader("overlay.vert", "overlay.frag");
    if (m_program == 0) return false;

    m_tex = loadImageToTexture(texturePath);

    if (m_tex != 0) {
        glBindTexture(GL_TEXTURE_2D, m_tex);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glUseProgram(m_program);
    glUniform1i(glGetUniformLocation(m_program, "uTex"), 0);
    glUseProgram(0);

    const float h = w * aspectH_over_W;

    // Donji desni ugao (NDC), bez margine
    const float x2 = 1.0f;
    const float y1 = -1.0f;
    const float x1 = x2 - w;
    const float y2 = y1 + h;

    const float overlayVerts[] = {
        x1, y1,  0.0f, 0.0f,
        x2, y1,  1.0f, 0.0f,
        x2, y2,  1.0f, 1.0f,

        x1, y1,  0.0f, 0.0f,
        x2, y2,  1.0f, 1.0f,
        x1, y2,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVerts), overlayVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    return true;
}

void OverlayRenderer::draw() const
{
    if (!isReady()) return;

    const GLboolean depthWas = glIsEnabled(GL_DEPTH_TEST);
    const GLboolean cullWas = glIsEnabled(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glUseProgram(m_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    if (depthWas) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    if (cullWas) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}

void OverlayRenderer::shutdown()
{
    if (m_tex != 0) {
        glDeleteTextures(1, &m_tex);
        m_tex = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_program != 0) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}
