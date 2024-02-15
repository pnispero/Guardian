/* G*/

#include "GuardianDriver.h"
#include <unistd.h>
 
void FELpulseEnergyMonitor(void *driverPointer); //Necessary otherwise we get an error about functions being declared


static GuardianDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. global
 
GuardianDriver::GuardianDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      FEL_PARAMS_SIZE, // number to specify maximum parameter lists (starting from 0)
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
    createParam(TOLERANCE_VALUE_STRING, asynParamFloat64, &ToleranceValueIndex);
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
    for (paramIndex = 0; paramIndex < FEL_PARAMS_SIZE; paramIndex++) {

        // Get value from device storage PVs
        status = getDoubleParam(paramIndex, StoredValueIndex, &curVal);
        std::cout << "curVal after get : " << curVal << "\n"; 

        // Set device desired snapshot PVs to value
        status = setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    }
    callParamCallbacks();
}

/* Trip Logic functions */

// Type 1
bool GuardianDriver::outsideTolerancePercentage(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.BC1_Etols * 0.01;
    // qq = stats.BC1_energy_state;
    // QQ = stored.BC1_energy_setpt;
    // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
    //     trip = 1;
    //     out.message = 'BC1 energy feedback state outside stored range';
    // end
    //     // Type 1
        
    //     // TripParam curParam = tripParams[key];
    //     // uint32_t condition;
    //     // getUIntDigitalParam(curParam->conditionId, ConditionValueIndex, &condition) // Optional condition
    //     // if (condition = 1) {
    //     //     double curTol;
    //     //     getDoubleParam(curParam->tolId, ToleranceValueIndex, &curTol); // Tolerance 'control' pvs

    //     //     double curDeviceVal;
    //     //     getDoubleParam(curParam->deviceDataId, StoredValueIndex, &curDeviceVal); // device pvs
    //     //     curDeviceVal = abs(curDeviceVal);

    //     //     double desiredDeviceVal;
    //     //     getDoubleParam(curParam->snapshotId, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs
    //     //     desiredDeviceVal = abs(desiredDeviceVal);
        
    //     //     double tolCompareVal = curTol * tolVal;

    //     //     // Ensure current device value is within desired tolerance
    //     //     if (curDeviceVal > (desiredDeviceVal*tolCompareVal + desiredDeviceVal) 
    //     //         ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolCompareVal))
    //     //     {
    //     //         tripped = true;
    //     //         tripMsg = curParam->tripMsg;
    //     //     }
    //     // }
    return false;
}

// Type 6
bool GuardianDriver::outsideAbsTolerancePercentage(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.L1Sphasetols * 0.01;
    // qq = abs(stats.L1S_phase_setpt);
    // QQ = abs(stored.L1S_phase_setpt);
    // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
    //     trip = 1;
    //     out.message = 'L1S phase setpoint has been changed. Check FEL pulse energy.';
    //     return
    // end

    return false;
}

// Type 3
bool GuardianDriver::outsideAbsTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.BC2_verntols;
    // qq = abs(stats.BC2_vernier);
    // QQ = abs(stored.BC2_vernier);
    // if qq > (tols + QQ) || qq < (QQ - tols)
    //     trip = 1;
    //     out.message = 'BC2 Vernier (SXR) has been changed. Check FEL pulse energy.';
    //     return
    // end

    return false;
}

// Type 4
bool GuardianDriver::outsideDegreeTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.LHwaveplatetols;
    // qq = stats.LH1_waveplate;
    // QQ = stored.LH1_waveplate;
    // if abs(QQ - qq) > abs(tols*QQ)
    //     trip = 1;
    //     out.message = 'Waveplate (LH1) angle has been changed. Check FEL pulse energy';
    //     return
    // end

    return false;
}

// Type 5
bool GuardianDriver::outsideQuadsTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // for iquad = 1:26
    //     qq = stats.CQMQctrl(iquad);
    //     QQ = stored.CQMQctrl(iquad);
    //     tols = 0.001;
    //     if abs(QQ - qq) > abs(tols*QQ)
    //         trip = 1;
    //         out.CQMQtrip = 1;
    //         tweakedQuad = stats.CQMQpv(iquad);
    //         out.message = ['No tweaking the CQs or matching QUADs! I see you  ', char(tweakedQuad)];
    //     end
    // end

    return false;
}

// Type 2
bool GuardianDriver::dataUnchanged(uint32_t deviceDataId, uint32_t snapshotId) {
    // if stats.BC1_vernier ~= stored.BC1_vernier
    //     trip = 1;
    //     out.message = 'BC1 vernier setpoint has been changed. Not Allowed.';
    //     return
    // end

    return false;
}

