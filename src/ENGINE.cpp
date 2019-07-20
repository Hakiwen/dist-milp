//
// Created by tkhamvilai on 4/14/19.
//

#include "ENGINE.hpp"

ENGINE::ENGINE(int N_CRs)
{
#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )
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
#elif defined ( USE_X_PLANE_SIMULATOR )
    this->delta_a_out = INVALID_DELTA_A_VAL;
    this->delta_a_in = new float[N_CRs];

    this->delta_a_for_Voter = new float[N_APP_TO_VOTE];
    this->delta_a_for_Voter_ind = new float[N_APP_TO_VOTE];
    this->delta_a_to_X_plane = 0;

    for (int i = 0; i < N_CRs; i++)
    {
        this->delta_a_in[i] = INVALID_DELTA_A_VAL;
    }

    for (int i = 0; i < N_APP_TO_VOTE; i++)
    {
        this->delta_a_for_Voter[i] = 0;
        this->delta_a_for_Voter_ind[i] = 0;
    }

    this->roll_deg = 0;
    this->roll_dot = 0;
#endif

    this->EngineSetup = 0;
    this->SensorSetup = 0;

#ifndef __x86_64__
    this->ch = NULL;
#endif

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

#ifndef __x86_64__
#if defined ( USE_ENGINE_W_FEEDBACK )
        PhidgetVoltageRatioInput_create(&this->ch);
        AskForDeviceParameters(&this->channelInfo, (PhidgetHandle *)&this->ch);
        Phidget_setDeviceSerialNumber((PhidgetHandle)this->ch, this->channelInfo.deviceSerialNumber);
        Phidget_setHubPort((PhidgetHandle)this->ch, this->channelInfo.hubPort);
        Phidget_setIsHubPortDevice((PhidgetHandle)ch, this->channelInfo.isHubPortDevice);
        Phidget_setChannel((PhidgetHandle)this->ch, this->channelInfo.channel);

        Phidget_setOnAttachHandler((PhidgetHandle)this->ch, onAttachHandler, NULL);
        Phidget_setOnDetachHandler((PhidgetHandle)this->ch, onDetachHandler, NULL);

        PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(ch, onVoltageRatioChangeHandler, this);

        Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000);
#elif defined ( USE_X_PLANE_SIMULATOR )
        this->udp.init(X_PLANE_IP_ADDRESS, "192.168.0.19", X_PLANE_PORT, X_PLANE_PORT);
#endif
#else
#if defined ( USE_X_PLANE_SIMULATOR )
        this->udp.init(X_PLANE_IP_ADDRESS, PC_IP_ADDRESS, X_PLANE_PORT, X_PLANE_PORT);
#endif
#endif
    }

#if defined ( USE_ENGINE_W_FEEDBACK )
    //    std::cout << "sensor: " << this->sensor_data << std::endl;
#elif defined ( USE_X_PLANE_SIMULATOR )
    if(this->udp.initYet)
    {
        this->udp.receivePacket(this->data, X_PLANE_RECEIVE_PACKET_BYTE);
    }
    this->roll_deg = Decode_Roll_X_plane (data);
    this->roll_dot = Decode_Roll_Dot_X_plane (data);
    std::cout << "roll_deg = " << this->roll_deg << std::endl;
    std::cout << "roll_dot = " << this->roll_dot << std::endl;
#endif
}

