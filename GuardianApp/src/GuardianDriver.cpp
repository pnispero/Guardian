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
    createParam(CounterString, asynParamFloat64, &CounterIndex);
    createParam(StepString, asynParamInt32, &StepIndex);
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
    // Initialize map - What do we need this map for?
    std::map<uint32_t, std::string> device_data_names;


    // TEST <<<<<<<<<<<<<<<<<<<
    uint32_t paramIndex = 0;
    double curVal;
    asynStatus status;
    // Current: If we do a getDoubleParam() first we get an error 10. If we do a setDoubleParam() then a getDoubleParam(), then
    // it is successful, and you only need to set once!
    // status = getDoubleParam(paramIndex, SnapshotValueIndex, &curVal);
    // std::cout << "4: " << status << "\n";
    // status = setDoubleParam(paramIndex, StoredValueIndex, curVal); 
    // std::cout << "0: " << status << "\n";
    status = getDoubleParam(StoredValueIndex, &curVal);
    std::cout << "1: " << status << "\n";
    std::cout << "curVal after get : " << curVal << "\n"; 
    status = setDoubleParam(SnapshotValueIndex, curVal);
    std::cout << "2: " << status << "\n";
    callParamCallbacks();

    // >>>>>>>>>>>>>>>>>>>>


    // Get and set values
    // uint32_t paramIndex;
    // for (paramIndex = 0; paramIndex < FEL_PARAMS_SIZE; paramIndex++) {
    //     double curVal;
    //     // Get value from device storage PVs
    //     getDoubleParam(paramIndex, StoredValueIndex, &curVal);

    //     // Set value to device desired snapshot PVs
    //     setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    // }

}

// Need this function in case ops want to access pvs directly
// asynStatus GuardianDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
//     asynStatus status = asynSuccess;
//     int addr;
//     getAddress(pasynUser, &addr);

//     if (pasynUser->reason == _)
    
// }
 
// TODO: Convert this to an asynStatus function
void GuardianDriver::FELpulseEnergyMonitor(void)
{
    /* Notes
    FELpulseEnergyMonitor() lines 7 - 337
    Definition: Within script
    What: This is the 'main' function, and after initalization, and initalizes a snapshot of current values 
            and stores them into 'stats' it then goes into a 'while forever' loop that 
            consistently grabs updates from all of its pvs and stores them into 'stored'.
            They are then passed into 'trip_logic(stored, stats)' returns boolean 'trip', and string 'out'.
            Finally, write 'trip' value to the MPS:UNDS:1:SXRSS_GUARDIAN_OK and the 'out' message to display.
    Conversion: seems like they run the script after they've set their values for the devices. We can make a pv
                that controls whether or not this guardian runs. And just use that as a conditional in the while loop.
    
    */

   /* 1) Initial Snapshot
   Original: When script is started, the while true loop begins.
             And only in the FIRST cycle, is the initial snapshot taken. OR
             If Ops want to take another snapshot, they will write to a PV that allows them to take another snapshot
             Then save the snapshot data EVERY cycle to a structure 'stored'. (Unnecessary but may be a matlab thing)

    New:     Make this into its own function. We can keep the functionality, but checking 'cycle' number doesn't seem necessary. Instead we can
                call initalizeSnapshot() before we start the while forever loop. And the while loop can have 
                a condition of checking boolean 'newSnapShot'.             
    */

   /* 2) Retrieve current device data values
   Original: Within loop, call lcaNewMonitorValue() which checks if new values came in. If so, then call lcaGetSmart().
            Then store the values to a structure called 'stats'

   New: Make this into its own function. Get the values but we can use 'I/O interrupt' so that we only grab values if changed. 
        If possible, to avoid making a million asyn parameters, we can make one, but that'll be the template parameter
        And each device data will have its own $(ID). Refer to the MPS central node ioc as an example.
        We can save the values to an array.
   */

    /* 3) Begin trip logic
    Original: Call [trip, out] = trip_logic(stored,stats). And if tripped or not, write to the MPS trip PV, and to the MPS permit PV
                accordingly. Then display the trip message once. 

    New: We can keep this the same for the most part. The real optimization can come in the trip_logic() itself, where lots
            of code is repeated.

    */

   /* 4) Done :)
    However, you will add some configurability here for SC vs NC eventually. But if the only difference is the tolerance levels,
    then you only need to modify the trip_logic(). 
   
   */

    // PATRICK TODO: try get and set, then call takeSnapshot()

    // Initalize parameters (you have to set the param, then )
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
        sleep(10); // TEMP

        // 5) Check snapshot_trg param, take snapshot then reset if true
        uint32_t snapshotTriggerVal;
        getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1);
        std::cout << "sstriggerval: " << snapshotTriggerVal << "\n"; // TEMP
        if (snapshotTriggerVal == 1) {
            pGDriver->takeSnapshot();
            setUIntDigitalParam(SnapshotTriggerIndex, 0, 1);
            callParamCallbacks();

            getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1); // TEMP

            std::cout << "Successfully triggered and resetted\n\n"; // TEMP
        }

        // 6) Check configuration

        // float current_device_data; 
        // retrieve_device_data(&current_device_data);

        // bool trip;
        // std::string trip_msg;
        // trip_logic(&snapshot_device_data, &current_device_data, &trip, &trip_msg);
    }


//    // Counter code - temporary
//     int step;
//     double count;
//     while(1)
//     {
//         asynStatus status;
//         // PATRICK - seems like the first getDoubleParam always has a status of 10, but once you use setDoubleParam()
//         // and callParamCallBacks() then it grabs the value

//         sleep(5); //Necessary because the thread will go faster than I/O Scan will accept and write inputs
//         status = getDoubleParam(CounterIndex, &count);
//         getIntegerParam(StepIndex, &step);
//         std::cout << status << "\n";
//         std::cout << "count: " << count << '\n';
 
//         setDoubleParam(CounterIndex, count + step);
//         setIntegerParam(StepIndex, step + 1);
//         callParamCallbacks();
//     }


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