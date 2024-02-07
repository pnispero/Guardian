#include "counter.h"
#include <unistd.h>
 
void counterTask(void *driverPointer); //Necessary otherwise we get an error about functions being declared
 
CounterDriver::CounterDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      1,
                                                      asynFloat64Mask | asynInt32Mask | asynDrvUserMask,
                                                      asynFloat64Mask | asynInt32Mask,
                                                      ASYN_MULTIDEVICE,
                                                      1,
                                                      0,
                                                      0
                                                     )
{
    createParam(CounterString, asynParamFloat64, &CounterIndex);
    createParam(StepString, asynParamInt32, &StepIndex);
    asynStatus status;
    status = (asynStatus)(epicsThreadCreate("CounterTask", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::counterTask, this) == NULL);
    if (status)
    {
        printf("Thread didn't launch please do something else");
        return;
    }
}
 
void counterTask(void* driverPointer)
{
    CounterDriver *pPvt = (CounterDriver *) driverPointer;
    pPvt->counterTask();
}
 
void CounterDriver::counterTask(void)
{
    int step;
    double count;
    while(1)
    {
        sleep(1); //Necessary because the thread will go faster than I/O Scan will accept and write inputs
        getDoubleParam(CounterIndex, &count);
        getIntegerParam(StepIndex, &step);
 
        setDoubleParam(CounterIndex, count + step);
        setIntegerParam(StepIndex, step + 1);
        callParamCallbacks();
    }
}
 
extern "C"
{
    int CounterDriverConfigure(const char* portName)
    {
        new CounterDriver(portName);
        return asynSuccess;
    }
    static const iocshArg initArg0 = {"portName", iocshArgString};
    static const iocshArg * const initArgs[] = {&initArg0};
    static const iocshFuncDef initFuncDef = {"CounterDriverConfigure", 1, initArgs};
    static void initCallFunc(const iocshArgBuf *args)
    {
        CounterDriverConfigure(args[0].sval);
    }
    void CounterDriverRegister(void)
    {
        iocshRegister(&initFuncDef, initCallFunc);
    }
    epicsExportRegistrar(CounterDriverRegister);
}