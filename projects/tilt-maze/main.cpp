#include "mbed.h"
#include "C12832.h"
#include "MMA7660.h"

// ==============================
//  Hårdvara
// ==============================
PwmOut speaker(p26);

//led lamporna 
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);


// LCD-skärm (krav: användning av tidigare hårdvaru-resurser)
C12832 lcd(p5, p7, p6, p8, p11);

// Accelerometer (krav: accelerometern måste användas)
MMA7660 acc(p28, p27);

// Potentiometer / AnalogIn (krav: ADC-läsning måste påverka spelet)
AnalogIn pot(p19);

// Startknapp (användning av digital input)
InterruptIn Start(p14);

// Timer (krav: tidshantering)
Timer timer;


// ==============================
// Variabler
// ==============================

// Accelerometerdata
float x, y, z;

// Spelobjekt
int ballX, ballY;
int goalX, goalY;

// ADC-values som påverkar gameplay (t.ex. svårighet)
float difficultyScale;

// Spelstatus
bool playing = false;
int roundNumber = 1;

// Poängsystem (krav: spelet ska registrera score)
int score = 0;


// ==============================
// Startmeny
// Krav: spelet ska ha en tydlig startmeny
// ==============================
void start_menu() {
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Tilt Maze - ET095G");
    
    lcd.locate(0,10);
    lcd.printf("5 Round Challenge");

    lcd.locate(0,20);
    lcd.printf("Tryck joystick START");
}

void play_tone(float freq, float duration) {
    speaker.period(1.0f / freq);
    speaker = 0.5f;
    wait(duration);
    speaker = 0.0f;
}

// ==============================
// Vinn-ruta per runda
// ==============================
void win_round_screen(int r) {
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Runda %d klar!", r);

    lcd.locate(0,12);
    lcd.printf("Score: %d", score);
    // Ljud när man vinner en runda
    play_tone(900, 0.15);
    play_tone(1100, 0.15);
    play_tone(1300, 0.25);


    wait(1.5);
}


// ==============================
// Förlorad runda
// ==============================
void lose_screen() {
    lcd.cls();
    lcd.locate(0,5);
    lcd.printf("Du forlorade, lol xdd");
    lcd.locate(0,15);
    lcd.printf("Tiden gick ut.");
    // WHOMP WHOMP ljud
    play_tone(200, 0.25);
    play_tone(150, 0.25);
    wait(0.1);
    play_tone(120, 0.35);


    wait(2.0);
}


// ==============================
// Slutskärm (krav: end screen)
// ==============================
void final_win_screen() {

    // Blink med alla LED-lampor
    for (int i = 0; i < 8; i++) {
        led1 = 1;
        led2 = 1;
        led3 = 1;
        led4 = 1;
        wait(0.1);

        led1 = 0;
        led2 = 0;
        led3 = 0;
        led4 = 0;
        wait(0.1);
    }

    // Släck alla LEDs efter blink
    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;

    // Stor vinstmelodi
    play_tone(700, 0.2);
    play_tone(900, 0.2);
    play_tone(1100, 0.3);
    play_tone(1500, 0.4);


    // Vanliga slutskärmen
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("GRATTIS!");
    lcd.locate(0,10);
    lcd.printf("Du klarade allt!");

    lcd.locate(0,22);
    lcd.printf("Final Score: %d", score);

    wait(3.0);
}


// ==============================
// Start av ny runda
// ==============================
void start_round(int r) {
    playing = true;

    // Startposition
    ballX = 10;
    ballY = 10;

    // Målposition (slumpad)
    goalX = 40 + (rand() % 80);
    goalY = 5  + (rand() % 20);

    // Läs ADC (krav: användning av sensor med ADC)
    // Potentiometern styr svårigheten:
    // 0.0 = enkel, 1.0 = extremt snabb
    difficultyScale = 1.0f + pot.read() * 2.0f;

    timer.reset();
    timer.start();
}


// ==============================
// Programstart
// ==============================
int main() {

    // Aktivera accelerometern (krav)
    acc.setActive(true);
    acc.setSampleRate(32);

    // Seed till random
    srand(52);

    start_menu();

    while (true) {

        // =========================
        // STARTMENY
        // =========================
        if (!playing && roundNumber == 1) {
            if (Start.read() == 1) {
                wait(0.25);
                score = 0;          // reset score vid nytt spel
                start_round(roundNumber);
                play_tone(600, 0.02);
            }
            wait(0.1);
            continue;
        }


        // =========================
        // SPELRUNDA
        // =========================
        if (playing) {

            // Hämta accelerometerdata (krav)
            x = acc.x();
            y = acc.y();
            z = acc.z();

            // Basfart + svårighet från ADC + ökad fart för varje runda
            float speed = (0.5f + (roundNumber * 0.4f)) * difficultyScale;

            ballX += (int)(-x * 10 * speed);
            ballY += (int)( y * 10 * speed);

            // Begränsa rörelse inom skärmen
            if (ballX < 0) ballX = 0;
            if (ballY < 0) ballY = 0;
            if (ballX > 120) ballX = 120;
            if (ballY > 28) ballY = 28;

            lcd.cls();

            // Rita mål och boll
            lcd.circle(goalX, goalY, 3, 1);
            lcd.fillcircle(ballX, ballY, 2, 1);

            // GUI – visa runda, tid och score
            lcd.locate(0,0);
            lcd.printf("Runda %d/5", roundNumber);

            lcd.locate(0,10);
            lcd.printf("Tid: %.1f", 10.0f - timer.read());

            lcd.locate(0,20);
            lcd.printf("Score: %d", score);


            // =========================
            // KOLLISION MED MÅL
            // =========================
            if (abs(ballX - goalX) < 5 && abs(ballY - goalY) < 5) {

                timer.stop();
                playing = false;

                // Score baserat på tid kvar
                score += (int)(10.0f - timer.read()) * 10;

                win_round_screen(roundNumber);
                roundNumber++;

                if (roundNumber > 5) {
                    final_win_screen();
                    roundNumber = 1;
                    start_menu();
                } else {
                    start_round(roundNumber);
                }
            }

            // =========================
            // TIDEN ÄR SLUT
            // =========================
            if (timer.read() > 10.0f) {

                timer.stop();
                playing = false;

                lose_screen();

                roundNumber = 1;
                start_menu();
            }

            wait(0.1);
        }
    }
}
