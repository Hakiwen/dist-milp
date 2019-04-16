//
// Created by tkhamvilai on 4/14/19.
//

#include "ENGINE.hpp"

ENGINE::ENGINE(int N_CRs, int N_apps_to_vote)
{
    this->sensor_data = 0;

    this->PWM_out = 0;
    this->PWM_in = new int[N_CRs];

    this->PWM_for_Voter = new int[N_apps_to_vote];
    this->PWM_to_Engine = 0;

    for (int i = 0; i < N_CRs; i++)
    {
        this->PWM_in[i] = 0;
    }

    for (int i = 0; i < N_apps_to_vote; i++)
    {
        this->PWM_for_Voter[i] = 0;
    }

    this->EngineSetup = 0;
    this->SensorSetup = 0;

    this->ch = NULL;

    this->fault_from_voter = 0;
}

void ENGINE::read_sensor()
{
    if(!this->SensorSetup)
    {
        this->SensorSetup = 1;

        PhidgetVoltageRatioInput_create(&this->ch);
        AskForDeviceParameters(&this->channelInfo, (PhidgetHandle *)&this->ch); // TODO manually set phidget parameters
        Phidget_setDeviceSerialNumber((PhidgetHandle)this->ch, this->channelInfo.deviceSerialNumber);
        Phidget_setHubPort((PhidgetHandle)this->ch, this->channelInfo.hubPort);
        Phidget_setIsHubPortDevice((PhidgetHandle)ch, this->channelInfo.isHubPortDevice);
        Phidget_setChannel((PhidgetHandle)this->ch, this->channelInfo.channel);

        Phidget_setOnAttachHandler((PhidgetHandle)this->ch, onAttachHandler, NULL);
        Phidget_setOnDetachHandler((PhidgetHandle)this->ch, onDetachHandler, NULL);

        PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(ch, onVoltageRatioChangeHandler, this);

        Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000);
    }

//    std::cout << "sensor: " << this->sensor_data << std::endl;
}

void ENGINE::pwm_send()
{
    if(!this->EngineSetup)
    {
        this->EngineSetup = 1;
        this->PWM_PIN = 1;
        pinMode(this->PWM_PIN, PWM_OUTPUT);
        pwmSetMode (PWM_MODE_MS);
        pwmSetRange (2000);
        pwmSetClock (192);
//        pwmWrite(this->PWM_PIN, 200); // for calibration only
//        delay(10000);
        pwmWrite(this->PWM_PIN, 100);
        delay(10000);
    }

    if(this->PWM_to_Engine < MIN_PWM)
    {
        this->PWM_to_Engine = MIN_PWM;
    }
    else if(this->PWM_to_Engine > MAX_PWM)
    {
        this->PWM_to_Engine = MAX_PWM;
    }

    std::cout << "sensor: " << this->sensor_data << " PWM: " << this->PWM_to_Engine << std::endl;
    pwmWrite(this->PWM_PIN, this->PWM_to_Engine);
}

void ENGINE::voter(int N_CRs, int N_apps_to_vote)
{
    for (int i = 0; i < N_apps_to_vote; i++) {
        this->PWM_for_Voter[i] = MIN_PWM;
    }

    int j = 0;
    for (int i = 0; i < N_CRs; i++)
    {
        if (this->PWM_in[i] > MIN_PWM)
        {
            this->PWM_for_Voter[j] = this->PWM_in[i];
            j++;

            if(j == N_apps_to_vote)
            {
                break;
            }
        }
    }

    this->fault_from_voter = this->error_detector(this->PWM_for_Voter);
    this->PWM_to_Engine = (int)this->voter_mean(this->PWM_for_Voter, this->fault_from_voter, N_apps_to_vote);
}

int ENGINE::error_detector(int* array){
    int v1 = array[0];
    int v2 = array[1];
    int v3 = array[2];
    /* Returns the number corresponding to the wrong value among three ones, 0 if they all match and 6 if they all mismatch
     * get the values from the three redundant applications
     */

    int mismatch12 = ( abs(v1 - v2) > TOLERANCE );
    int mismatch13 = ( abs(v1 - v3) > TOLERANCE );
    int mismatch23 = ( abs(v2 - v3) > TOLERANCE );

    return 1*mismatch12*mismatch13 + 2*mismatch12*mismatch23 + 3*mismatch13*mismatch23;
}

double ENGINE::voter_mean(int* array, int err_detector_result, int N_apps_to_vote){
    if (err_detector_result == 6)
    {
        return 0;
    }
    else
    {
        double sum = 0;
        for(int i = 0 ; i < N_apps_to_vote ; i++)
        {
            sum = sum + ((double) array[i])*(err_detector_result != i+1) / ( 3*(err_detector_result == 0) + 2*(err_detector_result != 0) );
        }
        return sum;
    }
}

/** Phidget Functions **/

static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx)
{
    Phidget_ChannelSubclass channelSubclass;
    Phidget_DeviceClass deviceClass;
    const char* channelClassName;
    int32_t serialNumber;
    int32_t hubPort;
    int32_t channel;

    Phidget_getDeviceSerialNumber(ph, &serialNumber);
    Phidget_getChannel(ph, &channel);
    Phidget_getChannelClassName(ph, &channelClassName);
    Phidget_getDeviceClass(ph, &deviceClass);

    if (deviceClass == PHIDCLASS_VINT)
    {
        Phidget_getHubPort(ph, &hubPort);
    }

    PhidgetVoltageRatioInput_setDataInterval((PhidgetVoltageRatioInputHandle)ph, 1000);
    PhidgetVoltageRatioInput_setVoltageRatioChangeTrigger((PhidgetVoltageRatioInputHandle)ph, 0.0);
    Phidget_getChannelSubclass(ph, &channelSubclass);

    if (channelSubclass == PHIDCHSUBCLASS_VOLTAGEINPUT_SENSOR_PORT)
    {
        PhidgetVoltageRatioInput_setSensorType((PhidgetVoltageRatioInputHandle)ph, SENSOR_TYPE_VOLTAGERATIO);
    }
}

static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx)
{
    Phidget_DeviceClass deviceClass;
    const char* channelClassName;
    int32_t serialNumber;
    int32_t hubPort;
    int32_t channel;

    Phidget_getDeviceSerialNumber(ph, &serialNumber);
    Phidget_getChannel(ph, &channel);
    Phidget_getChannelClassName(ph, &channelClassName);
    Phidget_getDeviceClass(ph, &deviceClass);

    if (deviceClass == PHIDCLASS_VINT)
    {
        Phidget_getHubPort(ph, &hubPort);
    }
}

static void CCONV onVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double voltageRatio)
{
//    printf("[VoltageRatio Event] -> VoltageRatio: %.4g\n", voltageRatio);
    ENGINE *Engine = (ENGINE *)ctx;
    Engine->sensor_data = voltageRatio*1000000.0; // TODO Calibrate Sensor
}
