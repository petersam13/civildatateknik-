#include "mbed.h"

// RGB-LED på application board (vanligen p23, p24, p25)
PwmOut ledR(p23), ledG(p24), ledB(p25);

// sätt till true om din LED är aktiv-låg
static const bool INVERT = true;

static inline float apply_polarity(float x) {
    return INVERT ? (1.0f - x) : x;
}

int main() {
    // PWM-frekvens ca 1 kHz
    ledR.period_ms(1);
    ledG.period_ms(1);
    ledB.period_ms(1);

    const int steps = 200;  // antal steg för fade
    const int step_ms = 10; // varje steg = 10 ms → 2 s upp/ned

    while (1) {
        // Fade in
        for (int i = 0; i <= steps; i++) {
            float d = (float)i / steps;       // 0–1
            float w = apply_polarity(d);
            ledR.write(w);
            ledG.write(w);
            ledB.write(w);
            wait_ms(step_ms);          
        }

        // Fade out
        for (int i = steps; i >= 0; i--) {
            float d = (float)i / steps;       // 1–0
            float w = apply_polarity(d);
            ledR.write(w);
            ledG.write(w);
            ledB.write(w);
            wait_ms(step_ms);
        }
    }
}
