/****************************************************************************
** Author: Laurens Vergote
****************************************************************************/

#include "serialcommunicator.h"
#include "hand.h"

SerialCommunicator::SerialCommunicator(int comPort, int baudRate)
{
    this->baudRate = 0;
    this->comPort = comPort;
    for(int i = 0 ; i < aBRSize ; i++)
    {
        if(baudRate == acceptedBaudRates[i])
            this->baudRate = baudRate;
    }

    if(this->baudRate == 0)
        this->baudRate = 9600;

    //Python
    Py_Initialize();

    pName = PyString_FromString("Serializer");
    pModule = PyImport_Import(pName);

    if(pModule != NULL)
    {
        pDict = PyModule_GetDict(pModule);

        //Build name of the callable class
        pClass = PyDict_GetItemString(pDict, "Serializer");

        //Create an instance of the class
        if(PyCallable_Check(pClass)){
            pInstance = PyObject_CallObject(pClass, NULL);
        }

        //Build parameter list
        PyRun_SimpleString("import serial");
        PyObject_CallMethod(pInstance, "setBaudRate", "(i)", this->baudRate);
        PyObject_CallMethod(pInstance, "setComPort", "(i)", this->comPort);
        PyObject_CallMethod(pInstance, "createSerialPort", NULL);
        PyObject_CallMethod(pInstance, "createConnection", NULL);
        PyObject_CallMethod(pInstance, "openConnection", NULL);
    }
    else
    {
        if(PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function \"%s\"\n", "failed");
    }

}

SerialCommunicator::~SerialCommunicator()
{
    Py_DecRef(pName);
    Py_DecRef(pModule);
    Py_DecRef(pDict);
    Py_DecRef(pClass);
    Py_Finalize();
}

void SerialCommunicator::openConnection()
{
    PyObject_CallMethod(pInstance, "openConnection", NULL);
}

void SerialCommunicator::closeConnection()
{
    PyObject_CallMethod(pInstance, "closeConnection", NULL);
}

void SerialCommunicator::sendCommand(std::string jointName, int rotation)
{
    byte selector = 0x00, rotator= 0x00;
    //Selector
    for(byte i = 0 ; i < JointAmount ; i++)
    {
        if(jointName == JointStrings[i])
            selector = i;
    }
    selector |= 0x20;

    //Rotator
    rotator = (byte)((abs(rotation) % 127) * 1);

    PyObject_CallMethod(pInstance, "sendData", "BB", selector, rotator);
}
