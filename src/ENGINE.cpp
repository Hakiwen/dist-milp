//
// Created by tkhamvilai on 4/14/19.
//

#include "ENGINE.hpp"
#include "phidget22.h"

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

    this->EngineSetup = 0;
    this->SensorSetup = 0;

    this->ch = NULL;
}

void ENGINE::read_sensor()
{
    if(!this->SensorSetup)
    {
        this->SensorSetup = 1;

        this->prc = PhidgetVoltageRatioInput_create(&this->ch);
        AskForDeviceParameters(&this->channelInfo, (PhidgetHandle *)&this->ch);
        this->prc = Phidget_setDeviceSerialNumber((PhidgetHandle)this->ch, this->channelInfo.deviceSerialNumber);
        this->prc = Phidget_setHubPort((PhidgetHandle)this->ch, this->channelInfo.hubPort);
        this->prc = Phidget_setIsHubPortDevice((PhidgetHandle)ch, this->channelInfo.isHubPortDevice);
        this->prc = Phidget_setChannel((PhidgetHandle)this->ch, this->channelInfo.channel);

        this->prc = Phidget_setOnAttachHandler((PhidgetHandle)this->ch, onAttachHandler, NULL);
        this->prc = Phidget_setOnDetachHandler((PhidgetHandle)this->ch, onDetachHandler, NULL);
        this->prc = Phidget_setOnErrorHandler((PhidgetHandle)this->ch, onErrorHandler, NULL);

        prc = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(ch, onVoltageRatioChangeHandler, NULL);
        prc = PhidgetVoltageRatioInput_setOnSensorChangeHandler(ch, onSensorChangeHandler, NULL);

        prc = Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000);
    }
}

void ENGINE::voter()
{

}

void ENGINE::pwm_send()
{
    if(!this->EngineSetup)
    {
        this->EngineSetup = 1;
        this->PWM_PIN = 1;
        pinMode(this->PWM_PIN, OUTPUT);
        pwmWrite(this->PWM_PIN, 1000);
        delay(5000);
    }
    pwmWrite(this->PWM_PIN, this->PWM_to_Engine);
}

static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx)
{
    PhidgetReturnCode prc; //Used to catch error codes from each Phidget function call
    Phidget_ChannelSubclass channelSubclass;
    Phidget_DeviceClass deviceClass;
    const char* channelClassName;
    int32_t serialNumber;
    int32_t hubPort;
    int32_t channel;

    printf("\nAttach Event: ");
    /*
    * Get device information and display it.
    */
    prc = Phidget_getDeviceSerialNumber(ph, &serialNumber);

    prc = Phidget_getChannel(ph, &channel);

    prc = Phidget_getChannelClassName(ph, &channelClassName);

    prc = Phidget_getDeviceClass(ph, &deviceClass);

    if (deviceClass == PHIDCLASS_VINT) {
        prc = Phidget_getHubPort(ph, &hubPort);

        printf("\n\t-> Channel Class: %s\n\t-> Serial Number: %d\n\t-> Hub Port: %d\n\t-> Channel %d\n\n", channelClassName, serialNumber, hubPort, channel);
    } else { //Not VINT
        printf("\n\t-> Channel Class: %s\n\t-> Serial Number: %d\n\t-> Channel %d\n\n", channelClassName, serialNumber, channel);
    }

    /*
    *	Set the DataInterval inside of the attach handler to initialize the device with this value.
    *	DataInterval defines the minimum time between VoltageRatioChange events.
    *	DataInterval can be set to any value from MinDataInterval to MaxDataInterval.
    */
    printf("\tSetting DataInterval to 1000ms\n");
    prc = PhidgetVoltageRatioInput_setDataInterval((PhidgetVoltageRatioInputHandle)ph, 1000);

    /*
    *	Set the VoltageRatioChangeTrigger inside of the attach handler to initialize the device with this value.
    *	VoltageRatioChangeTrigger will affect the frequency of VoltageRatioChange events, by limiting them to only occur when
    *	the voltage ratio changes by at least the value set.
    */
    printf("\tSetting VoltageRatio ChangeTrigger to 0.0\n");
    prc = PhidgetVoltageRatioInput_setVoltageRatioChangeTrigger((PhidgetVoltageRatioInputHandle)ph, 0.0);
    /*
    * Set the SensorType inside of the attach handler to initialize the device with this value.
    * You can find the appropriate SensorType for your sensor in its User Guide and the VoltageRatioInput API
    * SensorType will apply the appropriate calculations to the voltage ratio reported by the device
    * to convert it to the sensor's units.
    * SensorType can only be set for Sensor Port voltageRatio inputs(VINT Ports and Analog Input Ports)
    */
    prc = Phidget_getChannelSubclass(ph, &channelSubclass);

    if (channelSubclass == PHIDCHSUBCLASS_VOLTAGEINPUT_SENSOR_PORT) {
        printf("\tSetting VoltageRatio SensorType\n");
        prc = PhidgetVoltageRatioInput_setSensorType((PhidgetVoltageRatioInputHandle)ph, SENSOR_TYPE_VOLTAGERATIO);
    }
}

static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx)
{
    PhidgetReturnCode prc; //Used to catch error codes from each Phidget function call
    Phidget_DeviceClass deviceClass;
    const char* channelClassName;
    int32_t serialNumber;
    int32_t hubPort;
    int32_t channel;

    printf("\nDetach Event: ");
    /*
    * Get device information and display it.
    */
    prc = Phidget_getDeviceSerialNumber(ph, &serialNumber);

    prc = Phidget_getChannel(ph, &channel);

    prc = Phidget_getChannelClassName(ph, &channelClassName);

    prc = Phidget_getDeviceClass(ph, &deviceClass);

    if (deviceClass == PHIDCLASS_VINT) {
        prc = Phidget_getHubPort(ph, &hubPort);

        printf("\n\t-> Channel Class: %s\n\t-> Serial Number: %d\n\t-> Hub Port: %d\n\t-> Channel %d\n\n", channelClassName, serialNumber, hubPort, channel);
    } else { //Not VINT
        printf("\n\t-> Channel Class: %s\n\t-> Serial Number: %d\n\t-> Channel %d\n\n", channelClassName, serialNumber, channel);
    }
}

static void CCONV onErrorHandler(PhidgetHandle ph, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString)
{
    fprintf(stderr, "[Phidget Error Event] -> %s (%d)\n", errorString, errorCode);
}

static void CCONV onVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double voltageRatio)
{
    printf("[VoltageRatio Event] -> VoltageRatio: %.4g\n", voltageRatio);
}

static void CCONV onSensorChangeHandler(PhidgetVoltageRatioInputHandle ph, void *ctx, double sensorValue, Phidget_UnitInfo *unitInfo)
{
    printf("[Sensor Event] -> Sensor Value: %.4f %s\n", sensorValue, unitInfo->symbol);
}