/* G*/

#include "GuardianDriver.h"
#include <unistd.h>
 
void FELpulseEnergyMonitor(void *driverPointer); //Necessary otherwise we get an error about functions being declared


static GuardianDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. global
 
GuardianDriver::GuardianDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      1,
                                                      asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
                                                      asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask, // interruptMask
                                                      ASYN_MULTIDEVICE, // asynFlags
                                                      1, // autoConnect
                                                      0, // priority
                                                      0 // stackSize
                                                     )
{
    createParam(SNAPSHOT_TRIGGER_STRING, asynParamUInt32Digital, &SnapshotTriggerIndex);
    createParam(STORED_VALUE_STRING, asynParamFloat64, &StoredValueIndex);
    createParam(SNAPSHOT_VALUE_STRING, asynParamFloat64, &SnapshotValueIndex);
    setDoubleParam(StoredValueIndex,1);
    asynStatus status;
    status = (asynStatus)(epicsThreadCreate("FELpulseEnergyMonitor", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::FELpulseEnergyMonitor, this) == NULL);
    if (status)
    {
        printf("Thread didn't launch please do something else");
        return;
    }
}

void FELpulseEnergyMonitor(void* driverPointer)
{
    pGDriver->FELpulseEnergyMonitor();
}

void GuardianDriver::takeSnapshot()
{
    uint32_t paramIndex = 0;
    double curVal;
    asynStatus status;
    status = getDoubleParam(paramIndex, StoredValueIndex, &curVal);
    std::cout << "1: " << status << "\n";
    std::cout << "curVal after get : " << curVal << "\n"; 
    status = setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    std::cout << "2: " << status << "\n";
    callParamCallbacks();
}

void GuardianDriver::FELpulseEnergyMonitor(void)
{
    sleep(5); // Important - Let epics records load first then initialize. C++ thread runs faster than epics can initalize itself
    // Initalize parameters
    uint32_t paramIndex = 0;
    double curVal = 1;
    asynStatus status;
    status = setDoubleParam(paramIndex, StoredValueIndex, curVal);
    std::cout << "status: " << status << "\n";
    std::cout << "curVal after get : " << curVal << "\n"; 
    status = getDoubleParam(paramIndex, StoredValueIndex, &curVal);
    std::cout << "status: " << status << "\n";
    std::cout << "curVal after get : " << curVal << "\n"; 

    status = setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    std::cout << "status: " << status << "\n";
    std::cout << "curVal after set : " << curVal << "\n"; 
    status = getDoubleParam(paramIndex, SnapshotValueIndex, &curVal);
    std::cout << "status: " << status << "\n";
    std::cout << "curVal after get : " << curVal << "\n"; 

    callParamCallbacks(); // Call this to write the values back into epics records


    while (true) {
        sleep(10); 

        // Check snapshot_trg param, take snapshot then reset if true
        uint32_t snapshotTriggerVal;
        getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1);
        std::cout << "sstriggerval: " << snapshotTriggerVal << "\n"; 
        if (snapshotTriggerVal == 1) {
            pGDriver->takeSnapshot();
            setUIntDigitalParam(SnapshotTriggerIndex, 0, 1);
            callParamCallbacks();

            getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1); 

            std::cout << "Successfully triggered and resetted\n\n"; 
        }
    }
}

/* Configure the guardian driver which may include the port, filepath maybe other ports */
int GuardianDriverConfigure(const char* portName)
{
    pGDriver = new GuardianDriver(portName);
    return asynSuccess;
}
 
static const iocshArg initArg0 = {"portName", iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"GuardianDriverConfigure", 1, initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    GuardianDriverConfigure(args[0].sval);
}
void GuardianDriverRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

extern "C"
{
    epicsExportRegistrar(GuardianDriverRegister);
}