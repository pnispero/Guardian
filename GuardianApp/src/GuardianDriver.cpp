/* G*/

#include "GuardianDriver.h"
#include <unistd.h>
 
void FELpulseEnergyMonitor(void *driverPointer); //Necessary otherwise we get an error about functions being declared
void watchdog(void *driverPointer);


static GuardianDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. global
 
GuardianDriver::GuardianDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      1000, // number to specify maximum parameter lists (starting from 0)
                                                      asynOctetMask | asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
                                                      asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask, // interruptMask
                                                      ASYN_MULTIDEVICE, // asynFlags
                                                      1, // autoConnect
                                                      0, // priority
                                                      0 // stackSize
                                                     ),
                                                     heartbeatCnt(0)
{
    createParam(MONITOR_CYCLE_STRING, asynParamFloat64, &MonitorCycleIndex);
    createParam(SNAPSHOT_TRIGGER_STRING, asynParamUInt32Digital, &SnapshotTriggerIndex);
    createParam(DEVICE_PARAM_SIZE_STRING, asynParamInt32, &DeviceParamSizeIndex);
    createParam(CONDITION_PARAM_SIZE_STRING, asynParamInt32, &ConditionParamSizeIndex);
    createParam(TOLERANCE_PARAM_SIZE_STRING, asynParamInt32, &ToleranceParamSizeIndex);
    createParam(STORED_VALUE_STRING, asynParamFloat64, &StoredValueIndex);
    createParam(SNAPSHOT_VALUE_STRING, asynParamFloat64, &SnapshotValueIndex);
    createParam(CONDITION_VALUE_STRING, asynParamUInt32Digital, &ConditionValueIndex);
    createParam(TOLERANCE_VALUE_STRING, asynParamFloat64, &ToleranceValueIndex);
    createParam(LOGIC_TYPE_VALUE_STRING, asynParamInt32, &LogicTypeValueIndex);
    createParam(TOLERANCE_ID_STRING, asynParamInt32, &ToleranceIdIndex);
    createParam(CONDITION_ID_STRING, asynParamInt32, &ConditionIdIndex);
    createParam(TRIP_MSG_STRING, asynParamOctet, &TripMsgIndex);
    createParam(MPS_TRIP_STRING, asynParamUInt32Digital, &MpsTripIndex);
    createParam(MPS_PERMIT_STRING, asynParamUInt32Digital, &MpsPermitIndex);
    createParam(HEARTBEAT_VALUE_STRING, asynParamInt32, &HeartbeatValueIndex);
    createParam(ARM_VALUE_STRING, asynParamUInt32Digital, &ArmValueIndex);

    asynStatus status;
    status = (asynStatus)(epicsThreadCreate("FELpulseEnergyMonitor", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::FELpulseEnergyMonitor, this) == NULL);
    if (status)
    {
        std::cout << status << "\n";
        return;
    }

}

void FELpulseEnergyMonitor(void* driverPointer)
{
    pGDriver->FELpulseEnergyMonitor();
}

/* Trip Logic functions */

