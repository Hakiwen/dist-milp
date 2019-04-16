//
// Created by tkhamvilai on 4/16/19.
//

#include "APP_PID.hpp"

void APP_PID(NOC* NoC, ENGINE* Engine)
{
    APP_LED(NoC, Engine);

    float setpoint = 100.0; //, error_I = 0.0;
    float Kp = 2.0, Ki = 0.5, Kd = 0.0;
    float error = setpoint - Engine->sensor_data;
//    error_I += Ki*error;
//    if (error_I > MAX_I)
//    {
//        error_I = MAX_I;
//    }
//    else if (error_I < -MAX_I)
//    {
//        error_I = -MAX_I;
//    }
    Engine->PWM_out = (int)(OPER_PWM + Kp*error);

    //    std::cout << error << std::endl;
}