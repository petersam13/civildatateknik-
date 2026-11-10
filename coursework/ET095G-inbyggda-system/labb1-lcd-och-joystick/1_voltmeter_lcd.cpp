/* ET095G – Exercise 5: LCD – Sub-task 1 (Display texts)
   Syfte: Visa spänning från potentiometern på C12832-LCD.
   Hårdvara: LPC1768 + Application Board (C12832)
   Bibliotek: C12832 (importerat i projektet)
   Not: Max spänning = 3.3 V
*/

#include "mbed.h"
#include "C12832.h"

C12832 lcd(p5, p7, p6, p8, p11);   // standardkoppling för C12832
AnalogIn pot(p19);                 // potentiometer (0..3.3V)

static inline void center_text(const char* s, int y) {
    int len = 0; while (s[len] != '\0') len++;
    int px = 64 - (len * 6) / 2; if (px < 0) px = 0;
    lcd.locate(px, y); lcd.printf("%s", s);
}

int main() {
    lcd.cls();
    center_text("LCD Task 1: Voltmeter", 0);
    wait_ms(800);

    while (true) {
        float v = pot.read() * 3.3f;   // 0..1 -> 0..3.3V
        lcd.cls();
        lcd.locate(0, 0);  lcd.printf("Pot voltage:");
        lcd.locate(0, 12); lcd.printf("%.2f V", v);

        // enkel stapel (0..120 px)
        int bar = (int)(v / 3.3f * 120.0f);
        if (bar < 0) bar = 0; if (bar > 120) bar = 120;
        lcd.line(0, 28, bar, 28, 1);

        wait_ms(400);
    }
}
