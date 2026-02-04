#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "pico/rand.h"
#include "uc8151.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"


using namespace pimoroni;

enum Pin {
    A           = 12,
    B           = 13,
    C           = 14,
    D           = 15,
    E           = 11,
    UP          = 15, // alias for D
    DOWN        = 11, // alias for E
    USER        = 23,
    CS          = 17,
    CLK         = 18,
    MOSI        = 19,
    DC          = 20,
    RESET       = 21,
    BUSY        = 26,
    VBUS_DETECT = 24,
    LED         = 25,
    BATTERY     = 29,
    ENABLE_3V3  = 10
};


UC8151 display(296, 128, ROTATE_0);
PicoGraphics_Pen1BitY graphics(display.width, display.height, nullptr);

//initialize array of phrases and another array to track last selected words
const char* phrases[] = {"You are loved", "You are beautiful", "You are stunning", "You are amazing", "You are breathtaking", "You are worthwhile", 
                     "You're doing great! =)", "Keep it up, champ", "You rock", "You are stronger than you know", "Your best days are yet to come", "Hang in there!",
                       "Believe in yourself", "Treat yourself", "You are more than meets the eye", "It's gonna be OK", "You matter", "You are not alone", "Try, try again",
                       "Giving up is for wimps", "You smell nice =)", "Don't be afraid to get your feet a little wet", "MEEEEE", "I am not optimistic. I am not hopefull. I am sure.",
                       "Another vacation is always right around the corner!", "Sit with me and watch me choose you", "Hey now!", "I am the hell and the highwater!"};
int phrase_memory[20] = {};


Button button_a(Pin::A, Polarity::ACTIVE_LOW);
Button button_b(Pin::B);
Button button_c(Pin::C);
Button button_d(Pin::D);
Button button_e(Pin::E);



int main() {

    //Set update speed for display (0 = slowest, 3 = fastest)
    display.set_update_speed(2);

    graphics.set_pen(15);
    graphics.clear();

    graphics.set_pen(0);
    graphics.set_font("bitmap8");
    graphics.text("Press any button for motivation", {0, 40}, 296, 3);

    display.update(&graphics);

    //set up an index to track phrase memory
    int mem_index = 0;
    int rand_num = 0;

    while (true) {
        
        if (button_a.read()) {
            //Make sure screen is clear and pen color is set to black
            graphics.set_pen(15);
            graphics.clear();
            graphics.set_pen(0);

            //get random number to choose a phrase, track used words in phrase_memory
            rand_num = (get_rand_32() % std::size(phrases));
            
            graphics.text(phrases[rand_num], {0, 0}, 296, 3);
            display.update(&graphics);
        }
        sleep_ms(10); //Check buttons every 10ms
        
    

    

    return 0;
}
}
