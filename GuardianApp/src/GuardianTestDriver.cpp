/* G*/

#include "GuardianTestDriver.h"
#include <unistd.h>
 
void GuardianTestThread(void *driverPointer); //Necessary otherwise we get an error about functions being declared


static GuardianTestDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. global
 
GuardianTestDriver::GuardianTestDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      1000,
                                                      asynOctetMask | asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
                                                      asynOctetMask | asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask, // interruptMask
                                                      ASYN_MULTIDEVICE, // asynFlags
                                                      1, // autoConnect
                                                      0, // priority
                                                      0 // stackSize
                                                     )
{
    createParam(TEST_TRIGGER_STRING, asynParamUInt32Digital, &TestTriggerIndex);
    createParam(DEVICE_VALUE_STRING, asynParamFloat64, &DeviceTestValueIndex);
    createParam(LOGIC_TYPE_VALUE_STRING, asynParamInt32, &LogicTypeValueIndex);
    createParam(STORED_VALUE_STRING, asynParamFloat64, &StoredValueIndex);
    createParam(TOLERANCE_VALUE_STRING, asynParamFloat64, &ToleranceValueIndex);
    createParam(TOLERANCE_ID_STRING, asynParamInt32, &ToleranceIdIndex);
    createParam(MPS_PERMIT_STRING, asynParamUInt32Digital, &MpsPermitIndex);
    createParam(DEVICE_UPDATE_STRING, asynParamUInt32Digital, &DeviceTestUpdateIndex);
    createParam(DISPLAY_MSG_STRING, asynParamOctet, &DisplayMsgIndex);
    createParam(DEVICE_ID_STRING, asynParamInt32, &DeviceIdIndex);

    createParam(CONDITION_ID_STRING, asynParamInt32, &ConditionTestIdIndex);
    createParam(CONDITION_VALUE_STRING, asynParamUInt32Digital, &ConditionTestValueIndex);
    createParam(CONDITION_UPDATE_STRING, asynParamUInt32Digital, &ConditionTestUpdateIndex);
    
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

// Overload, get logicType as well
void GuardianTestDriver::getParameterValues(int deviceIndex, int &logicType,
                        double &desiredVal, double &tolVal) {
    getDoubleParam(deviceIndex, StoredValueIndex, &desiredVal); 
    int tolId;
    getIntegerParam(deviceIndex, ToleranceIdIndex, &tolId); 
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); 
    getIntegerParam(deviceIndex, LogicTypeValueIndex, &logicType);
}

void GuardianTestDriver::getParameterValues(int deviceIndex, double &desiredVal, double &tolVal) {
    getDoubleParam(deviceIndex, StoredValueIndex, &desiredVal); 
    int tolId;
    getIntegerParam(deviceIndex, ToleranceIdIndex, &tolId); 
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); 
}

void GuardianTestDriver::setConditionValue(int deviceIndex, int value) {
    setUIntDigitalParam(deviceIndex, ConditionTestValueIndex, value, 1);
    setUIntDigitalParam(ConditionTestUpdateIndex, 1, 1); // trigger condition proccess
    callParamCallbacks(); usleep(MONITOR_CYCLE_TIME);
    setUIntDigitalParam(ConditionTestUpdateIndex, 0, 1); // reset condition trigger
    callParamCallbacks(); usleep(MONITOR_CYCLE_TIME);
}

void GuardianTestDriver::testUpperTolerance(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex) {
    double tripVal = 0;
    switch (logicType) { // Trigger trip by going slightly over tolerance (+ 1)
        case outsidePercentageTolerance: case outsideAbsPercentageTolerance: 
            tripVal = desiredVal + (desiredVal * tolVal) + 1;
            break;
        case outsideAbsValueTolerance:
            tripVal = desiredVal + tolVal + tolVal;
            break;
        case outsideCollimatorTolerance:
            tripVal = desiredVal + tolVal + 1;
            break;
        case outsideAbsDifferencePercentageTolerance:
            tolVal = tolVal * 0.01;
            tripVal = desiredVal + (desiredVal * tolVal) + 1;
            break;
    }
    
    setDoubleParam(deviceIndex, DeviceTestValueIndex, tripVal); // write the trip value to the device
    setUIntDigitalParam(DeviceTestUpdateIndex, 1, 1); // Write to trigger test devices proccessing
    callParamCallbacks();
    usleep(MONITOR_CYCLE_TIME); // sleep to give time for epics records to be written, adjust if needed

    uint32_t mpsPermitVal = 3; int trippedDeviceId;
    getUIntDigitalParam(MpsPermitIndex, &mpsPermitVal, 1);
    getIntegerParam(DeviceIdIndex, &trippedDeviceId);
    std::ostringstream oss;
    oss << "testUpperTolerance() for device " << deviceIndex << ", tripVal = " << tripVal << ", mpsVal: " 
                << mpsPermitVal << ",\n" << "\t TrippedDeviceId: " << trippedDeviceId << ", logicType: " << logicType <<
                  ", desiredVal: " << desiredVal << ", tolVal: " << tolVal << "\n";
    std::string caseMessage = oss.str();
    // Trip is suppposed to trigger (mpsPermit = 0), if it didnt then it failed
    // And the device that tripped must match the Guardian device
    // Special cases use multiple deviceIndexes but refers to one case
    if (mpsPermitVal == 0 && (trippedDeviceId == deviceIndex || trippedDeviceId == specialIndex)) {
        std::cout << "++ PASS ++ " << " testUpperTolerance() for device " << deviceIndex << "\n"; // << caseMessage;
    }
    else {
        std::cout << "--- FAIL --- " << caseMessage;
    }

    // reset back to normal value
    setDoubleParam(deviceIndex, DeviceTestValueIndex, desiredVal);
    setUIntDigitalParam(DeviceTestUpdateIndex, 0, 1);
    callParamCallbacks();
    usleep(MONITOR_CYCLE_TIME);
}

