//
// Created by tkhamvilai on 4/14/19.
//

#include "ENGINE.hpp"

ENGINE::ENGINE(int N_CRs, int N_apps)
{
    this->sensor_data = 0;

    this->PWM_out = 0;
    this->PWM_in = new int[N_CRs];

    this->PWM_for_Voter = new int[N_apps];
    this->PWM_to_Engine = 0;

    for (int i = 0; i < N_CRs; i++)
    {
        this->PWM_in[i] = 0;
    }

    for (int i = 0; i < N_apps; i++)
    {
        this->PWM_for_Voter[i] = 0;
    }
}

void ENGINE::read_sensor()
{

}

void ENGINE::voter()
{

}

void ENGINE::pwm_send()
{

}