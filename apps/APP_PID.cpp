//
// Created by tkhamvilai on 4/16/19.
//

#include "APP_PID.hpp"

void APP_PID(NOC* NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE* Engine, int color)
{

#if defined ( USE_ENGINE_W_FEEDBACK ) || defined ( USE_ENGINE_WO_FEEDBACK )
    float setpoint = 90.0;
    float Kp = 0.5;
#elif defined ( USE_X_PLANE_SIMULATOR )
    float setpoint = 15.0;
    float K_roll = 40.2, K_roll_dot = 22.5, Kp = 0.5*0.011111;
#endif

#ifndef __x86_64__
    int switch_button_2 = COMPUTATIONAL_FAULT_PIN;
    pinMode (switch_button_2, INPUT);
    pullUpDnControl (switch_button_2, PUD_UP);

    if(digitalRead (switch_button_2) == 0)
    {
#if defined ( USE_ENGINE_W_FEEDBACK ) || defined ( USE_ENGINE_WO_FEEDBACK )
        Kp *= (NoC->node_to_run + 1);
#elif defined ( USE_X_PLANE_SIMULATOR )
        Kp = (NoC->node_to_run + 1);
#endif
        color = LED_CYAN;
    }
#endif

    APP_LED(NoC, NoC_Fault, NoC_GLPK, prob_GLPK, Engine, color);

    int sum_node_to_run = 1;
    for (int i = 0; i < NoC->N_apps; i++)
    {
        if(NoC->node_to_run == sum_node_to_run)
        {
#if defined ( USE_ENGINE_W_FEEDBACK )
            float error = setpoint - Engine->sensor_data;
            Engine->PWM_out = (int)(OPER_PWM + Kp*error);
#elif defined ( USE_ENGINE_WO_FEEDBACK )
            float error = setpoint*PERCENT_ERROR;
            Engine->PWM_out = (int)(OPER_PWM + Kp*error);
#elif defined ( USE_X_PLANE_SIMULATOR )
            Engine->delta_a_out = Kp*(K_roll*(setpoint - Engine->roll_deg) - K_roll_dot*Engine->roll_dot);
#endif
            break;
        }
        else
        {
            sum_node_to_run += NoC->N_nodes_apps[i];
        }
    }
}

void APP_PWM_OFF()
{

#ifndef __x86_64__
#if defined ( USE_ENGINE_W_FEEDBACK )
    int PWM_PIN = 1;
    pinMode(PWM_PIN, PWM_OUTPUT);
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange (2000);
    pwmSetClock (192);
    pwmWrite(PWM_PIN, 100);
#endif
#endif

}