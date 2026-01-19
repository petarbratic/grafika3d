// AccordionScene.cpp
#include "AccordionScene.h"
#include "TextureManager.h"
#include "Common.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "LeftBodyBox.h"

namespace
{
    // Helper: cache uniform loc once per program
    struct UniformCacheBasic {
        GLuint program = 0;
        GLint useTex = -1;
        GLint transparent = -1;
        bool ok = false;
    };

    struct UniformCachePhong {
        GLuint program = 0;
        GLint kA = -1;
        GLint kD = -1;
        GLint kS = -1;
        GLint shine = -1;
        bool ok = false;
    };

    inline void ensureBasicCache(UniformCacheBasic& c, GLuint program)
    {
        if (c.ok && c.program == program) return;
        c.program = program;
        c.useTex = glGetUniformLocation(program, "useTex");
        c.transparent = glGetUniformLocation(program, "transparent");
        c.ok = true;
    }

    inline void ensurePhongCache(UniformCachePhong& c, GLuint program)
    {
        if (c.ok && c.program == program) return;
        c.program = program;
        c.shine = glGetUniformLocation(program, "uMaterial.shine");
        c.kA = glGetUniformLocation(program, "uMaterial.kA");
        c.kD = glGetUniformLocation(program, "uMaterial.kD");
        c.kS = glGetUniformLocation(program, "uMaterial.kS");
        c.ok = true;
    }

    inline glm::mat4 buildAccM(const glm::vec3& accPos, float accRotY, float accRotX)
    {
        glm::mat4 accM(1.0f);
        accM = glm::translate(accM, accPos);
        accM = glm::rotate(accM, glm::radians(-90.0f), glm::vec3(0, 0, 1));
        accM = glm::rotate(accM, accRotY, glm::vec3(0, 1, 0));
        accM = glm::rotate(accM, accRotX, glm::vec3(1, 0, 0));
        return accM;
    }

    inline void setPhongMaterial(const UniformCachePhong& u,
        float shine,
        const glm::vec3& kA,
        const glm::vec3& kD,
        const glm::vec3& kS)
    {
        if (u.shine != -1) glUniform1f(u.shine, shine);
        if (u.kA != -1) glUniform3f(u.kA, kA.x, kA.y, kA.z);
        if (u.kD != -1) glUniform3f(u.kD, kD.x, kD.y, kD.z);
        if (u.kS != -1) glUniform3f(u.kS, kS.x, kS.y, kS.z);
    }

    // Crne note: sve sa # + Bb (flat) iz tvog enum-a
    static inline bool isBlackNote(NoteId n)
    {
        switch (n)
        {
        case FSHARP0:
        case GSHARP0:
        case BFLAT0:

        case CSHARP:
        case DSHARP:
        case FSHARP:
        case GSHARP:
        case BB:

        case CSHARP2:
        case DSHARP2:
        case FSHARP2:
        case GSHARP2:
            return true;

        default:
            return false;
        }
    }

} // namespace

