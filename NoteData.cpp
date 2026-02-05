#include "NoteData.h"

namespace
{
    // Desna ruka: fiksni layout 3x10
    constexpr NoteId kRightHandLayout[3][10] = {
        { FSHARP0, A0,      C,       DSHARP,  FSHARP,  A,      C2,      DSHARP2,  FSHARP2,  A2 },
        { G0,      BFLAT0,  CSHARP,  E,       G,       BB,     CSHARP2, E2,       G2,       BB },
        { F0,      GSHARP0, B0,      D,       F,       GSHARP, B,       D2,       F2,       GSHARP2 }
    };

    // NOTE wav putanje (NOTE_COUNT = 29)
    constexpr const char* kNoteWavs[NOTE_COUNT] = {
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

    // BASS wav putanje (BASS_COUNT = 24)
    constexpr const char* kBassWavs[BASS_COUNT] = {
        // osnovni
        "semplHarmonika/basF.wav",
        "semplHarmonika/basB.wav",
        "semplHarmonika/basC.wav",
        "semplHarmonika/basG.wav",
        "semplHarmonika/basD.wav",
        "semplHarmonika/basA.wav",
        "semplHarmonika/basE.wav",
        "semplHarmonika/basH.wav",

        // dur
        "semplHarmonika/basFdur.wav",
        "semplHarmonika/basBdur.wav",
        "semplHarmonika/basCdur.wav",
        "semplHarmonika/basGdur.wav",
        "semplHarmonika/basDdur.wav",
        "semplHarmonika/basAdur.wav",
        "semplHarmonika/basEdur.wav",
        "semplHarmonika/basHdur.wav",

        // mol
        "semplHarmonika/basFmol.wav",
        "semplHarmonika/basBmol.wav",
        "semplHarmonika/basCmol.wav",
        "semplHarmonika/basGmol.wav",
        "semplHarmonika/basDmol.wav",
        "semplHarmonika/basAmol.wav",
        "semplHarmonika/basEmol.wav",
        "semplHarmonika/basHmol.wav",
    };
}

void getRightHandLayout(NoteId outLayout[3][10])
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 10; c++)
            outLayout[r][c] = kRightHandLayout[r][c];
}

void getNoteWavs(const char* outWav[NOTE_COUNT])
{
    for (int i = 0; i < NOTE_COUNT; i++)
        outWav[i] = kNoteWavs[i];
}

std::vector<const char*> buildButtonFilesFromLayout()
{
    NoteId layout[3][10];
    getRightHandLayout(layout);

    const char* wavs[NOTE_COUNT];
    getNoteWavs(wavs);

    std::vector<const char*> files(30, nullptr);
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 10; c++) {
            int idx = r * 10 + c;           // 0..29
            files[idx] = wavs[layout[r][c]];
        }
    }
    return files;
}

void getBassWavs(const char* outWav[BASS_COUNT])
{
    for (int i = 0; i < BASS_COUNT; i++)
        outWav[i] = kBassWavs[i];
}

std::vector<const char*> buildBassFiles()
{
    const char* wavs[BASS_COUNT];
    getBassWavs(wavs);

    std::vector<const char*> files(BASS_COUNT, nullptr);
    for (int i = 0; i < BASS_COUNT; i++)
        files[i] = wavs[i];

    return files;
}