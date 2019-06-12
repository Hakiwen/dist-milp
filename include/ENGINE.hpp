//
// Created by tkhamvilai on 4/14/19.
//

#ifndef DIST_MILP_ENGINE_HPP
#define DIST_MILP_ENGINE_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

#include <wiringPi.h>

#ifndef __x86_64__
#include <phidget22.h>
#include "PhidgetHelperFunctions.hpp" // Manually Modified
#endif

#define MIN_PWM 100
#define MAX_PWM 200
#define OPER_PWM 120

#define TOLERANCE 1
#define N_APP_TO_VOTE 3

#define MAX_VOTER_DELAY 0 // 1000
#define MAX_WRITE_DELAY 10

class ENGINE
{
public:
    float sensor_data; // read sensor data from phidget and for broadcasting

    int PWM_out; // PWM value sending our from controller nodes to the engine
    int *PWM_in; // PWM value the engine node receiving from controller nodes

    int *PWM_for_Voter; // for voter to choose and detecting faults
    int *PWM_for_Voter_ind;
    int PWM_to_Engine; // actual PWM number to send to the engine

    int PWM_PIN; // 1 for wPI
    int EngineSetup;
    int SensorSetup;

    int fault_detect;
    int fault_from_voter;

    int voter_delay;
    int write_delay;

#ifndef __x86_64__
    PhidgetVoltageRatioInputHandle ch;
    ChannelInfo channelInfo;
#endif

    ENGINE(int N_nodes);

    void read_sensor();
    void pwm_send();

    void voter(int N_CRs);
    int error_detector(int* array);
    double voter_mean(int* array, int err_detector_result);

    void write_data();

    void run(int N_CRs);
};

#ifndef __x86_64__
static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double voltageRatio);
#endif

#endif //DIST_MILP_ENGINE_HPP
