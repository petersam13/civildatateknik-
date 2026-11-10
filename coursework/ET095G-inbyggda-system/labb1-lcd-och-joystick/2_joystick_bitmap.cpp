#include "mbed.h"
#include "C12832.h"
DigitalOut myled(LED1);
DigitalIn Up(p15);
DigitalIn Down(p12);
DigitalIn Left(p13);
DigitalIn Right(p16);
DigitalIn Reset(p14);

    static char rek[]={//Draw a shape 'B'
        0x60,
        0x50,
        0x50,
        0x60,
        0x50,
        0x50,
        0x60,
        
        };
        
        Bitmap Bitrek = {//Decide size and color
        8,
        8,
        1,
        rek
        };

    C12832 lcd(p5,p7,p6,p8,p11);
            
int main() {

    float x = 0.0;
    float y = 0.0;
    lcd.locate(x, y);  
    lcd.print_bm(Bitrek, x, y);
    lcd.copy_to_lcd();
    while(1){
        lcd.locate(x, y);
        if(Up == 1){ //if the input is up and the y postion is greater or equal to 0 move up
            if(y>=0){
                y = y-0.1;
                lcd.locate(x, y);
                lcd.cls();
                lcd.print_bm(Bitrek, x, y);
                lcd.copy_to_lcd();
                wait(0.001);
            }
        }
        if(Down == 1){//if the input is downn and the y postion is less than 24 move down
            if (y <24){
                y = y+0.1;
                lcd.locate(x, y);
                lcd.cls();
                lcd.print_bm(Bitrek, x, y);
                lcd.copy_to_lcd();
                wait(0.001);
            }
        }
        if (Left == 1){//if input is left and the x position is greater or equal to 0 move to the left by dicreasing x by 0.1
            if(x>=0){
                x=x-0.1;
                lcd.locate(x , y);
                lcd.cls();
                lcd.print_bm(Bitrek, x, y);
                lcd.copy_to_lcd();
                wait(0.001);
            }
        }  
        if (Right == 1){//if input is right and the x position is less than 124 move to the right by increasing x by 0.1
            if(x<124){
                x = x+0.1;
                lcd.locate(x, y);
                lcd.cls();  
                lcd.print_bm(Bitrek, x, y);
                lcd.copy_to_lcd();
                wait(0.001);
            }
        }
        
    
    
    }
        
}