void ENGINE::pwm_send()
{
#if defined ( USE_ENGINE_W_FEEDBACK ) || defined ( USE_ENGINE_WO_FEEDBACK )
    if(!this->EngineSetup)
    {
        this->EngineSetup = 1;

#ifndef __x86_64__
        this->PWM_PIN = 1;
        pinMode(this->PWM_PIN, PWM_OUTPUT);
        pwmSetMode (PWM_MODE_MS);
        pwmSetRange (2000);
        pwmSetClock (192);
//        pwmWrite(this->PWM_PIN, 200); // for calibration only
//        delay(10000);
        pwmWrite(this->PWM_PIN, 100);
        delay(5000); // uncommemt this when using the engine
#endif

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

#ifndef __x86_64__
    pwmWrite(this->PWM_PIN, this->PWM_to_Engine);
#endif
#endif

#ifdef USE_X_PLANE_SIMULATOR
//    std::cout << this->delta_a_to_X_plane << std::endl;
    Encode_Delta_to_X_plane(this->delta_a_to_X_plane, this->buf);
    udp.sendPacket(this->buf, X_PLANE_SEND_PACKET_BYTE);
#endif
}

#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )
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
        std::cout << "PWM_" << i+1 << ": " << PWM_in[i] << ", ";
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
#elif defined ( USE_X_PLANE_SIMULATOR )
void ENGINE::voter(float N_CRs)
{
    for (int i = 0; i < N_APP_TO_VOTE; i++)
    {
        this->delta_a_for_Voter[i] = 999;
        this->delta_a_for_Voter_ind[i] = 0;
    }

    int j = 0;
    for (int i = 0; i < N_CRs; i++)
    {
//        std::cout << "PWM_" << i+1 << ": " << delta_a_in[i] << ", ";
        if (this->delta_a_in[i] != 999)
        {
            this->delta_a_for_Voter[j] = this->delta_a_in[i];
            this->delta_a_for_Voter_ind[j] = i + 1;
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
        this->fault_detect = this->error_detector(this->delta_a_for_Voter);
        this->delta_a_to_X_plane = (float) this->voter_mean(this->delta_a_for_Voter, this->fault_detect);

        if(this->voter_delay >= MAX_VOTER_DELAY)
        {
            this->voter_delay = 0;
            if (this->fault_detect > 0 && this->fault_detect <= N_APP_TO_VOTE)
            {
                this->fault_from_voter = this->delta_a_for_Voter_ind[this->fault_detect - 1];
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
        this->delta_a_to_X_plane = 90;
    }

//    std::cout << "fault_detect: " << this->fault_detect << std::endl;
//    std::cout << "fault_from_voter: " << this->fault_from_voter << std::endl;
}

int ENGINE::error_detector(float* array)
{
    int v1 = (int)array[0];
    int v2 = (int)array[1];
    int v3 = (int)array[2];
    /* Returns the number corresponding to the wrong value among three ones, 0 if they all match and 6 if they all mismatch
     * get the values from the three redundant applications
     */

    int mismatch12 = ( abs(v1 - v2) > TOLERANCE );
    int mismatch13 = ( abs(v1 - v3) > TOLERANCE );
    int mismatch23 = ( abs(v2 - v3) > TOLERANCE );

    return 1*mismatch12*mismatch13 + 2*mismatch12*mismatch23 + 3*mismatch13*mismatch23;
}

double ENGINE::voter_mean(float* array, float err_detector_result)
{
    if (err_detector_result == 6)
    {
        return INVALID_DELTA_A_VAL; // all signals are different
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
#endif

void ENGINE::write_data()
{
    if (this->write_delay >= MAX_WRITE_DELAY)
    {
        this->write_delay = 0;
        std::ofstream myfile;
        myfile.open ("data.txt", std::ios::out | std::ios::app);
        for (int i = 0; i < N_APP_TO_VOTE; i++)
        {
#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )
            myfile << this->PWM_for_Voter[i] << ",";
#elif defined ( USE_X_PLANE_SIMULATOR )
            myfile << this->delta_a_for_Voter[i] << ",";
#endif
        }
#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )
        myfile << this->PWM_to_Engine << ",";
#if defined (USE_ENGINE_W_FEEDBACK)
        myfile << this->sensor_data << "\n";
#endif
#elif defined ( USE_X_PLANE_SIMULATOR )
        myfile << this->delta_a_to_X_plane << ",";
        myfile << this->roll_deg << ",";
        myfile << this->roll_dot << "\n";
#endif
        myfile.close();
    }
    else
    {
        this->write_delay += 1;
    }
}

void ENGINE::run(int N_CRs)
{
#if defined (__x86_64__)
#if defined (USE_ENGINE_W_FEEDBACK) || defined ( USE_ENGINE_WO_FEEDBACK )
#if ( PRINT )
    std::cout << "I'm the jet engine!" << std::endl;
#endif
#elif defined ( USE_X_PLANE_SIMULATOR )
#if ( PRINT )
    std::cout << "I'm the X-plane!" << std::endl;
#endif
    this->read_sensor();
    this->voter(N_CRs);
    this->pwm_send();
    this->write_data();
#endif
#else
#if !defined ( USE_ENGINE_WO_FEEDBACK )
        this->read_sensor();
#endif
        this->voter(N_CRs);
        this->pwm_send();
        this->write_data();
#endif
}

/** Phidget Functions **/

#ifndef __x86_64__
#ifdef USE_ENGINE_W_FEEDBACK
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
    Engine->sensor_data = voltageRatio*1000000.0;
}
#endif
#endif

/** X-plane Functions **/
#ifdef USE_X_PLANE_SIMULATOR
float Decode_Roll_X_plane (uint8_t *data)
{
    uint32_t roll1, roll2, roll3, roll4, roll_temp, yy=255;
    float roll_deg;

    // Calculating the Roll Angle of the Airplane
    roll1 = (data[85]);//^xx); Transforming 8bits representation to 32bits representation
    roll1 = (roll1&yy); // And with yy to ensure that the first bits are 0's

    roll2 = (data[86]);//^xx);Transforming 8bits representation to 32bits representation
    roll2 = (roll2&yy);// And with yy to ensure that the first bits are 0's

    roll3 = (data[87]);//^xx);Transforming 8bits representation to 32bits representation
    roll3 = (roll3&yy);// And with yy to ensure that the first bits are 0's

    roll4 = (data[88]);//^xx);Transforming 8bits representation to 32bits representation
    roll4 = (roll4&yy);// And with yy to ensure that the first bits are 0's

    // Concatenate the 4 bytes
    roll_temp = (roll4<<24)|(roll3<<16);
    roll_temp = roll_temp|(roll2<<8);
    roll_temp = roll_temp|(roll1);
    roll_deg = *(float*)&roll_temp; //X-Plane uses single-precision floating-point to represent angles

    return roll_deg;
}

float Decode_Roll_Dot_X_plane (uint8_t *data)
{
    uint32_t roll_dot1, roll_dot2, roll_dot3, roll_dot4, roll_dot_temp, yy=255;
    float roll_dot;

    // Calculating the Roll Angle of the Airplane
    roll_dot1 = (data[49]);//^xx); Transforming 8bits representation to 32bits representation
    roll_dot1 = (roll_dot1&yy); // And with yy to ensure that the first bits are 0's

    roll_dot2 = (data[50]);//^xx);Transforming 8bits representation to 32bits representation
    roll_dot2 = (roll_dot2&yy);// And with yy to ensure that the first bits are 0's

    roll_dot3 = (data[51]);//^xx);Transforming 8bits representation to 32bits representation
    roll_dot3 = (roll_dot3&yy);// And with yy to ensure that he first bits are 0's

    roll_dot4 = (data[52]);//^xx);Transforming 8bits representation to 32bits representation
    roll_dot4 = (roll_dot4&yy);// And with yy to ensure that he first bits are 0's

    // Concatenate the 4 bytes
    roll_dot_temp = (roll_dot4<<24)|(roll_dot3<<16);
    roll_dot_temp = roll_dot_temp|(roll_dot2<<8);
    roll_dot_temp = roll_dot_temp|(roll_dot1);
    roll_dot = *(float*)&roll_dot_temp;

    return roll_dot;
}

void Encode_Delta_to_X_plane (float del_a, uint8_t* buffer)
{
    uint8_t bytes_da[sizeof(float)];

    buffer[0]=68;/*"D"*/  buffer[1]=65;/*"A"*/; buffer[2]=84;/*"T"*/  buffer[3]=65;/*"A"*/ 	buffer[4]=48;/*"0"*/

    // Parameter YOKE
    buffer[5] = 8; 	buffer[6] = 0;
    buffer[7] = 0;	buffer[8] = 0;

    // Elevator Values -  Do NOT change values
    buffer[9] = 0; 	buffer[10] = 192; buffer[11] = 121; buffer[12] = 196;

    *(float*)(bytes_da) = del_a; // Converting the float delta_a (32bits) in four bytes (8bits) to be transmmited

    //Aileron - Send commands to YOKE
    buffer[13] = bytes_da[0]; buffer[14] = bytes_da[1];
    buffer[15] = bytes_da[2]; buffer[16] = bytes_da[3];

    // Rudder
    buffer[17] = 0;   buffer[18] = 192; buffer[19] = 121; buffer[20] = 196;

    // NOT USED -  But according to XPlane manual we have to transmit these remaining bytes
    buffer[21] = 0;   buffer[22] = 192; buffer[23] = 121; buffer[24] = 196;
    buffer[25] = 0;   buffer[26] = 192; buffer[27] = 121; buffer[28] = 196;
    buffer[29] = 0;   buffer[30] = 192; buffer[31] = 121; buffer[32] = 196;
    buffer[33] = 0;   buffer[34] = 192; buffer[35] = 121; buffer[36] = 196;
    buffer[37] = 0;   buffer[38] = 192; buffer[39] = 121; buffer[40] = 196;
}
#endif
