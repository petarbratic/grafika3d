// AccordionScene.cpp
#include "GLCommon.h"

#include "AccordionScene.h"
#include "Common.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include "LeftBodyBox.h"

namespace
{
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

    static inline float smoothTo(float current, float target, float inSpeed, float outSpeed, float dt)
    {
        float speed = (target > current) ? inSpeed : outSpeed;
        float next = current + (target - current) * (1.0f - std::exp(-speed * dt));
        return clampf(next, 0.0f, 1.0f);
    }

    static inline glm::mat4 buildButtonM(const glm::mat4& group,
        const glm::vec3& pos,
        float press,
        float pressDepth)
    {
        glm::mat4 local(1.0f);
        local = glm::translate(local, pos);

        glm::mat4 orient(1.0f);
        orient = glm::rotate(orient, glm::radians(-90.0f), glm::vec3(1, 0, 0));

        glm::mat4 pressM(1.0f);
        pressM = glm::translate(pressM, glm::vec3(0, pressDepth * press, 0));

        return group * local * orient * pressM;
    }

    static inline void getTrebleMaterial(NoteId n, float press,
        glm::vec3& kA, glm::vec3& kD, glm::vec3& kS)
    {
        if (isBlackNote(n))
        {
            kA = glm::vec3(0.25f);
            kD = glm::vec3(0.25f);
            kS = glm::vec3(0.25f);
        }
        else
        {
            kA = glm::vec3(0.10f);
            kD = glm::vec3(0.85f);
            kS = glm::vec3(0.35f);
        }

        float pressDark = 1.0f - 0.20f * press;
        kD *= pressDark;
    }

    static inline void getBassMaterial(float press,
        glm::vec3& kA, glm::vec3& kD, glm::vec3& kS)
    {
        kA = glm::vec3(0.10f);
        kD = glm::vec3(0.90f);
        kS = glm::vec3(0.25f);

        float pressDark = 1.0f - 0.25f * press;
        kD *= pressDark;
    }

    static inline glm::mat4 buildBassGroup(const glm::mat4& accM,
        const glm::vec3& offset,
        float rotY,
        float rotX)
    {
        glm::mat4 g = accM;
        g = g * glm::translate(glm::mat4(1.0f), offset);
        if (rotY != 0.0f) g = g * glm::rotate(glm::mat4(1.0f), rotY, glm::vec3(0, 1, 0));
        if (rotX != 0.0f) g = g * glm::rotate(glm::mat4(1.0f), rotX, glm::vec3(1, 0, 0));
        return g;
    }
} // namespace

void AccordionScene::init(const TextureManager* textures)
{
    textures_ = textures;

    getRightHandLayout(layout_);

    btn_ = CreateButtonCylinder(0.06f, 0.1f, 32);
    bassBtn_ = CreateButtonCylinder(0.040f, 0.085f, 32);

    N_ = totalRows_ * cols_;
    baseN_ = baseRows_ * cols_;

    bassN_ = bassRows_ * bassCols_;
    bassStartIndex_ = N_;
    N_ = N_ + bassN_;

    localPos_.assign(totalRows_ * cols_, glm::vec3(0));
    press_.assign(totalRows_ * cols_, 0.0f);

    bassLocalPos_.assign(bassN_, glm::vec3(0));
    bassPress_.assign(bassN_, 0.0f);

    {
        const float dx = 0.16f;
        const float dy = 0.14f;

        const float startX = -0.5f * (cols_ - 1) * dx;
        const float startY = 0.5f * (baseRows_ - 1) * dy;

        const float rowOffset[3] = { 0.0f, 0.08f, 0.0f };

        const float cloneGap = 0.25f;
        const float cloneBaseOffsetX = cols_ * dx + cloneGap;

        const float cloneTuneX = -1.85f;
        const float cloneTuneY = +0.43f;
        const float cloneTuneZ = 0.00f;

        const float cloneOffsetX = cloneBaseOffsetX + cloneTuneX;
        const float cloneOffsetY = cloneTuneY;
        const float cloneOffsetZ = cloneTuneZ;

        for (int r = 0; r < totalRows_; r++) {
            const int baseR = r % baseRows_;

            float blockX = 0.0f;
            float blockY = 0.0f;
            float blockZ = 0.0f;

            if (r >= baseRows_) {
                blockX = cloneOffsetX;
                blockY = cloneOffsetY;
                blockZ = cloneOffsetZ;
            }

            for (int c = 0; c < cols_; c++) {
                const int i = r * cols_ + c;
                localPos_[i] = glm::vec3(
                    (startX + c * dx + rowOffset[baseR]) + blockX,
                    (startY - baseR * dy) + blockY,
                    0.0f + blockZ
                );
            }
        }
    }

    RightBody_ = CreateRightBodyBox(0.95f, 0.55f, 0.03f);

    {
        const float left_dx = 0.95f;
        const float left_dy = 0.3f;
        const float left_dz = 0.24f + 0.01f;

        leftBodyOffset_ = glm::vec3(0.047f, 1.30f, 0.148f);
        leftBodyRotY_ = 0.0f;
        leftBodyRotX_ = 0.0f;

        LeftBody_ = CreateLeftBodyBox(left_dx, left_dy, left_dz);
    }

    bassOffset_ = glm::vec3(0.12f, 1.18f, 0.4f);
    bassRotY_ = 0.0f;
    bassRotX_ = 0.0f;

    {
        const float bdx = 0.11f;
        const float bdy = 0.12f;

        const float bStartX = -0.5f * (bassCols_ - 1) * bdx;
        const float bStartY = 0.5f * (bassRows_ - 1) * bdy;

        const float rowSkewX = 0.06f;
        const float colSkewY = 0.00f;

        for (int r = 0; r < bassRows_; r++)
        {
            const int rLogical = r;
            const float skewX = rLogical * rowSkewX;

            for (int c = 0; c < bassCols_; c++)
            {
                const int i = r * bassCols_ + c;

                float x = bStartX + c * bdx;
                float y = bStartY - rLogical * bdy;

                x += skewX;
                y += c * colSkewY;

                bassLocalPos_[i] = glm::vec3(x, y, 0.0f);
            }
        }
    }
}

