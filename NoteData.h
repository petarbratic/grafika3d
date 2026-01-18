#pragma once
#include <vector>

// NOTE: ostavljam enum i layout 1:1 kao kod tebe
enum NoteId {
    F0, FSHARP0, G0, GSHARP0, A0, BFLAT0, B0,
    C, CSHARP, D, DSHARP, E, F, FSHARP, G, GSHARP, A, BB, B,
    C2, CSHARP2, D2, DSHARP2, E2, F2, FSHARP2, G2, GSHARP2, A2,
    NOTE_COUNT
};

static inline void getRightHandLayout(NoteId outLayout[3][10])
{
    NoteId layout[3][10] = {
        { FSHARP0, A0,     C,      DSHARP,  FSHARP,  A,      C2,      DSHARP2,  FSHARP2,  A2 },
        { G0,      BFLAT0, CSHARP, E,       G,       BB,     CSHARP2, E2,       G2,       BB },
        { F0,      GSHARP0,B0,     D,       F,       GSHARP, B,       D2,       F2,       GSHARP2 }
    };

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 10; c++)
            outLayout[r][c] = layout[r][c];
}

static inline void getNoteWavs(const char* outWav[NOTE_COUNT])
{
    const char* noteWav[NOTE_COUNT] = {
        "semplHarmonika/f0.wav",
        "semplHarmonika/f#0.wav",
        "semplHarmonika/g0.wav",
        "semplHarmonika/g#0.wav",
        "semplHarmonika/a0.wav",
        "semplHarmonika/b0.wav",
        "semplHarmonika/h0.wav",
        "semplHarmonika/C.wav",
        "semplHarmonika/c#.wav",
        "semplHarmonika/d.wav",
        "semplHarmonika/d#.wav",
        "semplHarmonika/e.wav",
        "semplHarmonika/f.wav",
        "semplHarmonika/f#.wav",
        "semplHarmonika/g.wav",
        "semplHarmonika/g#.wav",
        "semplHarmonika/a.wav",
        "semplHarmonika/Bb.wav",
        "semplHarmonika/h.wav",
        "semplHarmonika/c2.wav",
        "semplHarmonika/c#2.wav",
        "semplHarmonika/d2.wav",
        "semplHarmonika/d#2.wav",
        "semplHarmonika/e2.wav",
        "semplHarmonika/f2.wav",
        "semplHarmonika/f#2.wav",
        "semplHarmonika/g2.wav",
        "semplHarmonika/g#2.wav",
        "semplHarmonika/a2.wav"
    };

    for (int i = 0; i < NOTE_COUNT; i++) outWav[i] = noteWav[i];
}

// Napravi fajl putanju za svako dugme (0..29) na osnovu layout-a
static inline std::vector<const char*> buildButtonFilesFromLayout()
{
    NoteId layout[3][10];
    getRightHandLayout(layout);

    const char* wavs[NOTE_COUNT];
    getNoteWavs(wavs);

    std::vector<const char*> files(30, nullptr);
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 10; c++) {
            int i = r * 10 + c;
            files[i] = wavs[layout[r][c]];
        }
    }
    return files;
}
