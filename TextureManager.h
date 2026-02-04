#pragma once
#include <array>
#include "GLCommon.h"
#include "NoteData.h"

class TextureManager {
public:
    bool loadAll();
    GLuint tex(NoteId id) const { return noteTex_[id]; }

private:
    static GLuint preprocessTexture(const char* filepath);

private:
    std::array<GLuint, NOTE_COUNT> noteTex_{};
};
