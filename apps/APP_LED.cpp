//
// Created by tkhamvilai on 4/9/19.
//

#include "APP_LED.hpp"

void APP_LED(NOC *NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE *Engine, int color)
{
#if defined (__x86_64__)
#if ( PRINT )
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
    else if(color == LED_CYAN)
    {
        std::cout << "CYAN" << std::endl;
    }
    else if(color == LED_MAGENTA)
    {
        std::cout << "MAGENTA" << std::endl;
    }
    else if(color == LED_YELLOW)
    {
        std::cout << "YELLOW" << std::endl;
    }
#endif
#elif !defined (__x86_64__)
    int RED_LED   = RED_LED_PIN;
    int GREEN_LED = GREEN_LED_PIN;
    int BLUE_LED  = BLUE_LED_PIN;
    pinMode (RED_LED, OUTPUT);
    pinMode (GREEN_LED, OUTPUT);
    pinMode (BLUE_LED, OUTPUT);
    if(color == LED_OFF)
    {
        digitalWrite (RED_LED,  LOW);
        digitalWrite (GREEN_LED,  LOW);
        digitalWrite (BLUE_LED,  LOW);
    }
    else if(color == LED_WHITE)
    {
        digitalWrite (RED_LED,  HIGH);
        digitalWrite (GREEN_LED,  HIGH);
        digitalWrite (BLUE_LED,  HIGH);
    }
    else if(color == LED_RED)
    {
        digitalWrite (RED_LED,  HIGH);
        digitalWrite (GREEN_LED,  LOW);
        digitalWrite (BLUE_LED,  LOW);
    }
    else if(color == LED_GREEN)
    {
        digitalWrite (RED_LED,  LOW);
        digitalWrite (GREEN_LED,  HIGH);
        digitalWrite (BLUE_LED,  LOW);
    }
    else if(color == LED_BLUE)
    {
        digitalWrite (RED_LED,  LOW);
        digitalWrite (GREEN_LED,  LOW);
        digitalWrite (BLUE_LED,  HIGH);
    }
    else if(color == LED_CYAN)
    {
        digitalWrite (RED_LED,  LOW);
        digitalWrite (GREEN_LED,  HIGH);
        digitalWrite (BLUE_LED,  HIGH);
    }
    else if(color == LED_MAGENTA)
    {
        digitalWrite (RED_LED,  HIGH);
        digitalWrite (GREEN_LED,  LOW);
        digitalWrite (BLUE_LED,  HIGH);
    }
    else if(color == LED_YELLOW)
    {
        digitalWrite (RED_LED,  HIGH);
        digitalWrite (GREEN_LED,  HIGH);
        digitalWrite (BLUE_LED,  LOW);
    }
#endif
}

void APP_LED_OFF()
{
#if defined(__x86_64__)
#if ( PRINT )
    std::cout << "OFF" << std::endl;
#endif
#elif !defined(__x86_64__)
    int RED_LED = RED_LED_PIN;
    int GREEN_LED = GREEN_LED_PIN;
    int BLUE_LED  = BLUE_LED_PIN;
    pinMode (RED_LED, OUTPUT);
    pinMode (GREEN_LED, OUTPUT);
    pinMode (BLUE_LED, OUTPUT);
    digitalWrite (RED_LED,  LOW);
    digitalWrite (GREEN_LED,  LOW);
    digitalWrite (BLUE_LED,  LOW);
#endif
}