void GuardianDriver::tripLogic() {

    // 1) Loop through every tripParam in tripParamMap

    // uint32_t key;
    // double tolVal = 0.01;
    // std::string tripMsg = "N/A";
    // bool tripped = false;
    // for (key = 0; key < tripParams.size(); key++) {

    // }

    // if (tripped) {

    //     // 1) write to trip PV
    //     setDoubleParam(tripPv, 1);

    //     // 2) write to MPS pv to turn off beam
    //     setDoubleParam(mpsPermitPv, 0);

    //     // 3) write to out message pv - only on initial trip
    //     // TODO: may make some logic so that when this trips, you can pause the guardian until it is manually resumed
    //     setStringParam(outMessagePv, tripMsg);

    //     callParamCallbacks();
    // }
    // else {
    //     // 1) write to trip PV
    //     setDoubleParam(tripPv, 0);

    //     // 2) write to MPS pv to leave as is
    //     setDoubleParam(mpsPermitPv, 1);

    //     // 3) write to out message pv - only on initial trip
    //     setStringParam(outMessagePv, "No Issues");

    //     callParamCallbacks();
    // }
}

void GuardianDriver::initializeNCTripParamMap() {

    // Initialize 9 undulators

    std::map<uint32_t, TripParam> testMap;
    testMap.emplace(0, TripParam(logicType::outsideTolerancePercentage, 31, 31, "Undulator 1 K value has changed", 0, 11));
    Patrick - Actually make it so a user can add logic just by adding to the .substitutions file. So Omit this dictionary
    x) Send email to tonee about the logic types and why they are different. 
    x) Mentioned hxr line may eventually use this guardian sioc as well (or may make it an engine)
    1) Goal of making it as seamless as we can to add parameters
        1.1) Change the #define FEL_PARAMS_SIZE and add it in the constructor of GuardianDriver by using findNumParam() instead
        1.2) Also add tripMsg as a pv, we can just make another template
        1.2) we can make the logic type a pv so it can be added to the substitutions file as well. ALSO try to make that 
            logicType pv read only in channel access 
        1.3) double check that there isnt any parameters that compare to different ones (i.e. current vs stored are the same parameter)

    // for (int undIndex = 1; undIndex < 10; undIndex++) {
    //     NCTripParamMap.emplace(1, TripParam(outsideTolerancePercentage, 31, 31, "Undulator 1 K value has changed", 0, 11));
    // }
}
void GuardianDriver::initializeSCTripParamMap() {
    
}

// TODO: Convert this to an asynStatus function
void GuardianDriver::FELpulseEnergyMonitor(void)
{

    // Initalize parameters

    pGDriver->initializeNCTripParamMap();

    uint32_t paramIndex = 0;
    double curVal = 1;
    for (paramIndex = 0; paramIndex < FEL_PARAMS_SIZE; paramIndex++) {
        setDoubleParam(paramIndex, StoredValueIndex, curVal);
        setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    }
    for (paramIndex = 0; paramIndex < TOL_PARAMS_SIZE; paramIndex++) {
        setDoubleParam(paramIndex, ToleranceValueIndex, curVal);
    }
    callParamCallbacks(); // Call this to write the values back into epics records

    sleep(10); // Wait some time for every device data pv to populate with real values
    // TODO: Actually test if you need to initialize parameters with setDoubleParam.
    // Maybe since it has 'PINI' field its already initialized

    while (true) {
        sleep(10); // TEMP // TODO: instead of hardcode 10, make it a pv instead, so the cycle speed is adjustable

        // 5) Check snapshot_trg param, take snapshot then reset if true
        uint32_t snapshotTriggerVal;
        getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1);
        if (snapshotTriggerVal == 1) {
            pGDriver->takeSnapshot();
            setUIntDigitalParam(SnapshotTriggerIndex, 0, 1);
            callParamCallbacks();

            getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1); // TEMP

            std::cout << "Successfully triggered and resetted\n\n"; // TEMP
        }

        // TODO: You'd pass in 'mode' here (NC vs SC)
        pGDriver->tripLogic();
    }

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
        NEW: the device data values update on their own. (anytime the actual device data pv changes)
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

    // int numParams;
    // getNumParams(&numParams);
    // std::cout << "\nNum params: " << numParams; // TEMP

}

// TODO: Need this function in case ops want to access pvs directly
// asynStatus GuardianDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
//     asynStatus status = asynSuccess;
//     int addr;
//     getAddress(pasynUser, &addr);

//     if (pasynUser->reason == _)
    
// }
 

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