// Type 1
bool GuardianDriver::outsidePercentageTolerance(int paramIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.BC1_Etols * 0.01;
        // qq = stats.BC1_energy_state;
        // QQ = stored.BC1_energy_setpt;
        // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
        //     trip = 1;
        //     out.message = 'BC1 energy feedback state outside stored range';
        // end

    int tolId; double tolVal, curDeviceVal, desiredDeviceVal;
    getIntegerParam(paramIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(paramIndex, StoredValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(paramIndex, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolValPercent + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolValPercent))
    {
        return true;
    }
    return false;
}

// Overload - used for special cases (comparing 2 different device pvs)
bool GuardianDriver::outsidePercentageTolerance(int curValIndex, int desiredValIndex) {
    int tolId; double tolVal, curDeviceVal, desiredDeviceVal;
    getIntegerParam(curValIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(curValIndex, StoredValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(desiredValIndex, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolValPercent + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolValPercent))
    {
        return true;
    }
    return false;
}

// Type 2
bool GuardianDriver::outsideAbsPercentageTolerance(int paramIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.BC1_Etols * 0.01;
        // qq = abs(stats.BC1_energy_state);
        // QQ = abs(stored.BC1_energy_setpt);
        // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
        //     trip = 1;
        //     out.message = 'BC1 energy feedback state outside stored range';
        // end

    int tolId; double tolVal, curDeviceVal, desiredDeviceVal;
    getIntegerParam(paramIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(paramIndex, StoredValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(paramIndex, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs
    curDeviceVal = abs(curDeviceVal);
    desiredDeviceVal = abs(desiredDeviceVal);

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolValPercent + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolValPercent))
    {
        return true;
    }
    return false;
}

// Type 3
bool GuardianDriver::outsideAbsValueTolerance(int paramIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.BC2_verntols;
        // qq = abs(stats.BC2_vernier);
        // QQ = abs(stored.BC2_vernier);
        // if qq > (tols + QQ) || qq < (QQ - tols)
        //     trip = 1;
        //     out.message = 'BC2 Vernier (SXR) has been changed. Check FEL pulse energy.';
        //     return
        // end

    int tolId; double tolVal, curDeviceVal, desiredDeviceVal;
    getIntegerParam(paramIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(paramIndex, StoredValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(paramIndex, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs
    curDeviceVal = abs(curDeviceVal);
    desiredDeviceVal = abs(desiredDeviceVal);

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (tolVal + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - tolVal))
    {
        return true;
    }
    return false;
}

// Type 4
bool GuardianDriver::outsideAbsDifferenceTolerance(int paramIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.LHwaveplatetols;
        // qq = stats.LH1_waveplate;
        // QQ = stored.LH1_waveplate;
        // if abs(QQ - qq) > abs(tols*QQ)
        //     trip = 1;
        //     out.message = 'Waveplate (LH1) angle has been changed. Check FEL pulse energy';
        //     return
        // end

    int tolId; double tolVal, curDeviceVal, desiredDeviceVal;
    getIntegerParam(paramIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(paramIndex, StoredValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(paramIndex, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > abs(tolVal + desiredDeviceVal)
        ||  curDeviceVal < abs(desiredDeviceVal - tolVal))
    {
        return true;
    }
    return false;
}

// Special case
bool GuardianDriver::outsideCollimatorTolerance() {
    int tolId; double tolVal;
    double curLeftColVal, curRightColVal, desiredLeftColVal, desiredRightColVal;
    getIntegerParam(19, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(19, StoredValueIndex, &curLeftColVal); // device pvs
    getDoubleParam(20, StoredValueIndex, &curRightColVal); // device pvs
    getDoubleParam(19, SnapshotValueIndex, &desiredLeftColVal); // snapshot pvs
    getDoubleParam(20, SnapshotValueIndex, &desiredRightColVal); // snapshot pvs

    // Check BC1 colls are within 0.030 mm (or user entered delta) of stored position
    if (curLeftColVal > (tolVal + desiredLeftColVal) ||
        curLeftColVal < (desiredLeftColVal - tolVal) ||
        curRightColVal > (tolVal + desiredRightColVal) ||
        curRightColVal < (desiredRightColVal - tolVal))
    {
        return true;
    }
    return false;
}

// These special cases will have hardcoded numbers here (can't generalize these unfortunately)
std::tuple<bool, std::string> GuardianDriver::tripSpecialCase(int paramIndex) {

    // Special cases have a condition
    // Get value of condition

    uint32_t conditionVal, conditionVal2;
    bool tripped = false; std::string tripMsg = "No Issues";
    switch (paramIndex) 
    {
    case 0: // devices 1,2,3 as well
        getUIntDigitalParam(1, ConditionValueIndex, &conditionVal, 1);
        getUIntDigitalParam(2, ConditionValueIndex, &conditionVal2, 1);
        if (conditionVal == 1) {
            tripped = pGDriver->outsidePercentageTolerance(0); // Check bunch charge feedback setpoint unchanged
            // getStringParam(0, TripMsgIndex, tripMsg) // TODO
            if (tripped) break;
            tripped = pGDriver->outsidePercentageTolerance(1, 0); // Check bunch charge feedback state within user entered % of stored setpt
            // getStringParam(1, TripMsgIndex, tripMsg) // TODO
            break;
        }
        else if (conditionVal2 == 1) {
            tripped = pGDriver->outsidePercentageTolerance(2); // Check matlab bunch charge feedback setpoint unchanged
            // getStringParam(2, TripMsgIndex, tripMsg) // TODO
            if (tripped) break;
            tripped = pGDriver->outsidePercentageTolerance(3, 2); // Check matlab bunch charge feedback state within user entered % of stored
            // getStringParam(3, TripMsgIndex, tripMsg) // TODO
            break;
        }
        else { tripMsg = "WARNING: Neither bunch charge feedback active!"; }
        break;
    case 4: // devices 5,6 as well
        getUIntDigitalParam(3, ConditionValueIndex, &conditionVal, 1);
        if (conditionVal == 1) {
            tripped = pGDriver->outsidePercentageTolerance(5, 4); // Check if BC1 energy feedback is on, then check that the SXR state is within (tols)% of stored setpoint
            // getStringParam(5, TripMsgIndex, tripMsg) // TODO
            if (tripped) break;
            tripped = pGDriver->outsidePercentageTolerance(6); // Check BC1 vernier setpoint unchanged 
            // getStringParam(6, TripMsgIndex, tripMsg) // TODO
            break;
        }
        else { tripMsg = "WARNING: BC1 Energy Feedback is OFF"; }
    case 7: // devices 8,9 as well
        getUIntDigitalParam(4, ConditionValueIndex, &conditionVal, 1);
        getUIntDigitalParam(5, ConditionValueIndex, &conditionVal2, 1);
        if (conditionVal == 1) {
            // Use the appropriate setpoint (ACT is always the SXR one) and
            // check BC1 current feedback state within (tols)% of stored setpoint
            tripped = pGDriver->outsidePercentageTolerance(8, 7); 
            if (conditionVal2 == 1) {
                tripped = pGDriver->outsidePercentageTolerance(8, 9); 
            }
            // getStringParam(5, TripMsgIndex, tripMsg) // TODO
            if (tripped) break;
            tripped = pGDriver->outsidePercentageTolerance(7);
            // getStringParam(5, TripMsgIndex, tripMsg) // TODO
            break;
        }
        else { tripMsg = "WARNING: BC1 Bunch Current Feedback is OFF"; }
    case 12: // devices 13,14 as well
        getUIntDigitalParam(6, ConditionValueIndex, &conditionVal, 1);
        getUIntDigitalParam(7, ConditionValueIndex, &conditionVal2, 1);
        if (conditionVal == 1) {
            double curVal;
            getDoubleParam(14, StoredValueIndex, &curVal);
            if (curVal > 25000) { // only check if BLEN reading is OK
                tripMsg = "WARNING: BC2 bunch current reading is Garbage!";
                break;
            }
            // Using the appropriate setpoint,
            // check first that it hasn't changed, then check that the BC2 current feedback 
            // state is within (tols)% of stored setpoint
            if (conditionVal2 == 1) {
                tripped = pGDriver->outsidePercentageTolerance(13);
                if (tripped) break;
            }
            else {
                tripped = pGDriver->outsidePercentageTolerance(12);
                break;
            }
        }
        else { tripMsg = "WARNING: BC2 Bunch Current Feedback is OFF"; }
    case 19: // devices 20 as well
        tripped = pGDriver->outsideCollimatorTolerance();
        break;
    default:
        break;
    }

    return std::make_tuple(tripped, tripMsg);
}

void GuardianDriver::tripLogic() {

    // 1) Loop through every deviceParam
    bool tripped = false; std::string tripMsg = "No Issues";
    for (int paramIndex = 0; paramIndex < 2; paramIndex++) { // TEMP set to 2 for testing
    // for (int paramIndex = 0; paramIndex < DEVICE_PARAMS_SIZE; paramIndex++) {
    
        // get value of logic type
        int logicType;
        getIntegerParam(paramIndex, LogicTypeValueIndex, &logicType);
        std::cout << "Logic type; " << logicType << "\n"; // TEMP

        switch (logicType)
        {
        case 0: // special case
            std::tie(tripped, tripMsg) = pGDriver->tripSpecialCase(paramIndex);
            break;
        case 1:
            tripped = pGDriver->outsidePercentageTolerance(paramIndex);
            break;
        case 2: 
            tripped = pGDriver->outsideAbsPercentageTolerance(paramIndex);
            break;
        case 3:
            tripped = pGDriver->outsideAbsValueTolerance(paramIndex);
            break;
        case 4: 
            tripped = pGDriver->outsideAbsDifferenceTolerance(paramIndex);
            break;
        default:
            std::cout << "*ERROR* Invalid logic type number: " << logicType
            << " for parameter: " << paramIndex << "\n";                                                                                                                                                          
            break;
        }
        if (tripped) {
            // 0) get msg
            if (tripMsg != "") { // If special case then message already filled
                // getStringParam(tripMsg) // TODO: will use waveform instead
            }
            
            // 1) write to trip PV
            setUIntDigitalParam(MpsTripIndex, 1, 1); // original trip PV

            // 2) write to MPS pv to turn off beam
            setUIntDigitalParam(MpsPermitIndex, 0, 1); // Tell MPS to turn off beam
            std::cout << "OFF BEAM\n"; // TEMP

            // // 3) write to out message pv - only on initial trip
            // setStringParam(outMessagePv, tripMsg); // TODO: This will be a waveform instead
            std::cout << "Trip Msg: " << tripMsg << "\n"; // TEMP

            callParamCallbacks();
            sleep(5); //TEMP
            return;
        }
    }

    /* Not Tripped */ 
    // 1) write to trip PV
    setUIntDigitalParam(MpsTripIndex, 0, 1); // original trip PV

    // 2) write to MPS pv to leave as is
    setUIntDigitalParam(MpsPermitIndex, 1, 1); // Tell MPS to turn off beam

    // 3) write to out message pv - only on initial trip
    if (tripMsg != "") { // If special case then message already filled
        // getStringParam(tripMsg) // TODO: will use waveform instead
    }
    // setStringParam(outMessagePv, tripMsg); // TODO: This will be a waveform instead

    callParamCallbacks();
}


void GuardianDriver::takeSnapshot()
{
    double curVal;
    for (int paramIndex = 0; paramIndex < DEVICE_PARAMS_SIZE; paramIndex++) {

        // Get value from device storage PVs
        getDoubleParam(paramIndex, StoredValueIndex, &curVal);
        std::cout << "curVal after get : " << curVal << "\n"; 

        // Set device desired snapshot PVs to value
        setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    }
    callParamCallbacks();
}

void GuardianDriver::initGuardian() {
    /* Initialize */
    sleep(5); // IMPORTANT - Let epics records load then initialize
    std::cout << "Initializing Guardian Please Wait...\n\n";

    getIntegerParam(DeviceParamSizeIndex, &DEVICE_PARAMS_SIZE);
    getIntegerParam(ToleranceParamSizeIndex, &TOL_PARAMS_SIZE); // TODO: Possible we may not use this
    getIntegerParam(ConditionParamSizeIndex, &CONDITION_PARAMS_SIZE); // TODO: Possible we may not use this

    setUIntDigitalParam(MpsPermitIndex, 1, 1); // Initialize mps permit to 1
    
    callParamCallbacks(); // Call this to write the values back into epics records

    sleep(5); // Wait some time for every device data pv to populate with real values
    std::cout << "\nFinished initialization.\n\n";

}

void GuardianDriver::FELpulseEnergyMonitor(void)
{
    pGDriver->initGuardian();
    uint32_t snapshotTriggerVal, armVal;
    while (true) {
        getDoubleParam(MonitorCycleIndex, &MONITOR_CYCLE_TIME);
        usleep(MONITOR_CYCLE_TIME);
      
        // Proceed only if guardian is 'armed'
        do {
            getUIntDigitalParam(ArmValueIndex, &armVal, 1);
        } while (armVal == 0);
        
        // heartbeat
        heartbeatCnt++;

        // Check snapshot_trg param, take snapshot then reset if true
        getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1);
        if (snapshotTriggerVal == 1) {
            pGDriver->takeSnapshot();
            setUIntDigitalParam(SnapshotTriggerIndex, 0, 1);
            callParamCallbacks();
        }
        // TODO: You'd pass in 'mode' here (NC vs SC)
        pGDriver->tripLogic();
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