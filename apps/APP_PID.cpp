//
// Created by tkhamvilai on 4/16/19.
//

#include "APP_PID.hpp"

void APP_PID(NOC* NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE* Engine, int color)
{
    float setpoint = 90.0;
    float Kp = 0.5;

#ifndef __x86_64__
    int switch_button_2 = 25;
    pinMode (switch_button_2, INPUT);
    pullUpDnControl (switch_button_2, PUD_UP);

    if(digitalRead (switch_button_2) == 0)
    {
        Kp *= (NoC->node_to_run + 1);
        color = LED_CYAN;
    }
#endif

    APP_LED(NoC, NoC_Fault, NoC_GLPK, prob_GLPK, Engine, color);

    int sum_node_to_run = 1;
    for (int i = 0; i < NoC->N_apps; i++)
    {
        if(NoC->node_to_run == sum_node_to_run)
        {
            float error = setpoint - Engine->sensor_data;
            Engine->PWM_out = (int)(OPER_PWM + Kp*error);
            break;
        }
        else
        {
            sum_node_to_run += NoC->N_nodes_apps[i];
        }
    }

//    float error = setpoint - Engine->sensor_data;
//    Engine->PWM_out = (int)(OPER_PWM + Kp*error);
//    std::cout << error << std::endl;
}

void APP_PWM_OFF()
{

#ifndef __x86_64__
    int PWM_PIN = 1;
    pinMode(PWM_PIN, PWM_OUTPUT);
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange (2000);
    pwmSetClock (192);
    pwmWrite(PWM_PIN, 100);
#endif

}