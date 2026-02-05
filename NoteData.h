#pragma once
#include <vector>

// NOTE: ostavljam enum i layout 1:1 kao kod tebe
enum NoteId {
    F0, FSHARP0, G0, GSHARP0, A0, BFLAT0, B0,
    C, CSHARP, D, DSHARP, E, F, FSHARP, G, GSHARP, A, BB, B,
    C2, CSHARP2, D2, DSHARP2, E2, F2, FSHARP2, G2, GSHARP2, A2,
    NOTE_COUNT
};

// Popunjava 3x10 mapu (desna ruka)
void getRightHandLayout(NoteId outLayout[3][10]);

// Popunjava niz putanja za NOTE_COUNT tonova
void getNoteWavs(const char* outWav[NOTE_COUNT]);

// Napravi fajl putanju za svako dugme (0..29) na osnovu layout-a
std::vector<const char*> buildButtonFilesFromLayout();

// ===========================
// BASS (leva ruka) - 8 * 3
// redosled: f b c g d a e h
// ===========================

enum BassId
{
    BAS_B, BAS_F, BAS_C, BAS_G, BAS_D, BAS_A, BAS_E, BAS_H,          // osnovni
    BAS_B_DUR, BAS_F_DUR, BAS_C_DUR, BAS_G_DUR, BAS_D_DUR, BAS_A_DUR, BAS_E_DUR, BAS_H_DUR, // dur
    BAS_B_MOL, BAS_F_MOL, BAS_C_MOL, BAS_G_MOL, BAS_D_MOL, BAS_A_MOL, BAS_E_MOL, BAS_H_MOL, // mol
    BASS_COUNT
};

// Popunjava niz putanja za BASS_COUNT basova
void getBassWavs(const char* outWav[BASS_COUNT]);

// Vraća vektor putanja u istom redosledu kao BassId (velicina BASS_COUNT)
std::vector<const char*> buildBassFiles();