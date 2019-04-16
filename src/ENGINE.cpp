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

void ENGINE::voter(int N_CRs, int N_apps_to_vote)
{
    for (int i = 0; i < N_apps_to_vote; i++) {
        this->PWM_for_Voter[i] = MIN_PWM;
    }

    int j = 0;
    for (int i = 0; i < N_CRs; i++)
    {
//        std::cout << "PWM_" << i << ": " << PWM_in[i] << ", ";
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
//    std::cout << std::endl;

    this->PWM_to_Engine = this->PWM_for_Voter[0];
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

    std::cout << "sensor: " << this->sensor_data << "PWM: " << this->PWM_to_Engine << std::endl;
    pwmWrite(this->PWM_PIN, this->PWM_to_Engine);
//    pwmWrite(this->PWM_PIN, 120);
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