void AccordionScene::init(const TextureManager* textures)
{
    textures_ = textures;

    // layout 1:1
    getRightHandLayout(layout_);

    // dugme mesh
    btn_ = CreateButtonCylinder(
        0.06f,
        0.1f,
        32,
        { 0.9f, 0.9f, 0.9f, 1.0f }
    );

    N_ = totalRows_ * cols_;
    baseN_ = baseRows_ * cols_;

    localPos_.assign(N_, glm::vec3(0));
    press_.assign(N_, 0.0f);

    // pozicije dugmadi 1:1
    float dx = 0.16f;
    float dy = 0.14f;

    float startX = -0.5f * (cols_ - 1) * dx;
    float startY = 0.5f * (baseRows_ - 1) * dy;

    float rowOffset[3] = { 0.0f, 0.08f, 0.0f };

    float cloneGap = 0.25f;
    float cloneBaseOffsetX = cols_ * dx + cloneGap;

    float cloneTuneX = -1.85f;
    float cloneTuneY = +0.43f;
    float cloneTuneZ = 0.00f;

    float cloneOffsetX = cloneBaseOffsetX + cloneTuneX;
    float cloneOffsetY = cloneTuneY;
    float cloneOffsetZ = cloneTuneZ;

    for (int r = 0; r < totalRows_; r++) {
        int baseR = r % baseRows_;

        float blockX = 0.0f;
        float blockY = 0.0f;
        float blockZ = 0.0f;

        if (r >= baseRows_) {
            blockX = cloneOffsetX;
            blockY = cloneOffsetY;
            blockZ = cloneOffsetZ;
        }

        for (int c = 0; c < cols_; c++) {
            int i = r * cols_ + c;
            localPos_[i] = glm::vec3(
                (startX + c * dx + rowOffset[baseR]) + blockX,
                (startY - baseR * dy) + blockY,
                0.0f + blockZ
            );
        }
    }

    // KORPUS: dimenzije (2*sx, 2*sy, 2*sz)
    RightBody_ = CreateRightBodyBox(0.95f, 0.55f, 0.03f);

    // ============================================================
    // TUNING: LEVA STRANA (menjaj samo ovde)
    // Dekla u RightBodyBox.cpp:
    //   dx=0.95f, dy=0.20f, dz=0.24f
    // Leva treba ista X/Y, malo deblja po Z.
    const float left_dx = 0.95f;
    const float left_dy = 0.3f;
    const float left_dz = 0.24f + 0.01f; // "malo deblja" od dekle (tuning)

    // Pozicija leve strane u odnosu na accM (lokalni prostor scene)
    // MENJAJ OVO dok ne "legne"
    leftBodyOffset_ = glm::vec3(0.047f, 1.30f, 0.148f);

    // Opciona rotacija leve strane (ako zatreba), u radijanima
    // (0 = nema dodatne rotacije)
    leftBodyRotY_ = 0.0f;
    leftBodyRotX_ = 0.0f;
    // ============================================================

    LeftBody_ = CreateLeftBodyBox(left_dx, left_dy, left_dz);
}

void AccordionScene::shutdown()
{
    DestroyLeftBodyBox(LeftBody_);
    DestroyRightBodyBox(RightBody_);
    DestroyButtonMesh(btn_);
}

void AccordionScene::updateRotationFromKeys(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  accRotY_ += 0.03f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) accRotY_ -= 0.03f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    accRotX_ += 0.03f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  accRotX_ -= 0.03f;


    // ------------------------------------------------------------
    // POMERANJE CELE HARMONIKE (TUNING)
    const float moveStep = 0.03f;

    // Q/W: gore/dole po Y osi
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) accPos_.y += moveStep;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) accPos_.y -= moveStep;

    // 1/2: levo/desno po X osi
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) accPos_.x -= moveStep;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) accPos_.x += moveStep;
}

void AccordionScene::updatePress(float dt, const std::vector<float>& pressTarget)
{
    float pressInSpeed = 10.0f;
    float pressOutSpeed = 14.0f;

    for (int i = 0; i < N_; i++) {
        float target = pressTarget[i];
        float speed = (target > press_[i]) ? pressInSpeed : pressOutSpeed;
        press_[i] = press_[i] + (target - press_[i]) * (1.0f - std::exp(-speed * dt));
        press_[i] = clampf(press_[i], 0.0f, 1.0f);
    }
}

void AccordionScene::render(GLuint shader, GLint modelLoc)
{
    // cache uniforms for this shader
    static UniformCacheBasic ub;
    ensureBasicCache(ub, shader);

    // zajednicka matrica scene (harmonika)
    const glm::mat4 accM = buildAccM(accPos_, accRotY_, accRotX_);

    // --------------------
    // KORPUS: "lazni" - samo depth (ne boji)
    if (RightBody_.VAO != 0 && RightBody_.vertexCount > 0)
    {
        glm::mat4 bodyM = accM;
        bodyM = bodyM * glm::translate(glm::mat4(1.0f), glm::vec3(0.05f, 0.2f, -0.03f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bodyM));

        if (ub.useTex != -1) glUniform1i(ub.useTex, 0);
        if (ub.transparent != -1) glUniform1i(ub.transparent, 0);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glBindVertexArray(RightBody_.VAO);
        glDrawArrays(GL_TRIANGLES, 0, RightBody_.vertexCount);
        glBindVertexArray(0);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    // --------------------
    // DUGMAD: teksture
    glBindVertexArray(btn_.VAO);

    for (int i = 0; i < N_; i++)
    {
        glm::mat4 local(1.0f);
        local = glm::translate(local, localPos_[i]);

        glm::mat4 orient(1.0f);
        orient = glm::rotate(orient, glm::radians(-90.0f), glm::vec3(1, 0, 0));

        glm::mat4 pressM(1.0f);
        pressM = glm::translate(pressM, glm::vec3(0, pressDepth_ * press_[i], 0));

        glm::mat4 M = accM * local * orient * pressM;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));

        int r = i / cols_;
        int c = i % cols_;
        int baseR = r % baseRows_;
        NoteId n = layout_[baseR][c];

        glBindTexture(GL_TEXTURE_2D, textures_->tex(n));

        if (ub.useTex != -1) glUniform1i(ub.useTex, 1);
        if (ub.transparent != -1) glUniform1i(ub.transparent, 1);

        glDrawArrays(GL_TRIANGLES, 0, btn_.vertexCount);
    }

    glBindVertexArray(0);
}

