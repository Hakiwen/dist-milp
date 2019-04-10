//
// Created by tkhamvilai on 4/9/19.
//

#include "APP_LED.hpp"

void APP_LED(int color)
{
    if(color == LED_OFF)
    {
        std::cout << "OFF" << std::endl;
    }
    else if(color == LED_WHITE)
    {
        std::cout << "WHITE" << std::endl;
    }
    else if(color == LED_RED)
    {
        std::cout << "RED" << std::endl;
    }
    else if(color == LED_GREEN)
    {
        std::cout << "GREEN" << std::endl;
    }
    else if(color == LED_BLUE)
    {
        std::cout << "BLUE" << std::endl;
    }

    //    wiringPiSetup () ;
//    pinMode (0, OUTPUT) ;
//    for (;;)
//    {
//        digitalWrite (0, HIGH) ; delay (500) ;
//        digitalWrite (0,  LOW) ; delay (500) ;
//    }
}