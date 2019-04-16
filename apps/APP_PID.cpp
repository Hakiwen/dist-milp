//
// Created by tkhamvilai on 4/16/19.
//

#include "APP_PID.hpp"

void APP_PID(NOC* NoC, ENGINE* Engine)
{
    APP_LED(NoC, Engine);

    float setpoint = 80.0;
    float Kp = 0.5;
    float error = setpoint - Engine->sensor_data;
    Engine->PWM_out = (int)(OPER_PWM + Kp*error);

    //    std::cout << error << std::endl;
}

void APP_PWM_OFF()
{
    int PWM_PIN = 1;
    pinMode(PWM_PIN, PWM_OUTPUT);
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange (2000);
    pwmSetClock (192);
    pwmWrite(PWM_PIN, 100);
}