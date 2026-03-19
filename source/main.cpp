#include <nds.h>
#include <stdio.h>
#include "gfx.h"
#include "snd.h"

int main() {
    // Init screens
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleDemoInit();

    bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    u16* fb = BG_GFX_SUB;

    appSndInit();

    Instrument inst = INST_GUITAR;
    drawInstrument(fb, inst);

    printf("\x1b[1;0HMusic Maker NDS");
    printf("\x1b[3;0HL/R = cambio strumento");
    printf("\x1b[4;0HTouch = suona");
    printf("\x1b[6;0HStrumento: GUITAR");

    bool wasTouching = false;
    int lastHit = -1;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u32 down = keysDown();
        u32 held = keysHeld();

        // Switch instrument
        if (down & (KEY_L | KEY_R)) {
            if      (inst == INST_GUITAR) inst = INST_BASS;
            else if (inst == INST_BASS)   inst = INST_DRUMS;
            else                          inst = INST_GUITAR;

            drawInstrument(fb, inst);
            lastHit = -1;

            printf("\x1b[6;0HStrumento: %s      ",
                inst == INST_GUITAR ? "GUITAR" :
                inst == INST_BASS   ? "BASS  " : "DRUMS ");
        }

        // Touch
        if (held & KEY_TOUCH) {
            touchPosition tp;
            touchRead(&tp);
            int hit = getHitZone(inst, tp.px, tp.py);

            if (hit >= 0 && (!wasTouching || hit != lastHit)) {
                playNote(inst, hit);
                highlightZone(fb, inst, hit);
                lastHit = hit;
            }
            wasTouching = true;
        } else {
            if (wasTouching) drawInstrument(fb, inst);
            wasTouching = false;
            lastHit = -1;
        }
    }
    return 0;
}