// Almost identical to testUpperTolerance()
void GuardianTestDriver::testLowerTolerance(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex) {
    double tripVal = 0;
    switch (logicType) { // Trigger trip by going slightly under tolerance (- 1)
        case outsidePercentageTolerance: case outsideAbsPercentageTolerance: 
            tripVal = desiredVal - (desiredVal * tolVal) - 1;
            break;
        case outsideAbsValueTolerance: 
            tripVal = desiredVal - tolVal - tolVal;
            break;
        case outsideCollimatorTolerance:
            tripVal = desiredVal - tolVal - 1;
            break;
        case outsideAbsDifferencePercentageTolerance:
            tolVal = tolVal * 0.01;
            tripVal = desiredVal - (desiredVal * tolVal) - 1;
            break;
    }
    
    setDoubleParam(deviceIndex, DeviceTestValueIndex, tripVal); // write the trip value to the device
    setUIntDigitalParam(DeviceTestUpdateIndex, 1, 1); // Write to trigger test devices proccessing
    callParamCallbacks();
    usleep(MONITOR_CYCLE_TIME); // sleep to give time for epics records to be written, adjust if needed

    uint32_t mpsPermitVal = 3; int trippedDeviceId;
    getUIntDigitalParam(MpsPermitIndex, &mpsPermitVal, 1);
    getIntegerParam(DeviceIdIndex, &trippedDeviceId);
    std::ostringstream oss;
    oss << "testLowerTolerance() for device " << deviceIndex << ", tripVal = " << tripVal << ", mpsVal: " 
                << mpsPermitVal << ",\n" << "\t TrippedDeviceId: " << trippedDeviceId << ", logicType: " << logicType <<
                  ", desiredVal: " << desiredVal << ", tolVal: " << tolVal << "\n";
    std::string caseMessage = oss.str();
    // Trip is suppposed to trigger (mpsPermit = 0), if it didnt then it failed
    // And the device that tripped must match the Guardian device
    // Special cases use multiple deviceIndexes but refers to one case
    if (mpsPermitVal == 0 && (trippedDeviceId == deviceIndex || trippedDeviceId == specialIndex)) {
        std::cout << "++ PASS ++ " << " testLowerTolerance() for device " << deviceIndex << "\n"; // << caseMessage;
    }
    else {
        std::cout << "--- FAIL --- " << caseMessage;
    }

    // reset back to normal value
    setDoubleParam(deviceIndex, DeviceTestValueIndex, desiredVal);
    setUIntDigitalParam(DeviceTestUpdateIndex, 0, 1);
    callParamCallbacks();
    usleep(MONITOR_CYCLE_TIME);
}

void GuardianTestDriver::testTolerances(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex) {
    pGDriver->testUpperTolerance(deviceIndex, logicType, desiredVal, tolVal, specialIndex);
    pGDriver->testLowerTolerance(deviceIndex, logicType, desiredVal, tolVal, specialIndex);
}

