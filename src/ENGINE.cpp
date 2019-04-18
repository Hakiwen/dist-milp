//
// Created by tkhamvilai on 4/14/19.
//

#include "ENGINE.hpp"

ENGINE::ENGINE(int N_CRs)
{
    this->sensor_data = 0;

    this->PWM_out = 0;
    this->PWM_in = new int[N_CRs];

    this->PWM_for_Voter = new int[N_APP_TO_VOTE];
    this->PWM_for_Voter_ind = new int[N_APP_TO_VOTE];
    this->PWM_to_Engine = 0;

    for (int i = 0; i < N_CRs; i++)
    {
        this->PWM_in[i] = 0;
    }

    for (int i = 0; i < N_APP_TO_VOTE; i++)
    {
        this->PWM_for_Voter[i] = 0;
        this->PWM_for_Voter_ind[i] = 0;
    }

    this->EngineSetup = 0;
    this->SensorSetup = 0;

    this->ch = NULL;

    this->fault_detect = 0;
    this->fault_from_voter = 0;

    this->voter_delay = 0;
    this->write_delay = 0;

    std::ofstream myfile;
    myfile.open ("data.txt", std::ios::out);
    myfile << "";
    myfile.close();
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
        delay(5000); // uncommemt this when using the engine
    }

    if(this->PWM_to_Engine < MIN_PWM)
    {
        this->PWM_to_Engine = MIN_PWM;
    }
    else if(this->PWM_to_Engine > MAX_PWM)
    {
        this->PWM_to_Engine = MAX_PWM;
    }

//    std::cout << "sensor: " << this->sensor_data << " PWM: " << this->PWM_to_Engine << std::endl;
    pwmWrite(this->PWM_PIN, this->PWM_to_Engine);
}

void ENGINE::voter(int N_CRs)
{
    for (int i = 0; i < N_APP_TO_VOTE; i++)
    {
        this->PWM_for_Voter[i] = MIN_PWM;
        this->PWM_for_Voter_ind[i] = 0;
    }

    int j = 0;
    for (int i = 0; i < N_CRs; i++)
    {
//        std::cout << "PWM_" << i+1 << ": " << PWM_in[i] << ", ";
        if (this->PWM_in[i] > MIN_PWM)
        {
            this->PWM_for_Voter[j] = this->PWM_in[i];
            this->PWM_for_Voter_ind[j] = i + 1;
            j++;
            if(j == N_APP_TO_VOTE)
            {
                break;
            }
        }
    }

//    for (int i = 0; i < N_APP_TO_VOTE; i++) {
//        std::cout << "PWM_" << PWM_for_Voter_ind[i] << ": " << PWM_for_Voter[i] << ", ";
//    }
//    std::cout << std::endl;

    if(j > 1)
    {
        this->fault_detect = this->error_detector(this->PWM_for_Voter);
        this->PWM_to_Engine = (int) this->voter_mean(this->PWM_for_Voter, this->fault_detect);

        if(this->voter_delay >= MAX_VOTER_DELAY)
        {
            this->voter_delay = 0;
            if (this->fault_detect > 0 && this->fault_detect <= N_APP_TO_VOTE)
            {
                this->fault_from_voter = this->PWM_for_Voter_ind[this->fault_detect - 1];
            }
            else if (this->fault_detect == 0 || this->fault_detect == 6)
            {
                this->fault_from_voter = 0;
            }
        }
        else
        {
            this->voter_delay += 1;
            this->fault_from_voter = 0;
        }
    }
    else // only one signal left
    {
        this->fault_detect = 0;
        this->fault_from_voter = 0;
        this->PWM_to_Engine = MIN_PWM;
    }

//    std::cout << "fault_detect: " << this->fault_detect << std::endl;
//    std::cout << "fault_from_voter: " << this->fault_from_voter << std::endl;
}

int ENGINE::error_detector(int* array)
{
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

double ENGINE::voter_mean(int* array, int err_detector_result)
{
    if (err_detector_result == 6)
    {
        return MIN_PWM; // all signals are different
    }
    else
    {
        double sum = 0;
        for(int i = 0 ; i < N_APP_TO_VOTE ; i++)
        {
            sum = sum + ((double) array[i])*(err_detector_result != i+1) / ( 3*(err_detector_result == 0) + 2*(err_detector_result != 0) );
        }
        return sum;
    }
}

void ENGINE::write_data()
{
    if (this->write_delay >= MAX_WRITE_DELAY)
    {
        this->write_delay = 0;
        std::ofstream myfile;
        myfile.open ("data.txt", std::ios::out | std::ios::app);
        for (int i = 0; i < N_APP_TO_VOTE; i++)
        {
            myfile << this->PWM_for_Voter[i] << ",";
        }
        myfile << this->PWM_to_Engine << ",";
        myfile << this->sensor_data << "\n";
        myfile.close();
    }
    else
    {
        this->write_delay += 1;
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
