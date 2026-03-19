#include "gfx.h"

static void fillRect(u16* fb, int x, int y, int w, int h, u16 col) {
    for (int j = y; j < y+h && j < 192; j++)
        for (int i = x; i < x+w && i < 256; i++)
            fb[j*256+i] = col;
}

static void fillScreen(u16* fb, u16 col) {
    for (int i = 0; i < 256*192; i++) fb[i] = col;
}

// Guitar: 6 horizontal strings
static const int GUITAR_Y[6] = {20, 50, 80, 110, 140, 165};
static const u16 STR_COLOR = ARGB16(1,25,25,25);
static const u16 BG_GUITAR  = ARGB16(1,6,3,1);

// Bass: 4 thick strings
static const int BASS_Y[4]   = {30, 75, 120, 160};
static const u16 BG_BASS    = ARGB16(1,3,2,1);

// Drums: 4 pads
static const u16 PAD_COLORS[4] = {
    ARGB16(1,28,4,4),   // kick  - red
    ARGB16(1,4,28,4),   // snare - green
    ARGB16(1,4,10,28),  // hihat - blue
    ARGB16(1,28,28,4),  // crash - yellow
};
static const u16 PAD_HI[4] = {
    ARGB16(1,31,16,16),
    ARGB16(1,16,31,16),
    ARGB16(1,16,20,31),
    ARGB16(1,31,31,16),
};
static const int PAD_X[4] = {10, 136, 10, 136};
static const int PAD_Y[4] = {10, 10, 100, 100};
static const int PAD_W = 110, PAD_H = 80;

void drawInstrument(u16* fb, Instrument inst) {
    if (inst == INST_GUITAR) {
        fillScreen(fb, BG_GUITAR);
        for (int i = 0; i < 6; i++)
            fillRect(fb, 0, GUITAR_Y[i], 256, 5, STR_COLOR);
    } else if (inst == INST_BASS) {
        fillScreen(fb, BG_BASS);
        for (int i = 0; i < 4; i++)
            fillRect(fb, 0, BASS_Y[i], 256, 8, STR_COLOR);
    } else {
        fillScreen(fb, ARGB16(1,4,4,5));
        for (int i = 0; i < 4; i++)
            fillRect(fb, PAD_X[i], PAD_Y[i], PAD_W, PAD_H, PAD_COLORS[i]);
    }
}

void highlightZone(u16* fb, Instrument inst, int zone) {
    drawInstrument(fb, inst);
    if (inst == INST_GUITAR)
        fillRect(fb, 0, GUITAR_Y[zone]-2, 256, 9, ARGB16(1,31,31,20));
    else if (inst == INST_BASS)
        fillRect(fb, 0, BASS_Y[zone]-2, 256, 12, ARGB16(1,31,31,20));
    else
        fillRect(fb, PAD_X[zone], PAD_Y[zone], PAD_W, PAD_H, PAD_HI[zone]);
}

int getHitZone(Instrument inst, int x, int y) {
    (void)x;
    if (inst == INST_GUITAR) {
        for (int i = 0; i < 6; i++)
            if (y >= GUITAR_Y[i]-12 && y <= GUITAR_Y[i]+12) return i;
    } else if (inst == INST_BASS) {
        for (int i = 0; i < 4; i++)
            if (y >= BASS_Y[i]-15 && y <= BASS_Y[i]+15) return i;
    } else {
        for (int i = 0; i < 4; i++)
            if (x >= PAD_X[i] && x < PAD_X[i]+PAD_W &&
                y >= PAD_Y[i] && y < PAD_Y[i]+PAD_H) return i;
    }
    return -1;
}