void GuardianTestDriver::testSpecialCase(int deviceIndex, double desiredVal, double tolVal) {

    switch (deviceIndex) 
    {
    case 0: // devices 1,2,3 as well
        std::cout << "<<< Start special case 0 (also 1, 2, 3) >>>\n";
        /* if conditionVal == 1 */
        setConditionValue(1, 1);

        // compare current val/tol of device 0 to desired value of device 0
        getParameterValues(0, desiredVal, tolVal);
        testTolerances(0, outsidePercentageTolerance, desiredVal, tolVal, 0); 

        // compare current val/tol of device 1 to desired value of device 0
        getParameterValues(1, desiredVal, tolVal);
        getDoubleParam(0, StoredValueIndex, &desiredVal); 
        testTolerances(1, outsidePercentageTolerance, desiredVal, tolVal, 0);

        // reset back to default of 0
        setConditionValue(1, 0);

        /* if conditionVal2 == 1 */
        setConditionValue(2, 1);

        // compare current val/tol of device 2 to desired value of device 2
        getParameterValues(2, desiredVal, tolVal);
        testTolerances(2, outsidePercentageTolerance, desiredVal, tolVal, 0);
        
        // compare current val/tol of device 3 to desired value of device 2
        getParameterValues(3, desiredVal, tolVal);
        getDoubleParam(2, StoredValueIndex, &desiredVal); 
        testTolerances(3, outsidePercentageTolerance, desiredVal, tolVal, 0);

        // reset back to default of 0
        setConditionValue(2, 0);
        std::cout << "<<< Finished special case 0 (also 1, 2, 3) >>>\n";
        break;
    case 4: // devices 5,6 as well
        std::cout << "<<< Start special case 4 (also 5, 6) >>>\n";
        /* if conditionVal == 1 */
        setConditionValue(3, 1);

        // compare current val/tol of device 5 to desired value of device 4
        getParameterValues(5, desiredVal, tolVal);
        getDoubleParam(4, StoredValueIndex, &desiredVal); 
        testTolerances(5, outsidePercentageTolerance, desiredVal, tolVal, 4);

        // compare current val/tol of device 6 to desired value of device 6
        getParameterValues(6, desiredVal, tolVal);
        testTolerances(6, outsidePercentageTolerance, desiredVal, tolVal, 4);

        // reset back to default of 0
        setConditionValue(3, 0);
        std::cout << "<<< Finished special case 4 (also 5, 6) >>>\n";
        break;  
    case 7: // devices 8,9 as well
        std::cout << "<<< Start special case 7 (also 8, 9) >>>\n";
        /* if conditionVal == 1 */
        setConditionValue(4, 1);

        // compare current val/tol of device 8 to desired value of device 7
        getParameterValues(8, desiredVal, tolVal);
        getDoubleParam(7, StoredValueIndex, &desiredVal); 
        testTolerances(8, outsidePercentageTolerance, desiredVal, tolVal, 7);

        /* if conditionVal2 == 1 */
        setConditionValue(5, 1);

        // compare current val/tol of device 8 to desired value of device 9
        getDoubleParam(9, StoredValueIndex, &desiredVal); 
        testTolerances(8, outsidePercentageTolerance, desiredVal, tolVal, 7);

        // compare current val/tol of device 7 to desired value of device 7
        getParameterValues(7, desiredVal, tolVal);
        testTolerances(7, outsidePercentageTolerance, desiredVal, tolVal, 7);

        // reset back to default of 0
        setConditionValue(4, 0);
        setConditionValue(5, 0);

        std::cout << "<<< Finished special case 7 (also 8, 9) >>>\n";
        break;
    case 10: // devices 11,12 as well
        std::cout << "<<< Start special case 10 (also 11, 12) >>>\n";
        /* if conditionVal == 1 */
        setConditionValue(6, 1);

        // Skip testing the device 12 > 25000 since its just a simple comparison

        // compare current val/tol of device 10 to desired value of device 10
        getParameterValues(10, desiredVal, tolVal);
        testTolerances(10, outsidePercentageTolerance, desiredVal, tolVal, 10);

        /* if conditionVal2 == 1 */
        setConditionValue(7, 1);

        // compare current val/tol of device 11 to desired value of device 11
        getParameterValues(11, desiredVal, tolVal);
        testTolerances(11, outsidePercentageTolerance, desiredVal, tolVal, 10);

        // reset back to default of 0
        setConditionValue(6, 0);
        setConditionValue(7, 0);

        std::cout << "<<< Finished special case 10 (also 11, 12) >>>\n";
        break;
    case 13: // device 14 as well
        // compare current val/tol of device 13 to desired value of device 13
        getParameterValues(13, desiredVal, tolVal);
        testTolerances(13, outsidePercentageTolerance, desiredVal, tolVal, 13);
        // compare current val/tol of device 14 to desired value of device 14
        getParameterValues(14, desiredVal, tolVal);
        testTolerances(14, outsidePercentageTolerance, desiredVal, tolVal, 13);
    }
}

void GuardianTestDriver::runTestCases()
{
    std::cout << "\n\t=== TESTING ===\n\n";
    int logicType; double desiredVal, tolVal;
    for (int deviceIndex = 0; deviceIndex < DEVICE_PARAMS_SIZE; deviceIndex++) {
        getParameterValues(deviceIndex, logicType, desiredVal, tolVal);
        if (logicType == 0) { // special case
            pGDriver->testSpecialCase(deviceIndex, desiredVal, tolVal);
        }
        else {
            pGDriver->testTolerances(deviceIndex, logicType, desiredVal, tolVal);
        }
    }
}

void GuardianTestDriver::GuardianTestThread(void)
{
    sleep(3); // Important - Let epics records load first then initialize. C++ thread runs faster than epics can initalize itself

    callParamCallbacks(); // Call this to write the values back into epics records
    // getIntegerParam(DeviceParamSizeIndex, &DEVICE_PARAMS_SIZE);
    std::cout << "Initializing test\n\n";
    for (int conditionIndex=1; conditionIndex < CONDITION_PARAMS_SIZE; conditionIndex++) {
        setConditionValue(conditionIndex,1);
        setConditionValue(conditionIndex,0);
    }
    std::cout << "Finished initializing test\n";

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