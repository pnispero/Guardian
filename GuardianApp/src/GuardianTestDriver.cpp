/* G*/

#include "GuardianTestDriver.h"
#include <unistd.h>
 
void GuardianTestThread(void *driverPointer); //Necessary otherwise we get an error about functions being declared


static GuardianTestDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. global
 
GuardianTestDriver::GuardianTestDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      1000,
                                                      asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
                                                      asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask, // interruptMask
                                                      ASYN_MULTIDEVICE, // asynFlags
                                                      1, // autoConnect
                                                      0, // priority
                                                      0 // stackSize
                                                     )
{
    createParam(TEST_TRIGGER_STRING, asynParamUInt32Digital, &TestTriggerIndex);
    createParam(DEVICE_VALUE_STRING, asynParamFloat64, &DeviceTestValueIndex);
    asynStatus status;
    status = (asynStatus)(epicsThreadCreate("GuardianTestThread", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::GuardianTestThread, this) == NULL);
    if (status)
    {
        printf("Thread didn't launch please do something else");
        return;
    }
}

void GuardianTestThread(void* driverPointer)
{
    pGDriver->GuardianTestThread();
}

void GuardianTestDriver::runTestCases()
{
// TODO:
    std::cout << "Running test...\n";
    // getIntegerParam(DeviceParamSizeIndex, &DEVICE_PARAMS_SIZE);
// Loop through every device
    for (int deviceIndex = 0; deviceIndex < 2; deviceIndex++) { // TEMP set to 2 for testing
    // for (int deviceIndex = 0; deviceIndex < DEVICE_PARAMS_SIZE; deviceIndex++) {
        // TODO: Create storedValue and TolValue pvs to grab from real guardian
        double desiredVal, tolVal, tripVal;
        // getDoubleParam(deviceIndex, StoredValueIndex, &desiredVal); 
        // getDoubleParam(deviceIndex, ToleranceValueIndex, &tolVal); 
        // // TODO: tripVal will vary based off logic type
        // tripVal = desiredVal + (desiredVal * tolVal)
        // setDoubleParam(deviceIndex, DeviceTestValueIndex, tripVal);

        // TODO: Create outMessage pv and mps input pv to grab from real guardian
        
    }
// For each device, get the stored value and the tolerance. caput to the devices test db,
// a trip value which is the stored value + the tolerance + small amount over the tolerance.
// (this would look slightly different for the different types). 

// Then we can caget the guardian outMessage PV to see if the message matches the device that
// was intentionally tripped. If it matches, then the guardian successfully tripped.
// If no match, then the test case failed. Should also caget the mps input pv (
// MPS:UNDS:1:SXRSS_GUARDIAN_OK) to see if it was set to 0.

// Once test case finished, record the success/failure, then reset the device test db 
// value to initial state, and go to next test case.
// Then make test cases for the special cases, which can't be generalized.
}

void GuardianTestDriver::GuardianTestThread(void)
{
    sleep(5); // Important - Let epics records load first then initialize. C++ thread runs faster than epics can initalize itself

    callParamCallbacks(); // Call this to write the values back into epics records

    while (true) {
        sleep(1); 

        // Check test_trg param, take test then reset if true
        uint32_t testTriggerVal;
        getUIntDigitalParam(TestTriggerIndex, &testTriggerVal, 1);
        if (testTriggerVal == 1) {
            pGDriver->runTestCases();
            setUIntDigitalParam(TestTriggerIndex, 0, 1);
            callParamCallbacks();

            getUIntDigitalParam(TestTriggerIndex, &testTriggerVal, 1); 
        }
    }
}

/* Configure the guardian driver which may include the port, filepath maybe other ports */
int GuardianTestDriverConfigure(const char* portName)
{
    pGDriver = new GuardianTestDriver(portName);
    return asynSuccess;
}
 
static const iocshArg initArg0 = {"portName", iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"GuardianTestDriverConfigure", 1, initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    GuardianTestDriverConfigure(args[0].sval);
}
void GuardianTestDriverRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

extern "C"
{
    epicsExportRegistrar(GuardianTestDriverRegister);
}