#pragma once
#include <nds.h>

typedef enum { INST_GUITAR, INST_BASS, INST_DRUMS } Instrument;

void drawInstrument(u16* fb, Instrument inst);
void highlightZone(u16* fb, Instrument inst, int zone);
int  getHitZone(Instrument inst, int x, int y);
