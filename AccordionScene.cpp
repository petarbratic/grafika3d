#include "AccordionScene.h"
#include "TextureManager.h"
#include "Common.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void AccordionScene::init(const TextureManager* textures)
{
    textures_ = textures;

    // layout 1:1
    getRightHandLayout(layout_);

    // mesh 1:1
    btn_ = CreateButtonCylinder(
        0.06f,
        0.03f,
        32,
        { 0.9f, 0.9f, 0.9f, 1.0f }
    );

    N_ = totalRows_ * cols_;
    baseN_ = baseRows_ * cols_;

    localPos_.assign(N_, glm::vec3(0));
    press_.assign(N_, 0.0f);

    // pozicije 1:1
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
}

void AccordionScene::shutdown()
{
    DestroyButtonMesh(btn_);
}

void AccordionScene::updateRotationFromKeys(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  accRotY_ += 0.03f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) accRotY_ -= 0.03f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    accRotX_ += 0.03f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  accRotX_ -= 0.03f;
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
    (void)shader;

    glm::mat4 accM(1.0f);
    accM = glm::translate(accM, accPos_);
    accM = glm::rotate(accM, glm::radians(-90.0f), glm::vec3(0, 0, 1));
    accM = glm::rotate(accM, accRotY_, glm::vec3(0, 1, 0));
    accM = glm::rotate(accM, accRotX_, glm::vec3(1, 0, 0));

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

        glUniform1i(glGetUniformLocation(shader, "useTex"), 1);
        glUniform1i(glGetUniformLocation(shader, "transparent"), 1);

        glDrawArrays(GL_TRIANGLES, 0, btn_.vertexCount);
    }

    glBindVertexArray(0);
}
