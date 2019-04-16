//
// Created by tkhamvilai on 4/14/19.
//

#ifndef DIST_MILP_ENGINE_HPP
#define DIST_MILP_ENGINE_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <cstring>

#include <wiringPi.h>
#include <phidget22.h>
#include "PhidgetHelperFunctions.hpp" // Manually Modified

#define MIN_PWM 100
#define MAX_PWM 200
#define OPER_PWM 120

#define TOLERANCE 5

class ENGINE
{
public:
    float sensor_data; // read sensor data from phidget and for broadcasting

    int PWM_out; // PWM value sending our from controller nodes to the engine
    int *PWM_in; // PWM value the engine node receiving from controller nodes

    int *PWM_for_Voter; // for voter to choose and detecting faults
    int PWM_to_Engine; // actual PWM number to send to the engine

    int PWM_PIN; // 1 for wPI
    int EngineSetup;
    int SensorSetup;

    int fault_from_voter;

    PhidgetVoltageRatioInputHandle ch;
    ChannelInfo channelInfo;

    ENGINE(int N_nodes, int N_apps);

    void read_sensor();
    void pwm_send();

    void voter(int N_CRs, int N_apps);
    int error_detector(int* array);
    double voter_mean(int* array, int err_detector_result, int N_apps_to_vote);
};

static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double voltageRatio);

#endif //DIST_MILP_ENGINE_HPP
