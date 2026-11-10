#include "mbed.h"


InterruptIn button(p14); // joystick center
Timeout timeout; // fördröjd händelse
Ticker ticker;   // för 1 Hz uppdatering

int remaining = 0;
bool running = false;

// körs varje sekund → visa återstående tid
void tick() {
    if (running) {
        printf("Tid kvar: %d s\n", remaining);
        remaining--;
        if (remaining < 0) ticker.detach(); // stoppa tickern när klar
    }
}

// körs när tiden är slut
void done() {
    running = false;
    printf("Tiden är ute!\n");
}

// när knappen trycks
void start_delay() {
    if (!running) {
        remaining = 5 + rand() % 6; // slump mellan 5–10 s
        printf("Startar fördröjning på %d s\n", remaining);
        running = true;
        ticker.attach(&tick, 1.0); // uppdatera varje sekund
        timeout.attach(&done, remaining); // kör när tiden är ute
    }
}

int main() {
    srand(time(NULL)); // initiera slump
    button.rise(&start_delay);
    printf("Tryck på joystick för att starta timer.\n");
    while (true) { } // gör inget mer
}
