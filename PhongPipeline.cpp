#include "GLCommon.h"
#include "PhongPipeline.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"

bool PhongPipeline::init(int viewportW, int viewportH)
{
    (void)viewportW;

    m_program = createShader("phong.vert", "phong.frag");
    if (m_program == 0) return false;

    m_modelLoc = glGetUniformLocation(m_program, "uM");
    m_viewLoc = glGetUniformLocation(m_program, "uV");
    m_projLoc = glGetUniformLocation(m_program, "uP");

    m_viewPosLoc = glGetUniformLocation(m_program, "uViewPos");

    m_lightPosLoc = glGetUniformLocation(m_program, "uLight.pos");
    m_lightALoc = glGetUniformLocation(m_program, "uLight.kA");
    m_lightDLoc = glGetUniformLocation(m_program, "uLight.kD");
    m_lightSLoc = glGetUniformLocation(m_program, "uLight.kS");

    m_matShineLoc = glGetUniformLocation(m_program, "uMaterial.shine");
    m_matALoc = glGetUniformLocation(m_program, "uMaterial.kA");
    m_matDLoc = glGetUniformLocation(m_program, "uMaterial.kD");
    m_matSLoc = glGetUniformLocation(m_program, "uMaterial.kS");

    // Kamera (isto kao u tvom main-u)
    const glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 2.2f),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    const float aspect = (viewportH != 0) ? (float)viewportW / (float)viewportH : 1.0f;
    const glm::mat4 proj = glm::perspective(
        glm::radians(70.0f),
        aspect,
        0.1f,
        100.0f
    );

    glUseProgram(m_program);

    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(m_viewPosLoc, 0.0f, 0.0f, 2.2f);

    // Svetlo (isto kao u tvom main-u)
    glUniform3f(m_lightPosLoc, 0.0f, 0.0f, 2.2f);
    glUniform3f(m_lightALoc, 0.25f, 0.25f, 0.25f);
    glUniform3f(m_lightDLoc, 0.85f, 0.85f, 0.85f);
    glUniform3f(m_lightSLoc, 1.0f, 1.0f, 1.0f);

    glUseProgram(0);

    return true;
}

void PhongPipeline::shutdown()
{
    if (m_program != 0) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

void PhongPipeline::use() const
{
    glUseProgram(m_program);
}

void PhongPipeline::stop() const
{
    glUseProgram(0);
}