void AccordionScene::shutdown()
{
    DestroyLeftBodyBox(LeftBody_);
    DestroyRightBodyBox(RightBody_);
    DestroyButtonMesh(btn_);
    DestroyButtonMesh(bassBtn_);
}

void AccordionScene::updateRotationFromKeys(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  accRotY_ += 0.03f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) accRotY_ -= 0.03f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    accRotX_ += 0.03f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  accRotX_ -= 0.03f;

    const float moveStep = 0.03f;

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) accPos_.y += moveStep;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)  accPos_.y -= moveStep;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) accPos_.x -= moveStep;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) accPos_.x += moveStep;
}

void AccordionScene::updatePress(float dt, const std::vector<float>& pressTarget)
{
    const float pressInSpeed = 10.0f;
    const float pressOutSpeed = 14.0f;

    const int trebleCount = totalRows_ * cols_;
    for (int i = 0; i < trebleCount; i++) {
        float target = (i < (int)pressTarget.size()) ? pressTarget[i] : 0.0f;
        press_[i] = smoothTo(press_[i], target, pressInSpeed, pressOutSpeed, dt);
    }

    for (int i = 0; i < bassN_; i++) {
        int idx = bassStartIndex_ + i;
        float target = (idx >= 0 && idx < (int)pressTarget.size()) ? pressTarget[idx] : 0.0f;
        bassPress_[i] = smoothTo(bassPress_[i], target, pressInSpeed, pressOutSpeed, dt);
    }
}

void AccordionScene::render(GLuint shader, GLint modelLoc)
{
    static UniformCacheBasic ub;
    ensureBasicCache(ub, shader);

    const glm::mat4 accM = buildAccM(accPos_, accRotY_, accRotX_);

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

    glBindVertexArray(btn_.VAO);

    const int trebleCount = totalRows_ * cols_;
    for (int i = 0; i < trebleCount; i++)
    {
        glm::mat4 group = accM;
        glm::mat4 M = buildButtonM(group, localPos_[i], press_[i], pressDepth_);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));

        int r = i / cols_;
        int c = i % cols_;
        int baseR = r % baseRows_;
        NoteId n = layout_[baseR][c];

        

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

    static UniformCachePhong up;
    ensurePhongCache(up, phongShader);

    const glm::mat4 accM = buildAccM(accPos_, accRotY_, accRotX_);

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

    if (LeftBody_.VAO != 0 && LeftBody_.vertexCount > 0)
    {
        const glm::vec3 left_kA(0.02f);
        const glm::vec3 left_kD(0.02f);
        const glm::vec3 left_kS(0.08f);
        const float left_shine = 64.0f;

        setPhongMaterial(up, left_shine, left_kA, left_kD, left_kS);

        glm::mat4 leftM = accM;
        leftM = leftM * glm::translate(glm::mat4(1.0f), leftBodyOffset_);
        if (leftBodyRotY_ != 0.0f) leftM = leftM * glm::rotate(glm::mat4(1.0f), leftBodyRotY_, glm::vec3(0, 1, 0));
        if (leftBodyRotX_ != 0.0f) leftM = leftM * glm::rotate(glm::mat4(1.0f), leftBodyRotX_, glm::vec3(1, 0, 0));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftM));

        glBindVertexArray(LeftBody_.VAO);
        glDrawArrays(GL_TRIANGLES, 0, LeftBody_.vertexCount);
        glBindVertexArray(0);
    }

    if (bassBtn_.VAO != 0 && bassBtn_.vertexCount > 0)
    {
        const float b_shine = 96.0f;

        const glm::mat4 bassGroup = buildBassGroup(accM, bassOffset_, bassRotY_, bassRotX_);

        glBindVertexArray(bassBtn_.VAO);

        for (int i = 0; i < bassN_; i++)
        {
            glm::vec3 kA, kD, kS;
            getBassMaterial(bassPress_[i], kA, kD, kS);
            setPhongMaterial(up, b_shine, kA, kD, kS);

            glm::mat4 M = buildButtonM(bassGroup, bassLocalPos_[i], bassPress_[i], pressDepth_);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));

            glDrawArrays(GL_TRIANGLES, 0, bassBtn_.vertexCount);
        }

        glBindVertexArray(0);
    }

    glBindVertexArray(btn_.VAO);

    const int trebleCount = totalRows_ * cols_;
    for (int i = 0; i < trebleCount; i++)
    {
        glm::mat4 M = buildButtonM(accM, localPos_[i], press_[i], pressDepth_);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));

        int r = i / cols_;
        int c = i % cols_;
        int baseR = r % baseRows_;
        NoteId n = layout_[baseR][c];

        glm::vec3 kA, kD, kS;
        getTrebleMaterial(n, press_[i], kA, kD, kS);

        setPhongMaterial(up, btn_shine, kA, kD, kS);

        glDrawArrays(GL_TRIANGLES, 0, btn_.vertexCount);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}