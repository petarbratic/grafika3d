// Autori: Nedeljko Tesanovic i Vasilije Markovic
// Opis: Harmonika dugmetara – demo sa teksturama nota + Audio (irrKlang)

#include <thread>
#include <chrono>

#include <iostream>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"
#include "Audio.h"

#include "AppContext.h"
#include "Common.h"
#include "NoteData.h"
#include "TextureManager.h"
#include "KeyInput.h"
#include "AccordionScene.h"


static void framebufferSizeCallback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

int main(void)
{
    if (!glfwInit()) {
        std::cout << "GLFW nije ucitan\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(
        mode->width,
        mode->height,
        "Harmonika dugmetara",
        monitor,
        nullptr
    );

    if (!window) {
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW greska\n";
        glfwTerminate();
        return 3;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glViewport(0, 0, mode->width, mode->height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    AppContext ctx;

    ctx.audioOk = Audio::init();
    if (!ctx.audioOk) {
        std::cout << "Audio init nije uspeo (irrKlang)\n";
    }

    unsigned int shader = createShader("basic.vert", "basic.frag");
    unsigned int phongShader = createShader("phong.vert", "phong.frag");
    unsigned int overlayShader = createShader("overlay.vert", "overlay.frag");

    int modelLocP = glGetUniformLocation(phongShader, "uM");
    int viewLocP = glGetUniformLocation(phongShader, "uV");
    int projLocP = glGetUniformLocation(phongShader, "uP");

    int viewPosLoc = glGetUniformLocation(phongShader, "uViewPos");

    int lightPosLoc = glGetUniformLocation(phongShader, "uLight.pos");
    int lightALoc = glGetUniformLocation(phongShader, "uLight.kA");
    int lightDLoc = glGetUniformLocation(phongShader, "uLight.kD");
    int lightSLoc = glGetUniformLocation(phongShader, "uLight.kS");

    int matShineLoc = glGetUniformLocation(phongShader, "uMaterial.shine");
    int matALoc = glGetUniformLocation(phongShader, "uMaterial.kA");
    int matDLoc = glGetUniformLocation(phongShader, "uMaterial.kD");
    int matSLoc = glGetUniformLocation(phongShader, "uMaterial.kS");

    int modelLoc = glGetUniformLocation(shader, "uM");
    int viewLoc = glGetUniformLocation(shader, "uV");
    int projLoc = glGetUniformLocation(shader, "uP");

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "uTex"), 0);
    glUseProgram(0);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 2.2f),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    float aspect = (mode->height != 0) ? (float)mode->width / (float)mode->height : 1.0f;
    glm::mat4 proj = glm::perspective(
        glm::radians(70.0f),
        aspect,
        0.1f,
        100.0f
    );

    glUseProgram(phongShader);
    glUniformMatrix4fv(viewLocP, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLocP, 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(viewPosLoc, 0.0f, 0.0f, 2.2f);

    glUniform3f(lightPosLoc, 0.0f, 0.0f, 2.2f);
    glUniform3f(lightALoc, 0.25f, 0.25f, 0.25f);
    glUniform3f(lightDLoc, 0.85f, 0.85f, 0.85f);
    glUniform3f(lightSLoc, 1.0f, 1.0f, 1.0f);
    glUseProgram(0);

    glUseProgram(shader);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);

    TextureManager textures;
    textures.loadAll();

    AccordionScene scene;
    scene.init(&textures);

    static int keyMap[3][10] = {
        { GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P },
        { GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_SEMICOLON },
        { GLFW_KEY_TAB, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_B, GLFW_KEY_N, GLFW_KEY_M, GLFW_KEY_COMMA, GLFW_KEY_PERIOD }
    };

    static int keyMap2[3][10] = {
        { GLFW_KEY_A, GLFW_KEY_X, GLFW_KEY_V, GLFW_KEY_H, GLFW_KEY_K, GLFW_KEY_PERIOD, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE },
        { GLFW_KEY_Z, GLFW_KEY_D, GLFW_KEY_G, GLFW_KEY_N, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_BACKSPACE, GLFW_KEY_SLASH, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE },
        { GLFW_KEY_TAB, GLFW_KEY_S, GLFW_KEY_C, GLFW_KEY_B, GLFW_KEY_M, GLFW_KEY_L, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_PERIOD }
    };

    ctx.bassKeyBtn_basic[0] = GLFW_KEY_8;
    ctx.bassKeyBtn_basic[1] = GLFW_KEY_9;
    ctx.bassKeyBtn_basic[2] = GLFW_KEY_7;
    ctx.bassKeyBtn_basic[3] = GLFW_KEY_6;
    ctx.bassKeyBtn_basic[4] = GLFW_KEY_5;
    ctx.bassKeyBtn_basic[5] = GLFW_KEY_4;
    ctx.bassKeyBtn_basic[6] = GLFW_KEY_3;
    ctx.bassKeyBtn_basic[7] = GLFW_KEY_2;

    ctx.bassKeyBtn_dur[0] = GLFW_KEY_F8;
    ctx.bassKeyBtn_dur[1] = GLFW_KEY_F9;
    ctx.bassKeyBtn_dur[2] = GLFW_KEY_F7;
    ctx.bassKeyBtn_dur[3] = GLFW_KEY_F6;
    ctx.bassKeyBtn_dur[4] = GLFW_KEY_F5;
    ctx.bassKeyBtn_dur[5] = GLFW_KEY_F4;
    ctx.bassKeyBtn_dur[6] = GLFW_KEY_F3;
    ctx.bassKeyBtn_dur[7] = GLFW_KEY_F2;

    ctx.bassKeyPiano_basic[0] = GLFW_KEY_I;
    ctx.bassKeyPiano_basic[1] = GLFW_KEY_O;
    ctx.bassKeyPiano_basic[2] = GLFW_KEY_U;
    ctx.bassKeyPiano_basic[3] = GLFW_KEY_Y;
    ctx.bassKeyPiano_basic[4] = GLFW_KEY_T;
    ctx.bassKeyPiano_basic[5] = GLFW_KEY_R;
    ctx.bassKeyPiano_basic[6] = GLFW_KEY_E;
    ctx.bassKeyPiano_basic[7] = GLFW_KEY_W;

    ctx.bassKeyPiano_dur[0] = GLFW_KEY_8;
    ctx.bassKeyPiano_dur[1] = GLFW_KEY_9;
    ctx.bassKeyPiano_dur[2] = GLFW_KEY_7;
    ctx.bassKeyPiano_dur[3] = GLFW_KEY_6;
    ctx.bassKeyPiano_dur[4] = GLFW_KEY_5;
    ctx.bassKeyPiano_dur[5] = GLFW_KEY_4;
    ctx.bassKeyPiano_dur[6] = GLFW_KEY_3;
    ctx.bassKeyPiano_dur[7] = GLFW_KEY_2;

    ctx.bassKeyPiano_mol[0] = GLFW_KEY_F8;
    ctx.bassKeyPiano_mol[1] = GLFW_KEY_F9;
    ctx.bassKeyPiano_mol[2] = GLFW_KEY_F7;
    ctx.bassKeyPiano_mol[3] = GLFW_KEY_F6;
    ctx.bassKeyPiano_mol[4] = GLFW_KEY_F5;
    ctx.bassKeyPiano_mol[5] = GLFW_KEY_F4;
    ctx.bassKeyPiano_mol[6] = GLFW_KEY_F3;
    ctx.bassKeyPiano_mol[7] = GLFW_KEY_F2;

    ctx.baseRows = scene.baseRows();
    ctx.cols = scene.cols();
    ctx.baseN = scene.baseButtons();

    ctx.keyMapBtn = keyMap;
    ctx.keyMapPiano = keyMap2;

    ctx.buttonFiles = buildButtonFilesFromLayout();
    ctx.bassFiles = buildBassFiles();

    ctx.pressTarget.assign(scene.totalButtons(), 0.0f);

    KeyInput::applyKeyMapping(ctx, false);

    glfwSetWindowUserPointer(window, &ctx);
    glfwSetKeyCallback(window, KeyInput::keyCallback);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    double lastTime = glfwGetTime();

    auto setMaterial = [&](float shine, glm::vec3 kA, glm::vec3 kD, glm::vec3 kS) {
        glUniform1f(matShineLoc, shine);
        glUniform3f(matALoc, kA.x, kA.y, kA.z);
        glUniform3f(matDLoc, kD.x, kD.y, kD.z);
        glUniform3f(matSLoc, kS.x, kS.y, kS.z);
        };

    glm::vec3 body_kA(0.02f);
    glm::vec3 body_kD(0.05f);
    glm::vec3 body_kS(0.15f);
    float body_shine = 64.0f;

    glm::vec3 btn_kA(0.10f);
    glm::vec3 btn_kD(0.75f);
    glm::vec3 btn_kS(0.30f);
    float btn_shine = 96.0f;

    const double TARGET_FPS = 75.0;
    const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    unsigned int nameTex = loadImageToTexture("media/ime.png");
    
    if (nameTex != 0) {
        glBindTexture(GL_TEXTURE_2D, nameTex);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glUseProgram(overlayShader);
    glUniform1i(glGetUniformLocation(overlayShader, "uTex"), 0);
    glUseProgram(0);

    float w = 0.35f;
    float h = w * (117.0f / 230.0f);
    float x2 = 1.0f;
    float y1 = -1.0f;
    float x1 = x2 - w;
    float y2 = y1 + h;

    float overlayVerts[] = {
        x1, y1,  0.0f, 0.0f,
        x2, y1,  1.0f, 0.0f,
        x2, y2,  1.0f, 1.0f,

        x1, y1,  0.0f, 0.0f,
        x2, y2,  1.0f, 1.0f,
        x1, y2,  0.0f, 1.0f
    };

    unsigned int overlayVAO = 0, overlayVBO = 0;
    glGenVertexArrays(1, &overlayVAO);
    glGenBuffers(1, &overlayVBO);

    glBindVertexArray(overlayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVerts), overlayVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;
        dt = clampf(dt, 0.0f, 0.05f);

        scene.updateRotationFromKeys(window);
        scene.updatePress(dt, ctx.pressTarget);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(phongShader);

        scene.renderPhong(phongShader, modelLocP,
            body_kA, body_kD, body_kS, body_shine,
            btn_kA, btn_kD, btn_kS, btn_shine);

        glUseProgram(0);

        if (nameTex != 0) {
            glDisable(GL_DEPTH_TEST);

            glUseProgram(overlayShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, nameTex);

            glBindVertexArray(overlayVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);

            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        double frameEnd = glfwGetTime();
        double frameTime = frameEnd - now;
        if (frameTime < TARGET_FRAME_TIME) {
            double sleepSec = TARGET_FRAME_TIME - frameTime;
            if (sleepSec > 0.0) {
                std::this_thread::sleep_for(
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::duration<double>(sleepSec)
                    )
                );
            }
        }
    }

    Audio::shutdown();

    if (nameTex != 0) glDeleteTextures(1, &nameTex);

    glDeleteBuffers(1, &overlayVBO);
    glDeleteVertexArrays(1, &overlayVAO);

    glDeleteProgram(overlayShader);
    glDeleteProgram(phongShader);
    glDeleteProgram(shader);

    scene.shutdown();

    glfwTerminate();
    return 0;
}
