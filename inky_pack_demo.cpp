#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <hardware/adc.h>
#include <stdio.h>

#include "pico/rand.h"
#include "uc8151.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"

#define Batt_x_loc 250
#define Batt_y_loc 110




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


Button button_a(Pin::A);
Button button_b(Pin::B);
Button button_c(Pin::C);
Button button_d(Pin::D);
Button button_e(Pin::E);

uint16_t Batt_Read(float conversion_factor, float max_voltage, float min_voltage){
        //This function requires ADC 3, GPIO 29 to be configured
        uint16_t raw_value = adc_read();
        uint16_t batt_percent = (((raw_value * conversion_factor) - min_voltage) / (max_voltage - min_voltage)) * 100;

        if (batt_percent > 100){
            batt_percent = 100;
        }

        return batt_percent;
    }

int main() {

    //Set up internal ADC pin to monitor system voltage
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);

    gpio_init(24);
    gpio_set_dir(24, GPIO_IN);
    gpio_pull_down(24);
    bool ChargeState = gpio_get(24);
    
    const float conversion_factor = (3 * 3.3) / 4095;
    const float max_voltage = 4.2; //max voltage/full battery
    const float min_voltage = 3.2; //min voltage/dead battery


    //Set update speed for display (0 = slowest, 3 = fastest)
    display.set_update_speed(2);

    graphics.set_pen(15);
    graphics.clear();
    graphics.set_pen(0);
    graphics.set_font("bitmap8");

    graphics.text("Press A for motivation", {0, 40}, 296, 3);

    

    //Read system voltage for initial battery percentage display on startup
    
    
    //calculate batt percent
    uint16_t batt_percent = Batt_Read(conversion_factor, max_voltage, min_voltage);
    if(ChargeState){
        graphics.text("Charging", {(Batt_x_loc), Batt_y_loc + 5}, 296, 1);
    }else{
        graphics.text("Batt " + std::to_string(batt_percent) + "%", {Batt_x_loc, Batt_y_loc}, 296, 1);
    }
    
    

    

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

            //read charging pin
            ChargeState = gpio_get(24);

            //get random number to choose a phrase, track used words in phrase_memory
            rand_num = (get_rand_32() % std::size(phrases));
            for (int i = 0; i < std::size(phrase_memory); i++) {
                if (phrase_memory[i] == rand_num) {
                    rand_num = (get_rand_32() % std::size(phrases));
                    i = 0; //reset loop to check new random number against memory
                }
            }

            phrase_memory[mem_index] = rand_num; //store new random number in memory
            if (mem_index < std::size(phrase_memory) - 1) {
                mem_index++;
            } else {
                mem_index = 0; //reset memory index if we reach the end of the array. Also update batt percentage.
                batt_percent = Batt_Read(conversion_factor, max_voltage, min_voltage);
            }
            //Print out phrase memory for debugging
            // std::string phrase_memory_str = "";
            // for (int i = 0; i < std::size(phrase_memory); i++) {
            //     phrase_memory_str += std::to_string(phrase_memory[i]) + " ";
            // }
            //graphics.text(phrase_memory_str, {0, 100}, 296, 1);

            // print out random phrase and latest battery percent
            graphics.text(phrases[rand_num], {0, 0}, 296, 3);

            if(ChargeState){
                graphics.text("Charging", {(Batt_x_loc), Batt_y_loc + 5}, 296, 1);
            }else{
                graphics.text(std::to_string(batt_percent) + "%", {Batt_x_loc, Batt_y_loc}, 296, 2);
            }
           
            display.update(&graphics);
        }
        sleep_ms(10); //Check buttons every 10ms
    
    }

    return 0;
}
