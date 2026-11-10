#include "mbed.h"
#include "C12832.h"
#include "LM75B.h"

C12832 lcd(p5, p7, p6, p8, p11);
LM75B   tmp(p28, p27, 0x90);   // samma adress som funkade hos dig

#define W    128
#define H    32
#define X0   10
#define Y0   28
#define TMIN 15.0f
#define TMAX 35.0f

static int map_y(float t) {
    float f = (t - TMIN) / (TMAX - TMIN);
    if (f < 0) f = 0;
    if (f > 1) f = 1;
    int y = (int)(Y0 - f * (Y0 - 2));   // 2..Y0
    if (y < 2)  y = 2;
    if (y > Y0) y = Y0;
    return y;
}

static void axes() {
    lcd.cls();
    lcd.line(X0, 2,  X0, Y0, 1);     // y-axel
    lcd.line(X0, Y0, W-2, Y0, 1);    // x-axel
    lcd.locate(0,  0);  lcd.printf("35");
    lcd.locate(0, 26); lcd.printf("15");
    lcd.locate(20, 0);  lcd.printf("Temp (C)");
}

int main() {
    const float dt = 1.0f;   // 1 punkt/sek
    while (1) {
        axes();
        for (int x = X0 + 1; x < W - 2; x++) {
            float t = tmp.temp();           // läs °C
            int   y = map_y(t);

            lcd.pixel(x, y, 1);             // rita EN punkt
            lcd.locate(20, 16);
            lcd.printf("Now: %4.1fC  ", (double)t);

            wait(dt);
        }
        // när vi når kanten startar svepet om med nya axlar
    }
}
