//
// Created by tkhamvilai on 4/14/19.
//

#ifndef DIST_MILP_ENGINE_HPP
#define DIST_MILP_ENGINE_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

#if defined ( USE_ENGINE_W_FEEDBACK ) || defined ( USE_ENGINE_WO_FEEDBACK )
#define PIN_PWM 1
#define MIN_PWM 100
#define MAX_PWM 200
#define OPER_PWM 120
#if defined ( USE_ENGINE_WO_FEEDBACK )
#define PERCENT_ERROR 0.1
#endif
#elif defined ( USE_X_PLANE_SIMULATOR )
#include "udp_driver.h"
#define PC_IP_ADDRESS "192.168.0.19"
#define X_PLANE_IP_ADDRESS "192.168.0.99"
#define X_PLANE_PORT 49000
#define X_PLANE_MAX_BYTE 1024
#define X_PLANE_BUFFER_SIZE 40
#define X_PLANE_RECEIVE_PACKET_BYTE 113 // dependong on what data you want to received
#define X_PLANE_SEND_PACKET_BYTE 41 // dependong on what data you want to received
#define INVALID_DELTA_A_VAL 999
#endif

#ifndef __x86_64__
#include <wiringPi.h>
#include <phidget22.h>
#include "PhidgetHelperFunctions.hpp" // Manually Modified
#endif

#define TOLERANCE 1
#define N_APP_TO_VOTE 3

#define MAX_VOTER_DELAY 0 // 1000
#define MAX_WRITE_DELAY 10

class ENGINE
{
public:

#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )

    float sensor_data; // read sensor data from phidget and for broadcasting

    int PWM_out; // PWM value sending our from controller nodes to the engine
    int *PWM_in; // PWM value the engine node receiving from controller nodes

    int *PWM_for_Voter; // for voter to choose and detecting faults
    int *PWM_for_Voter_ind;
    int PWM_to_Engine; // actual PWM number to send to the engine

    int PWM_PIN; // 1 for wPI

#elif defined ( USE_X_PLANE_SIMULATOR )

    UDPSocket udp;
    uint8_t buf[X_PLANE_BUFFER_SIZE];
    uint8_t data[X_PLANE_MAX_BYTE];
    float roll_deg, roll_dot, delta_a_out;
    float *delta_a_in;

    float *delta_a_for_Voter; // for voter to choose and detecting faults
    float *delta_a_for_Voter_ind;
    float delta_a_to_X_plane; // actual PWM number to send to the engine

#endif

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

#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )
    void voter(int N_CRs);
    int error_detector(int* array);
    double voter_mean(int* array, int err_detector_result);
#elif defined ( USE_X_PLANE_SIMULATOR )
    void voter(float N_CRs);
    int error_detector(float* array);
    double voter_mean(float* array, float err_detector_result);
#endif

    void write_data();

    void run(int N_CRs);
};

#ifndef __x86_64__
#ifdef USE_ENGINE_W_FEEDBACK
static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx);
static void CCONV onVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double voltageRatio);
#endif
#endif

#ifdef USE_X_PLANE_SIMULATOR
float Decode_Roll_X_plane (uint8_t *data);
float Decode_Roll_Dot_X_plane (uint8_t *data);
void Encode_Delta_to_X_plane (float del_a, uint8_t* buffer);
#endif

#endif //DIST_MILP_ENGINE_HPP
