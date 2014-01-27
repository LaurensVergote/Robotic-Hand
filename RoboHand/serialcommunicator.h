/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#ifndef SERIALCOMMUNICATOR_H
#define SERIALCOMMUNICATOR_H

#include <Python.h>
#include <QString>
#include <string>

#include "qnode.h"
/*
enum Motors{
    Thumb0, Thumb1, Thumb2, Thumb3,
    Index0, Index1, Index2, Index3,
    Middle0, Middle1, Middle2, Middle3,
    Ring0, Ring1, Ring2, Ring3,
    Little0, Little1, Little2, Little3
};

static const std::string JointNames[] = {
    "j0", "j1", "j2", "j3",
    "j4", "j5", "j6", "d6",
    "j7", "j8", "j9", "d9",
    "j10", "j11", "j12", "d12",
    "j13", "j14", "j15", "d15"
};
*/

static const int aBRSize = 17; // amount of Joints
static const int acceptedBaudRates[] = {
    50, 75, 110, 134, 150, 200, 300, 600,
    1200, 1800, 2400, 4800, 9600, 19200,
    38400, 57600, 115200
};

class SerialCommunicator
{
public:
    SerialCommunicator(int comPort = 9, int baudRate = 9600);
    ~SerialCommunicator();
    void sendCommand(std::string jointName, int rotation);
    void closeConnection();
    void openConnection();

    bool setBaudRate(int baudRate){
        for(int i = 0 ; i < aBRSize ; i++){
            if(baudRate == acceptedBaudRates[i])
            {
                this->baudRate = baudRate;
                if(pInstance)
                    PyObject_CallMethod(pInstance, "setBaudRate", "(i)", this->baudRate);
                return true;
            }
        }
        return false;
    }
    int getBaudRate() const{
        return this->baudRate;
    }

    void setComPort(int comPort){
        this->comPort = comPort;
        if(pInstance)
            PyObject_CallMethod(pInstance, "setComPort", "(i)", this->comPort);
    }

    int getComPort() const{
        return this->comPort;
    }

private:
    int comPort, baudRate;
    PyObject *pName, *pModule, *pDict, *pClass, *pInstance;
};

#endif // SERIALCOMMUNICATOR_H
