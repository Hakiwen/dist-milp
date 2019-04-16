//
// Created by tkhamvilai on 4/16/19.
//

#include "APP_PID.hpp"

void APP_PID(NOC* NoC, ENGINE* Engine)
{
    APP_LED(NoC, Engine);

    float setpoint = 8.0, Kp = 1.0;
    float error = setpoint - Engine->sensor_data;
    Engine->PWM_out = (int)(OPER_PWM + Kp*error);
}