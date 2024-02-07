/* G*/

#include "GuardianDriver.h"
#include <unistd.h>
 
void counterTask(void *driverPointer); //Necessary otherwise we get an error about functions being declared

static GuardianDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. Used for iocShell commands
 
GuardianDriver::GuardianDriver(const char *portName) : asynPortDriver                                                      (
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
    GuardianDriver *pPvt = (GuardianDriver *) driverPointer;
    pPvt->counterTask();
}
 
void GuardianDriver::counterTask(void)
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