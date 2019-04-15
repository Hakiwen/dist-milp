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
#include "PhidgetHelperFunctions.h"

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

    PhidgetVoltageRatioInputHandle ch;
    PhidgetReturnCode prc;
    ChannelInfo channelInfo;

    ENGINE(int N_nodes, int N_apps);

    void read_sensor();
    void voter();
    void pwm_send();
};

static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onErrorHandler(PhidgetHandle ph, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString);
static void CCONV onVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double voltageRatio);
static void CCONV onSensorChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double sensorValue, Phidget_UnitInfo *unitInfo);

#endif //DIST_MILP_ENGINE_HPP
