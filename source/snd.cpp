#include "snd.h"
#include <nds.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define RATE 16384
#define MAX_BUF 32768

static s8 bufG[6][MAX_BUF]; static u32 szG[6];
static s8 bufB[4][MAX_BUF]; static u32 szB[4];
static s8 bufD[4][MAX_BUF]; static u32 szD[4];

static void genString(s8* out, u32* sz, float freq, float dur, float damp) {
    u32 n = (u32)(RATE * dur);
    if (n > MAX_BUF) n = MAX_BUF;
    *sz = n;
    int dl = (int)(RATE / freq);
    if (dl < 2) dl = 2;
    float* line = (float*)malloc(dl * sizeof(float));
    for (int i = 0; i < dl; i++)
        line[i] = ((rand() % 256) - 128) / 128.0f;
    int ptr = 0; float prev = 0;
    for (u32 i = 0; i < n; i++) {
        float cur = line[ptr];
        float nv = (cur + prev) * 0.5f * damp;
        line[ptr] = nv; prev = nv;
        float o = cur * 120.0f;
        if (o >  127) o =  127;
        if (o < -128) o = -128;
        out[i] = (s8)o;
        ptr = (ptr + 1) % dl;
    }
    free(line);
}

static void genDrum(s8* out, u32* sz, int type) {
    u32 n;
    if      (type == 0) n = (u32)(RATE * 0.25f);
    else if (type == 1) n = (u32)(RATE * 0.20f);
    else if (type == 2) n = (u32)(RATE * 0.08f);
    else                n = (u32)(RATE * 1.2f);
    if (n > MAX_BUF) n = MAX_BUF;
    *sz = n;
    float phase = 0, freq = (type == 0) ? 140.0f : 200.0f;
    for (u32 i = 0; i < n; i++) {
        float env = 1.0f - (float)i / n;
        env = env * env * (type == 3 ? 1.0f : env);
        float noise = ((rand() % 256) - 128) / 128.0f;
        float val;
        if (type == 0) {
            val = sinf(phase) * 127.0f * env;
            phase += 2.0f * 3.14159f * freq / RATE;
            freq *= 0.998f;
        } else if (type == 1) {
            val = (noise * 0.7f + sinf(phase) * 0.3f) * 90.0f * env;
            phase += 2.0f * 3.14159f * freq / RATE;
        } else {
            val = noise * 70.0f * env;
        }
        if (val >  127) val =  127;
        if (val < -128) val = -128;
        out[i] = (s8)val;
    }
}

void appSndInit() {
    soundEnable();
    // Guitar E2 A2 D3 G3 B3 E4
    float gf[6] = {82.4f, 110.0f, 146.8f, 196.0f, 246.9f, 329.6f};
    for (int i = 0; i < 6; i++) genString(bufG[i], &szG[i], gf[i], 1.8f, 0.995f);
    // Bass E1 A1 D2 G2
    float bf[4] = {41.2f, 55.0f, 73.4f, 98.0f};
    for (int i = 0; i < 4; i++) genString(bufB[i], &szB[i], bf[i], 2.0f, 0.999f);
    // Drums
    for (int i = 0; i < 4; i++) genDrum(bufD[i], &szD[i], i);
}

void playNote(Instrument inst, int idx) {
    if (inst == INST_GUITAR && idx < 6)
        soundPlaySample(bufG[idx], SoundFormat_8Bit, szG[idx], RATE, 127, 64, false, 0);
    else if (inst == INST_BASS && idx < 4)
        soundPlaySample(bufB[idx], SoundFormat_8Bit, szB[idx], RATE, 127, 64, false, 0);
    else if (inst == INST_DRUMS && idx < 4)
        soundPlaySample(bufD[idx], SoundFormat_8Bit, szD[idx], RATE, 127, 64, false, 0);
}
