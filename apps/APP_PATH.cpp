//
// Created by tkhamvilai on 7/22/19.
//

#include "APP_PATH.hpp"

void PATH_LED(int* path_to_run, Eigen::MatrixXi G, int rank)
{
    int LED_COLOR[LED_PATHS_PER_NODE] = {0, 0};
    int i = 0;
    for (int j = 0; j < G.cols(); j++)
    {
        if (G(rank - 1, j) == -1)
        {
            LED_COLOR[i] = path_to_run[j];
            i++;
            if (i == LED_PATHS_PER_NODE)
            {
                break;
            }
        }
    }

#if defined (__x86_64__)
#if ( PRINT )
    // Too Lazy to print
#endif
#elif !defined (__x86_64__)
    int RED_LED_PATH[LED_PATHS_PER_NODE] = {RED_LED_PATH_PIN_1, RED_LED_PATH_PIN_2};
    int GREEN_LED_PATH[LED_PATHS_PER_NODE] = {GREEN_LED_PATH_PIN_1, GREEN_LED_PATH_PIN_2};
    int BLUE_LED_PATH[LED_PATHS_PER_NODE] = {BLUE_LED_PATH_PIN_1, BLUE_LED_PATH_PIN_2};

    for (int i = 0; i < LED_PATHS_PER_NODE; i++)
    {
        pinMode (RED_LED_PATH[i], OUTPUT);
        pinMode (GREEN_LED_PATH[i], OUTPUT);
        pinMode (BLUE_LED_PATH[i], OUTPUT);

        if(LED_COLOR[i] == LED_PATH_OFF || LED_COLOR[i] == LED_PATH_WHITE)
        {
            digitalWrite (RED_LED_PATH[i], LOW);
            digitalWrite (GREEN_LED_PATH[i], LOW);
            digitalWrite (BLUE_LED_PATH[i], LOW);
        }
        else if(LED_COLOR[i] == LED_PATH_RED)
        {
            digitalWrite (RED_LED_PATH[i], HIGH);
            digitalWrite (GREEN_LED_PATH[i], LOW);
            digitalWrite (BLUE_LED_PATH[i], LOW);
        }
        else if(LED_COLOR[i] == LED_PATH_GREEN)
        {
            digitalWrite (RED_LED_PATH[i], LOW);
            digitalWrite (GREEN_LED_PATH[i], HIGH);
            digitalWrite (BLUE_LED_PATH[i], LOW);
        }
        else if(LED_COLOR[i] == LED_PATH_RED_GREEN)
        {
            digitalWrite (RED_LED_PATH[i], HIGH);
            digitalWrite (GREEN_LED_PATH[i], HIGH);
            digitalWrite (BLUE_LED_PATH[i], LOW);
        }
        else if(LED_COLOR[i] == LED_PATH_BLUE)
        {
            digitalWrite (RED_LED_PATH[i], LOW);
            digitalWrite (GREEN_LED_PATH[i], LOW);
            digitalWrite (BLUE_LED_PATH[i], HIGH);
        }
        else if(LED_COLOR[i] == LED_PATH_RED_BLUE)
        {
            digitalWrite (RED_LED_PATH[i], HIGH);
            digitalWrite (GREEN_LED_PATH[i], LOW);
            digitalWrite (BLUE_LED_PATH[i], HIGH);
        }
        else if(LED_COLOR[i] == LED_PATH_GREEN_BLUE)
        {
            digitalWrite (RED_LED_PATH[i], LOW);
            digitalWrite (GREEN_LED_PATH[i], HIGH);
            digitalWrite (BLUE_LED_PATH[i], HIGH);
        }
        else if(LED_COLOR[i] == LED_PATH_RED_GREEN_BLUE)
        {
            digitalWrite (RED_LED_PATH[i], HIGH);
            digitalWrite (GREEN_LED_PATH[i], HIGH);
            digitalWrite (BLUE_LED_PATH[i], HIGH);
        }
    }
#endif
}

void PATH_LED_OFF()
{
#if defined(__x86_64__)
#if ( PRINT )
    std::cout << "PATH OFF" << std::endl;
#endif
#elif !defined(__x86_64__)
    int RED_LED_1 = RED_LED_PATH_PIN_1;
    int GREEN_LED_1 = GREEN_LED_PATH_PIN_1;
    int BLUE_LED_1 = BLUE_LED_PATH_PIN_1;
    pinMode (RED_LED_1, OUTPUT);
    pinMode (GREEN_LED_1, OUTPUT);
    pinMode (BLUE_LED_1, OUTPUT);
    digitalWrite (RED_LED_1, LOW);
    digitalWrite (GREEN_LED_1, LOW);
    digitalWrite (BLUE_LED_1, LOW);

    int RED_LED_2 = RED_LED_PATH_PIN_2;
    int GREEN_LED_2 = GREEN_LED_PATH_PIN_2;
    int BLUE_LED_2 = BLUE_LED_PATH_PIN_2;
    pinMode (RED_LED_2, OUTPUT);
    pinMode (GREEN_LED_2, OUTPUT);
    pinMode (BLUE_LED_2, OUTPUT);
    digitalWrite (RED_LED_2, LOW);
    digitalWrite (GREEN_LED_2, LOW);
    digitalWrite (BLUE_LED_2, LOW);
#endif
}