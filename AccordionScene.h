#pragma once
#include <vector>
#include "GLCommon.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ButtonCylinder.h"
#include "NoteData.h"



#include "RightBodyBox.h"
#include "LeftBodyBox.h"

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

    void renderPhong(GLuint phongShader, GLint modelLoc,
        const glm::vec3& body_kA, const glm::vec3& body_kD, const glm::vec3& body_kS, float body_shine,
        const glm::vec3& btn_kA, const glm::vec3& btn_kD, const glm::vec3& btn_kS, float btn_shine);

private:
    const TextureManager* textures_ = nullptr;

    // DESNA dugmad (treble)
    ButtonMesh btn_{};

    int baseRows_ = 3;
    int cols_ = 10;
    int totalRows_ = 6;
    int N_ = 60;      // ukupno renderovanih dugmadi (treble + klon)
    int baseN_ = 30;  // samo original treble

    std::vector<glm::vec3> localPos_; // za treble (60)
    std::vector<float> press_;        // za treble (60)

    // transform (cela harmonika)
    glm::vec3 accPos_ = glm::vec3(0, 0, 0);
    float accRotX_ = 0.0f;
    float accRotY_ = 0.0f;

    float pressDepth_ = 0.020f;

    // layout (desna ruka)
    NoteId layout_[3][10]{};

    RightBodyBoxMesh RightBody_{};
    LeftBodyBoxMesh LeftBody_{};

    // leva kutija tuning
    glm::vec3 leftBodyOffset_ = glm::vec3(0.0f);
    float leftBodyRotY_ = 0.0f;
    float leftBodyRotX_ = 0.0f;

    // ============================================================
    // BASOVI (leva ruka) - NOVO
    ButtonMesh bassBtn_{};

    int bassRows_ = 3;   // osnovni / dur / mol
    int bassCols_ = 8;   // F B C G D A E H
    int bassN_ = 24;     // 3*8

    // bas dugmad su dodatak preko postojećih 60:
    // index u pressTarget: bassStartIndex_ .. bassStartIndex_ + bassN_ - 1
    int bassStartIndex_ = 60;

    std::vector<glm::vec3> bassLocalPos_; // 24
    std::vector<float> bassPress_;        // 24

    // TUNING basova (lako pomeranje)
    glm::vec3 bassOffset_ = glm::vec3(0.10f, 1.20f, 0.18f); // pomeraj bas grupe u odnosu na accM
    float bassRotY_ = 0.0f;
    float bassRotX_ = 0.0f;
    // ============================================================
};
