#include "TextureManager.h"
#include <iostream>
#include "Util.h"

GLuint TextureManager::preprocessTexture(const char* filepath)
{
    GLuint tex = loadImageToTexture(filepath);
    if (tex == 0) {
        std::cout << "Neuspesno ucitana textura: " << filepath << "\n";
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

bool TextureManager::loadAll()
{
    // Ucitavamo osnovne slike
    GLuint texC = preprocessTexture("notes/C.png");
    GLuint texCsharp = preprocessTexture("notes/c#.png");
    GLuint texD = preprocessTexture("notes/d.png");
    GLuint texDsharp = preprocessTexture("notes/d#.png");
    GLuint texE = preprocessTexture("notes/e.png");
    GLuint texF = preprocessTexture("notes/f.png");
    GLuint texFsharp = preprocessTexture("notes/f#.png");
    GLuint texG = preprocessTexture("notes/g.png");
    GLuint texGsharp = preprocessTexture("notes/g#.png");
    GLuint texA = preprocessTexture("notes/a.png");
    GLuint texBflat = preprocessTexture("notes/Bb.png");
    GLuint texB = preprocessTexture("notes/h.png");

    // Mapiranje kao u tvom main-u
    noteTex_[F0] = texF;
    noteTex_[FSHARP0] = texFsharp;
    noteTex_[G0] = texG;
    noteTex_[GSHARP0] = texGsharp;
    noteTex_[A0] = texA;
    noteTex_[BFLAT0] = texBflat;
    noteTex_[B0] = texB;

    noteTex_[C] = texC;
    noteTex_[CSHARP] = texCsharp;
    noteTex_[D] = texD;
    noteTex_[DSHARP] = texDsharp;
    noteTex_[E] = texE;
    noteTex_[F] = texF;
    noteTex_[FSHARP] = texFsharp;
    noteTex_[G] = texG;
    noteTex_[GSHARP] = texGsharp;
    noteTex_[A] = texA;
    noteTex_[BB] = texBflat;
    noteTex_[B] = texB;

    noteTex_[C2] = texC;
    noteTex_[CSHARP2] = texCsharp;
    noteTex_[D2] = texD;
    noteTex_[DSHARP2] = texDsharp;
    noteTex_[E2] = texE;
    noteTex_[F2] = texF;
    noteTex_[FSHARP2] = texFsharp;
    noteTex_[G2] = texG;
    noteTex_[GSHARP2] = texGsharp;
    noteTex_[A2] = texA;

    return true;
}
