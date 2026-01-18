#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ButtonCylinder.h"
#include "NoteData.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class TextureManager;

class AccordionScene
{
public:
    void init(const TextureManager* textures);
    void shutdown();

    int totalButtons() const { return N_; }
    int baseButtons() const { return baseN_; }
    int cols() const { return cols_; }
    int baseRows() const { return baseRows_; }

    void updatePress(float dt, const std::vector<float>& pressTarget);
    void updateRotationFromKeys(GLFWwindow* window);

    void render(GLuint shader, GLint modelLoc);

private:
    const TextureManager* textures_ = nullptr;

    ButtonMesh btn_{};

    int baseRows_ = 3;
    int cols_ = 10;
    int totalRows_ = 6;
    int N_ = 60;
    int baseN_ = 30;

    std::vector<glm::vec3> localPos_;
    std::vector<float> press_;

    // transform
    glm::vec3 accPos_ = glm::vec3(0, 0, 0);
    float accRotX_ = 0.0f;
    float accRotY_ = 0.0f;

    float pressDepth_ = 0.025f;

    // layout
    NoteId layout_[3][10]{};
};