void AccordionScene::renderPhong(GLuint phongShader, GLint modelLoc,
    const glm::vec3& body_kA, const glm::vec3& body_kD, const glm::vec3& body_kS, float body_shine,
    const glm::vec3& btn_kA, const glm::vec3& btn_kD, const glm::vec3& btn_kS, float btn_shine)
{
    glUseProgram(phongShader);

    // cache uniforms for this shader
    static UniformCachePhong up;
    ensurePhongCache(up, phongShader);

    // zajednicka matrica
    const glm::mat4 accM = buildAccM(accPos_, accRotY_, accRotX_);

    // --------------------
    // KORPUS: sada vidljiv, sa materijalom
    if (RightBody_.VAO != 0 && RightBody_.vertexCount > 0)
    {
        setPhongMaterial(up, body_shine, body_kA, body_kD, body_kS);

        glm::mat4 bodyM = accM;
        bodyM = bodyM * glm::translate(glm::mat4(1.0f), glm::vec3(0.05f, 0.2f, -0.03f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bodyM));

        glBindVertexArray(RightBody_.VAO);
        glDrawArrays(GL_TRIANGLES, 0, RightBody_.vertexCount);
        glBindVertexArray(0);
    }

    // --------------------
    // LEVA STRANA: crna kutija (privremeno)
    if (LeftBody_.VAO != 0 && LeftBody_.vertexCount > 0)
    {
        // crn materijal
        const glm::vec3 left_kA(0.02f);
        const glm::vec3 left_kD(0.02f);
        const glm::vec3 left_kS(0.08f);
        const float left_shine = 64.0f;

        setPhongMaterial(up, left_shine, left_kA, left_kD, left_kS);

        // koristi TUNING parametre iz init()
        glm::mat4 leftM = accM;
        leftM = leftM * glm::translate(glm::mat4(1.0f), leftBodyOffset_);
        if (leftBodyRotY_ != 0.0f) leftM = leftM * glm::rotate(glm::mat4(1.0f), leftBodyRotY_, glm::vec3(0, 1, 0));
        if (leftBodyRotX_ != 0.0f) leftM = leftM * glm::rotate(glm::mat4(1.0f), leftBodyRotX_, glm::vec3(1, 0, 0));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftM));

        glBindVertexArray(LeftBody_.VAO);
        glDrawArrays(GL_TRIANGLES, 0, LeftBody_.vertexCount);
        glBindVertexArray(0);
    }

    // --------------------
    // DUGMAD: materijal po noti (crno/belo)
    glBindVertexArray(btn_.VAO);

    for (int i = 0; i < N_; i++)
    {
        glm::mat4 local(1.0f);
        local = glm::translate(local, localPos_[i]);

        glm::mat4 orient(1.0f);
        orient = glm::rotate(orient, glm::radians(-90.0f), glm::vec3(1, 0, 0));

        glm::mat4 pressM(1.0f);
        pressM = glm::translate(pressM, glm::vec3(0, pressDepth_ * press_[i], 0));

        glm::mat4 M = accM * local * orient * pressM;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));

        int r = i / cols_;
        int c = i % cols_;
        int baseR = r % baseRows_;
        NoteId n = layout_[baseR][c];

        glm::vec3 kA(0.10f);
        glm::vec3 kD(0.85f);
        glm::vec3 kS(0.35f);

        if (isBlackNote(n))
        {
            kA = glm::vec3(0.25f);
            kD = glm::vec3(0.25f);
            kS = glm::vec3(0.25f);
        }

        float pressDark = 1.0f - 0.20f * press_[i];
        kD *= pressDark;

        setPhongMaterial(up, btn_shine, kA, kD, kS);

        glDrawArrays(GL_TRIANGLES, 0, btn_.vertexCount);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
